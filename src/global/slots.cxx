// Copyright 2017 Patrick Flynn
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
#include <QApplication>

#include "slots.hh"
#include "file_actions.hh"
#include "../window.hh"
#include "../tabpane.hh"

void Slots::newFileSlot() {
    FileActions::newFile();
}

void Slots::openFileSlot() {
    FileActions::openFile();
}

void Slots::saveFileSlot() {
    FileActions::saveFile();
}

void Slots::saveFileAsSlot() {
    FileActions::saveFileAs();
}

void Slots::quitSlot(QMainWindow *window) {
    if (Window::checkSave()) {
        Window::appExit(window,true);
    }
}

void Slots::cutSlot() {
    TabPane::currentWidget()->cut();
}

void Slots::copySlot() {
    TabPane::currentWidget()->copy();
}

void Slots::pasteSlot() {
    TabPane::currentWidget()->paste();
}

void Slots::selectAllSlot() {
    TabPane::currentWidget()->selectAll();
}

void Slots::undoSlot() {
    TabPane::currentWidget()->undo();
}

void Slots::redoSlot() {
    TabPane::currentWidget()->redo();
}