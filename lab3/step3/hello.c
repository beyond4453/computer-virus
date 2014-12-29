#include <windows.h>
int main()
{
    while (MessageBoxA(0,"hello", "hello.exe", MB_YESNO) 
        == IDYES) {}
    return 0;
}
