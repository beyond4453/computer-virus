#include<windows.h>

#pragma comment(lib, "user32.lib")

int main()
{
	MessageBox(0,"","",0);
	__asm{
		sub  esp, 0x20
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
		
		push 0
		lea  eax, [ebp-0x20]
		push eax
		push eax
		push 0
		mov  eax, dword ptr [MessageBoxA]
//		mov eax, 0x76BCEA11
        call eax
		add esp, 0x20
	}
	return 0;

}
