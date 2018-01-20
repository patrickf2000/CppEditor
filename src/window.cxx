// Copyright 2017-2018 Patrick Flynn
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, 
//	this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this 
//	list of conditions and the following disclaimer in the documentation and/or 
//	other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may 
//	be used to endorse or promote products derived from this software 
//	without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#include <QIcon>
#include <QPixmap>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QStringList>
#include <QMessageBox>
#include <QApplication>
#include <QMenuBar>
#include <QFileInfo>
#include <QFile>
#ifdef _WIN32
#include <settings.hh>
#else
#include <cpplib/settings.hh>
#endif

#include "window.hh"
#include "tabpane.hh"
#include "main_toolbar.hh"
#include "global/file_actions.hh"
#include "global/recent.hh"
#include "global/css.hh"
#include "menus/system_tray_menu.hh"
#include "lang/lang-parser.hh"
#include "icon.hh"

using namespace CppLib;

QStatusBar *Window::statusbar;
QLabel *Window::modLabel, *Window::pathLabel, *Window::lineCountLabel;
QSplitter *Window::centralSplitter;
ProjectPane *Window::projectPane;
QTextEdit *Window::richTextPane;
FinderToolbar *Window::finderToolbar;
DateDockWidget *Window::dateDockWidget;
#ifndef _WIN32
ConsoleWin *Window::consoleWidget;
#endif
TemplateMenu *Window::templateMenu;
QSystemTrayIcon *Window::trayIcon;

Window::Window() {
    this->setWindowTitle(trans("CppEditor"));
    this->setWindowIcon(IconManager::getIcon("accessories-text-editor"));
    int winX = Settings::getSetting("window/winX","700").toInt();
    int winY = Settings::getSetting("window/winY","600").toInt();
    this->resize(winX,winY);
    qApp->setStyleSheet(CssManager::stylesheetContent("window"));

    filemenu = new FileMenu(this);
    editmenu = new EditMenu;
    insertmenu = new InsertMenu(this);
    viewmenu = new ViewMenu(this);
    settingsmenu = new SettingsMenu;
    templateMenu = new TemplateMenu;
#ifdef _WIN32
    windowsMenu = new WindowsMenu;
#endif
    helpmenu = new HelpMenu;

    this->menuBar()->setContextMenuPolicy(Qt::PreventContextMenu);
    this->menuBar()->addMenu(filemenu);
    this->menuBar()->addMenu(editmenu);
    this->menuBar()->addMenu(insertmenu);
    this->menuBar()->addMenu(viewmenu);
    this->menuBar()->addMenu(settingsmenu);
    this->menuBar()->addMenu(templateMenu);
#ifdef _WIN32
    this->menuBar()->addMenu(windowsMenu);
#endif
    this->menuBar()->addMenu(helpmenu);

    statusbar = new QStatusBar();
    statusbar->setContextMenuPolicy(Qt::PreventContextMenu);
    this->setStatusBar(statusbar);

    modLabel = new QLabel();
    pathLabel = new QLabel();
    lineCountLabel = new QLabel("Current Line: 1");

    statusbar->addWidget(pathLabel);
    statusbar->addWidget(modLabel);
    statusbar->addPermanentWidget(lineCountLabel);

    centralSplitter = new QSplitter();
    this->setCentralWidget(centralSplitter);

    TabPane *tabs = new TabPane(this);
    projectPane = new ProjectPane;
    projectPane->hide();

    centralSplitter->addWidget(projectPane);
    centralSplitter->addWidget(tabs);

    richTextPane = new QTextEdit;
    richTextPane->setAcceptRichText(true);
    richTextPane->setReadOnly(true);
    richTextPane->setVisible(false);
    centralSplitter->addWidget(richTextPane);

    MainToolBar *toolbar = new MainToolBar(this);
    toolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    toolbar->setFloatable(false);
    this->addToolBar(Qt::TopToolBarArea, toolbar);

    dateDockWidget = new DateDockWidget;
    dateDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    if (QVariant(Settings::getSetting("subwindows/date_selector","true")).toBool()) {
        dateDockWidget->hide();
    } else {
        dateDockWidget->show();
    }
    this->addDockWidget(Qt::LeftDockWidgetArea,dateDockWidget);
#ifndef _WIN32
    consoleWidget = new ConsoleWin;
    if (QVariant(Settings::getSetting("subwindows/console","true")).toBool()) {
        consoleWidget->hide();
    } else {
        consoleWidget->show();
    }
    this->addDockWidget(Qt::BottomDockWidgetArea,consoleWidget);
#endif
    trayIcon = new QSystemTrayIcon();
    SystemTrayMenu *sysTrayMenu = new SystemTrayMenu(this);
    trayIcon->setContextMenu(sysTrayMenu);

    trayIcon->setIcon(IconManager::getIcon("accessories-text-editor"));
    bool showTrayIcon = QVariant(Settings::getSetting("systray/display","true")).toBool();
    trayIcon->setVisible(showTrayIcon);

    finderToolbar = new FinderToolbar;
    finderToolbar->setAllowedAreas(Qt::BottomToolBarArea);
    finderToolbar->setVisible(false);
    finderToolbar->setMovable(false);
    this->addToolBar(Qt::BottomToolBarArea,finderToolbar);
}

Window::~Window() {
    delete statusbar;
    delete centralSplitter;
    delete finderToolbar;
    delete dateDockWidget;
    delete templateMenu;
    delete filemenu;
    delete editmenu;
    delete insertmenu;
    delete viewmenu;
    delete settingsmenu;
#ifdef _WIN32
    delete windowsMenu;
#endif
    delete helpmenu;
}

void Window::setTitle(QString title) {
    QString t = title + " [CppEditor]";
    this->setWindowTitle(t);
}

void Window::addFile(QString path) {
    if ((TabPane::currentWidget()->isUntitled())&&(TabPane::currentWidget()->isModified()==false)) {
        TabPane::setCurrentTabPath(path);
        TabPane::setCurrentTabTitle(QFileInfo(path).fileName());
    } else {
        TabPane::addNewTab(path);
    }
    TabPane::setCurrentTabText(FileActions::fileContents(path));
    TabPane::currentWidget()->setModified(false);
}

void Window::setStatusBarModified(bool modified) {
    if (modified) {
        modLabel->setText("unsaved");
    } else {
        modLabel->setText("saved");
    }
}

void Window::setStatusBarPath(QString path) {
    pathLabel->setText(path);
}

void Window::setStatusBarLineCount(int count) {
    QString strCount = QString::number(count);
    lineCountLabel->setText("Current Line: "+strCount);
}

bool Window::checkSave() {
    bool found = false;
    QStringList items;
    int count = TabPane::tabs->count();
    for (int i = 0; i<count; i++) {
        if (TabPane::widgetAt(i)->isModified()) {
            found = true;
            items.push_back(TabPane::widgetAt(i)->path());
        }
    }

    if (found) {
        QString details = "";
        for (int i = 0; i<items.size(); i++) {
            details+=items.at(i)+"\n";
        }

        QMessageBox msg;
        msg.setWindowTitle("Warning!");
        msg.setText("You have unsaved documents!\n"
                    "Do you wish to exit?");
        msg.setDetailedText(details);
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.setDefaultButton(QMessageBox::No);
        msg.setIcon(QMessageBox::Warning);
        int ret = msg.exec();

        if (ret==QMessageBox::Yes) {
            return true;
        }
        return false;
    }
    return true;
}

void Window::displayProjectPane() {
    if (projectPane->isVisible()) {
        projectPane->hide();
    } else {
        projectPane->show();
    }
}

void Window::setRichTextPane(QString text) {
    richTextPane->setHtml(text);
}

void Window::displayRichTextPane() {
    if (richTextPane->isVisible()) {
        richTextPane->setVisible(false);
    } else {
        richTextPane->setVisible(true);
    }
}

bool Window::isRichTextPaneVisible() {
    return richTextPane->isVisible();
}

void Window::displayFinderToolbar() {
    if (finderToolbar->isVisible()) {
        finderToolbar->setVisible(false);
    } else {
        finderToolbar->setVisible(true);
    }
}

void Window::dispalyDateSelector() {
    if (dateDockWidget->isHidden()) {
        dateDockWidget->show();
    } else {
        dateDockWidget->hide();
    }
}

#ifndef _WIN32
void Window::displayConsole() {
    if (consoleWidget->isHidden()) {
        consoleWidget->show();
    } else {
        consoleWidget->hide();
    }
}
#else
void Window::displayConsole() { }
#endif

void Window::displaySysTrayIcon() {
    if (trayIcon->isVisible()) {
        trayIcon->hide();
    } else {
        trayIcon->show();
    }
}

void Window::appExit(QMainWindow *win, bool quit) {
    Settings::writeSetting("subwindows/date_selector",QVariant(dateDockWidget->isHidden()).toString());
#ifndef _WIN32
    Settings::writeSetting("subwindows/console",QVariant(consoleWidget->isHidden()).toString());
#endif
    Settings::writeSetting("window/winX",QString::number(win->width()));
    Settings::writeSetting("window/winY",QString::number(win->height()));
    Recent::write();
    delete trayIcon;
    if (quit) {
    	qApp->exit(0);
    }
}

void Window::closeEvent(QCloseEvent *event) {
    if (checkSave()) {
    	appExit(this,false);
        event->accept();
    } else {
        event->ignore();
    }
}

void Window::keyPressEvent(QKeyEvent *event) {
    if ((event->modifiers()==Qt::ControlModifier)&&(event->key()==Qt::Key_Q)) {
        if (checkSave()) {
            qApp->exit(0);
        }
    } else if ((event->modifiers()==Qt::ControlModifier)&&(event->key()==Qt::Key_N)) {
        FileActions::newFile();
    } else if ((event->modifiers()==Qt::ControlModifier)&&(event->key()==Qt::Key_O)) {
        FileActions::openFile();
    } else if ((event->modifiers()==Qt::ControlModifier)&&(event->key()==Qt::Key_S)) {
        FileActions::saveFile();
    } else if (event->modifiers()==Qt::ControlModifier | Qt::ShiftModifier) {
        if (event->key()==Qt::Key_S) {
            FileActions::saveFileAs();
        }
    }
}