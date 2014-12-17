
@ECHO OFF 

cl /c main.c
link /DYNAMICBASE:no main.obj

cl /c test.c
link /DYNAMICBASE:no test.obj

cmd.exe
