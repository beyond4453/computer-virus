#include <windows.h>

#pragma comment(lib, "user32.lib")
char *s = "I'm hacked";

//	addr : 0x76BCEA11

char code[] = { 0x6A, 0x00,
				0xFF, 0x35, 0x30, 0x60, 0x40, 0x00,
				0xFF, 0x35, 0x30, 0x60, 0x40, 0x00,
				0x6A, 0x00,
				0xB8, 0x11, 0xEA, 0xBC, 0x76,
//				0xA1, 0x9C, 0x50, 0x40, 0x00,
				0xFF, 0xD0,
				0xC3};
void main()
{
	
    int op;
	MessageBoxA(0,s,s,0);
    VirtualProtect(&code[0], sizeof(code), PAGE_EXECUTE_READWRITE, &op);
    __asm {
       call offset code
    }
} 