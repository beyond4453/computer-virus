#include<windows.h>

#pragma comment(lib, "user32.lib")

char *s = "I'm hacked";

int main()
{
	__asm{
		push 0
		push s 
		push s
		push 0 
		mov  eax, dword ptr [MessageBoxA]
		call eax
//		mov eax, 0x76BCEA11
//		call eax
	}
	return 0;

}
