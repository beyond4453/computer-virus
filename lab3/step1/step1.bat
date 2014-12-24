
@ECHO OFF 

cl /c test.c
link /DYNAMICBASE:no test.obj user32.lib

cmd.exe
