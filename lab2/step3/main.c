#include<windows.h>

#pragma comment(lib, "user32.lib")

int main()
{
	__asm{
//push "user32.dll" to the stack 
		sub esp, 0x40
		mov  [ebp-0x40], 'u'
		mov  [ebp-0x3f], 's'
		mov  [ebp-0x3e], 'e'
		mov  [ebp-0x3d], 'r'
		mov  [ebp-0x3c], '3'
		mov  [ebp-0x3b], '2'
		mov  [ebp-0x3a], '.'
		mov  [ebp-0x39], 'd'
		mov  [ebp-0x38], 'l'
		mov  [ebp-0x37], 'l'
		mov  [ebp-0x36], 0x0
		lea ebx, [ebp-0x40]
		push ebx
//call LoadLibraryA , result-->eax
		mov  eax, dword ptr [LoadLibraryA]
		call eax

//push "MessageBoxA" to the stack
		mov  [ebp-0x30], 'M'
		mov  [ebp-0x2f], 'e'
		mov  [ebp-0x2e], 's'
		mov  [ebp-0x2d], 's'
		mov  [ebp-0x2c], 'a'
		mov  [ebp-0x2b], 'g'
		mov  [ebp-0x2a], 'e'
		mov  [ebp-0x29], 'B'
		mov  [ebp-0x28], 'o'
		mov  [ebp-0x27], 'x'
		mov  [ebp-0x26], 'A'
		mov  [ebp-0x25], 0x0
		lea ebx, [ebp-0x30]
		push ebx

//push LoadLibraryA 's result
		push eax
// call GetProcAddress , result --> eax
		mov eax, dword ptr[GetProcAddress]
		call eax

//push "I'm hacked" to the stack
		mov  [ebp-0x20], 'I'
		mov  [ebp-0x1f], '\''
		mov  [ebp-0x1e], 'm'
		mov  [ebp-0x1d], ' '
		mov  [ebp-0x1c], 'h'
		mov  [ebp-0x1b], 'a'
		mov  [ebp-0x1a], 'c'
		mov  [ebp-0x19], 'k'
		mov  [ebp-0x18], 'e'
		mov  [ebp-0x17], 'd'
		mov  [ebp-0x16], 0x0
		lea ebx, [ebp-0x20]

		push 0
		push ebx
		push ebx
		push 0
//call MessageBoxA
		call eax

		add esp, 0x40
	}
	return 0;

}
