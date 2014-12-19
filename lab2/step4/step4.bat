
@ECHO OFF 
cl /c msg.c
link /DLL  /DYNAMICBASE:no msg.obj

cl /c hellow.c
link /DYNAMICBASE:no hellow.obj msg.lib

cl /c inj.c
link /DYNAMICBASE:no inj.obj
 
cmd.exe
