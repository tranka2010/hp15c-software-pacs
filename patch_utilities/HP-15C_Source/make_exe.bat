echo off
REM Make an executable wrapper around the TCL file
REM The binaries from kbspkit should be in the system path
REM Reference: https://wiki.tcl-lang.org/page/How+to+compile+a+TCL+script+into+an+EXE+program

SET KBSVQ=C:\Users\pepin\OneDrive\Documents\GitHub\hp15c-software-pacs\patch_utilities\kbskit_MINGW64_NT-10.0-17763-x86_64\bin
SET KBSVQ_CLI_EXE=kbsvq8.6-cli.exe
SET KBSVQ_GUI_EXE=kbsvq8.6-gui.exe

REM Clean up previous compile
del HP-15C.kit
del HP-15C.EXE
rmdir /S /Q HP-15C.vfs

REM Do qwrap
echo Qwrapping...
%KBSVQ%\%KBSVQ_CLI_EXE% %KBSVQ%\sdx.kit qwrap HP-15C.tcl

REM Do unwrap
echo Unwrapping...
%KBSVQ%\%KBSVQ_CLI_EXE% %KBSVQ%\sdx.kit unwrap HP-15C.kit
REM Copy build folders into the .vfs
echo Tranferring build folders...
xcopy "css" "HP-15C.vfs\lib\app-HP-15C\css" /h /i /c /k /e /r /y
xcopy "doc" "HP-15C.vfs\lib\app-HP-15C\doc" /h /i /c /k /e /r /y
xcopy "icons" "HP-15C.vfs\lib\app-HP-15C\icons" /h /i /c /k /e /r /y
xcopy "images" "HP-15C.vfs\lib\app-HP-15C\images" /h /i /c /k /e /r /y
xcopy "lib" "HP-15C.vfs\lib\app-HP-15C\lib" /h /i /c /k /e /r /y
xcopy "logo" "HP-15C.vfs\lib\app-HP-15C\logo" /h /i /c /k /e /r /y
xcopy "msgs" "HP-15C.vfs\lib\app-HP-15C\msgs" /h /i /c /k /e /r /y

REM Copy .ico file into the vfs
echo Installing icons...
mkdir HP-15C.vfs\lib\app-HP-15C\icons
copy HP-15C.ico HP-15C.vfs\lib\app-HP-15C\icons\.

REM DO wrap into .exe (use -gui version of kbsvq for standalone app)
echo Wrapping into .exe
%KBSVQ%\%KBSVQ_CLI_EXE% %KBSVQ%\sdx.kit wrap HP-15C.exe -runtime %KBSVQ%\%KBSVQ_GUI_EXE%


REM Clean up compile
del HP-15C.kit
rmdir /S /Q HP-15C.vfs

echo Done!