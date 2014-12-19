
@ECHO OFF 

cl /c inj.c
link /DYNAMICBASE:no inj.obj
 
cmd.exe
