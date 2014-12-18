#include<windows.h>

#pragma comment(lib, "user32.lib")

char *s = "I'm hacked";
//char *s1  = "user32.dll";
char *s2  = "MessageBoxA";

int main()
{
	__asm{
//		push s1 
		sub esp, 0x20
		mov  [ebp-0x20], 'u'
		mov  [ebp-0x1f], 's'
		mov  [ebp-0x1e], 'e'
		mov  [ebp-0x1d], 'r'
		mov  [ebp-0x1c], '3'
		mov  [ebp-0x1b], '2'
		mov  [ebp-0x1a], '.'
		mov  [ebp-0x19], 'd'
		mov  [ebp-0x18], 'l'
		mov  [ebp-0x17], 'l'
		mov  [ebp-0x16], 0x0
		lea eax, [ebp-0x20]
		push eax

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

		add esp, 0x20
	}
	return 0;

}
