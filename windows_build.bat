@echo off

echo ~~~~~~~~~~ buidling windows layer ~~~~~~~~~~
pushd build
rc /nologo /fo .\res.res ..\source\res.rc
cl /nologo /Zi /D UNICODE ..\source\iss_mandarin_thing.c /link /nologo /subsystem:windows /debug User32.lib Gdi32.lib Winmm.lib	Shlwapi.lib res.res /out:mandarin_flashcards.exe
popd