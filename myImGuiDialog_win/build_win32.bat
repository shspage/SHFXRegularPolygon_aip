@REM Build myImGuiDialog standalone dialog app for testing
@REM Build for Visual Studio compiler. Run your copy of vcvars32.bat or vcvarsall.bat to setup command-line compiler.
@set OUT_DIR=Debug
@set OUT_EXE=standalone_myImGuiDialog_win
@set INCLUDES=/I..\imgui /I "%DXSDK_DIR%/Include"
@set SOURCES=main.cpp myImGuiDialog.cpp ..\imgui\examples\imgui_impl_dx9.cpp ..\imgui\examples\imgui_impl_win32.cpp ..\imgui\imgui*.cpp
@set LIBS=/LIBPATH:"%DXSDK_DIR%/Lib/x86" d3d9.lib
mkdir %OUT_DIR%
cl /EHsc /nologo /Zi /MD /utf-8 %INCLUDES% /D UNICODE /D _UNICODE %SOURCES% /Fe%OUT_DIR%/%OUT_EXE%.exe /Fo%OUT_DIR%/ /link %LIBS%
