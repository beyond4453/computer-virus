
@ECHO OFF 

cl /c inj.c
link /DYNAMICBASE:no inj.obj

cl /c hello.c
link /DYNAMICBASE:no hello.obj user32.lib

cmd.exe
