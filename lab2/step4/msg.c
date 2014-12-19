#include <windows.h>


#pragma comment(lib, "user32.lib")
    
__declspec(dllexport) int ShowMsg(char* s)
{
   if (MessageBoxA(0, s, "Message", MB_YESNO) == IDYES)
 return 1;
   else 
        return 0;
}