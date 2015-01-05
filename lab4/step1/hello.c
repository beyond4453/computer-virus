#include <windows.h>
int WINAPI WinMain(int a, int b, int c, int d)
{   
    while (MessageBoxA(0, "Hello", "Msg", MB_YESNO) == IDYES);
    return 0;
}