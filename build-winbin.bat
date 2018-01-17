@echo off
:: Copyright 2017 Patrick Flynn
::
:: Redistribution and use in source and binary forms, with or without modification, 
:: are permitted provided that the following conditions are met:
::
:: 1. Redistributions of source code must retain the above copyright notice, 
::	this list of conditions and the following disclaimer.
::
:: 2. Redistributions in binary form must reproduce the above copyright notice, this 
::	list of conditions and the following disclaimer in the documentation and/or 
::	other materials provided with the distribution.
::
:: 3. Neither the name of the copyright holder nor the names of its contributors may 
::	be used to endorse or promote products derived from this software 
::	without specific prior written permission.
::
:: THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
:: ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
:: WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
:: IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
:: INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
:: NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
:: PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
:: WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
:: ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
:: EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

::Builds a winbin folder
::This folder contains all the files needed to build a distributable CppEditor
::You must do this before building the setup.iss script.

echo Checking for folders...
if exist ".\winbin" rmdir .\winbin
md winbin

echo Copying executable...
if exist ".\build\src\debug\CppEditor.exe" (
	copy .\build\src\debug\CppEditor.exe .\winbin\CppEditor.exe
) else (
	echo Program executable not found.
	echo Please make sure the program is built.
	exit /B
)

echo Copying libraries...
cd winbin

md platforms
cd platforms
copy "C:\Qt\5.10.0\mingw53_32\plugins\platforms\qwindows.dll" .\
cd ..

copy "C:\Qt\5.10.0\mingw53_32\bin\libEGL.dll" .\
copy "C:\Qt\5.10.0\mingw53_32\bin\libgcc_s_dw2-1.dll" .\
copy "C:\Qt\5.10.0\mingw53_32\bin\libstdc++-6.dll" .\
copy "C:\Qt\5.10.0\mingw53_32\bin\libwinpthread-1.dll" .\
copy "C:\Qt\5.10.0\mingw53_32\bin\Qt5Cored.dll" .\
copy "C:\Qt\5.10.0\mingw53_32\bin\Qt5Guid.dll" .\
copy "C:\Qt\5.10.0\mingw53_32\bin\Qt5Widgetsd.dll" .\

cd ..

echo Done
