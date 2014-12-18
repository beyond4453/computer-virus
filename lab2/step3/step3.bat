
@ECHO OFF 

cl /c main.c
link /DYNAMICBASE:no main.obj


cmd.exe
