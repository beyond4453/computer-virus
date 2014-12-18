#include<windows.h>

#pragma comment(lib, "user32.lib")

char *s = "I'm hacked";
char *s1  = "user32.dll";
char *s2  = "MessageBoxA";

int main()
{
	__asm{
		push s1 
		mov  eax, dword ptr [LoadLibraryA]
		call eax
		push s2
		push eax
		mov eax, dword ptr[GetProcAddress]
		call eax
		push 0
		push s
		push s
		push 0
		call eax
	}
	return 0;

}
