#include<windows.h>

#pragma comment(lib, "user32.lib")

int main()
{
	MessageBox(0,"","",0);
	__asm{
		sub  esp, 0x20
		mov  [ebp-0x20], 'u'
		mov  [ebp-0x1f], 's''
		mov  [ebp-0x1e], 'e'
		mov  [ebp-0x1d], 'r'
		mov  [ebp-0x1c], '3'
		mov  [ebp-0x1b], '2'
		mov  [ebp-0x1a], '.'
		mov  [ebp-0x19], 'd'
		mov  [ebp-0x18], 'l'
		mov  [ebp-0x17], 'l'
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
