@echo off

set opts=-FC -GR- -EHa- -nologo -Zi -DASSERTIONS_ENABLED=1 -DDEBUG_MODE=1
set code=%cd%
pushd build
cl %opts% %code%\win32_minesweeper.cpp -Feminesweeper /link /LIBPATH:..\lib raylibdll.lib winmm.lib
popd
