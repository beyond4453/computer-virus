
@ECHO OFF 

cl /c hello.c
link /DYNAMICBASE:no hello.obj user32.lib

cl /c step1.c
link /DYNAMICBASE:no step1.obj 

cmd.exe
