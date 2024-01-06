@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64

set CodeDir=..\DX12_C++
set OutputDir=..\build_win32

set CompilerFlags=-Od -Zi -nologo
set LinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib

If NOT EXIST %OutputDir% mkdir %OutputDir%

pushd %OutputDir%

del *.pdb > NUL 2> NUL

call cl %CompilerFlags% %CodeDir%\win32_graphics.cpp -Fmwin32_graphics.map /link %LinkerFlags%

popd