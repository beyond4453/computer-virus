#include <windows.h>

DWORD dwOldAddr = 0;
char *b1 = "I'm hacked" ;
char *c1 = "Merry xMas";
//char *d1 = "MB_YESNO";
char *a1 = "NULL";

DWORD __stdcall MyMessageBox(DWORD a, DWORD b, DWORD c, DWORD d)
{
     //请在这里补全代码

	__asm{
		push MB_YESNO
		push c1
		push b1
		push 0
		mov eax, dwOldAddr
		call eax
	}
    return 0;
}

int IATHook(PCHAR szFuncName, DWORD MyMessageBox)
{
    DWORD dwBaseAddress;
    PIMAGE_DOS_HEADER pDosHeader;
    PIMAGE_NT_HEADERS pNtHeaders;
    PIMAGE_FILE_HEADER pFileHeader;
    PIMAGE_OPTIONAL_HEADER32 pOptHeader;

    DWORD dwRVAImpTbl;
    DWORD dwSizeOfImpTbl;
    PIMAGE_IMPORT_DESCRIPTOR pImpTbl, pImpDesc;

    PIMAGE_THUNK_DATA pthunk, pthunk2;
    PIMAGE_IMPORT_BY_NAME pOrdinalName;

    PDWORD pdwAddr = NULL;
    DWORD  old;

    dwBaseAddress = (DWORD)GetModuleHandle(NULL);
    pDosHeader = (PIMAGE_DOS_HEADER)dwBaseAddress;
    pNtHeaders = (PIMAGE_NT_HEADERS)(dwBaseAddress + pDosHeader->e_lfanew);
    pOptHeader = &(pNtHeaders->OptionalHeader);
    
    dwRVAImpTbl = pOptHeader->DataDirectory[1].VirtualAddress;
    dwSizeOfImpTbl = pOptHeader->DataDirectory[1].Size;
    pImpTbl = (PIMAGE_IMPORT_DESCRIPTOR)(dwBaseAddress + dwRVAImpTbl);

    for (pImpDesc = pImpTbl; 
        (DWORD)pImpDesc < ((DWORD)pImpTbl + dwSizeOfImpTbl); pImpDesc++)
    {
        if (pImpDesc->Name == 0)     break;
        pthunk = (PIMAGE_THUNK_DATA) 
            (dwBaseAddress + pImpDesc->OriginalFirstThunk);
        pthunk2 = (PIMAGE_THUNK_DATA) 
            (dwBaseAddress + pImpDesc->FirstThunk);
        for (;(pthunk->u1.Function != 0); pthunk++, pthunk2++) {
            if (pthunk->u1.Ordinal & 0x80000000) continue;
            pOrdinalName = (PIMAGE_IMPORT_BY_NAME)
                (dwBaseAddress + pthunk->u1.AddressOfData);
            if (CompareStringA(0, NORM_IGNORECASE, 
                    "MessageBoxA", -1, (LPSTR)&pOrdinalName->Name, -1) 
                == CSTR_EQUAL) {
                pdwAddr = (PDWORD)pthunk2;
                break;
            }
        }
        if (pdwAddr) break;
    }

    if (!pdwAddr) {
        printf("Error: MessageBoxA not found.\n");
        return -1;
    }
    printf("MessageBoxA: 0x%08X -> 0x%08X\n", pdwAddr, *pdwAddr);
	dwOldAddr = *pdwAddr;

    if (VirtualProtect(pdwAddr, 4, PAGE_READWRITE, &old)==0)
    {
        printf("Error: VirtualProtect Failed.\n");
        return -1;
    };

    *pdwAddr = (DWORD)MyMessageBox;
    printf("MessageBoxA: 0x%08X -> 0x%08X\n", pdwAddr, *pdwAddr);

    return 1;
}

void main()
{
    //请在这里补全代码   
    //定位MessageBoxA的导入地址表表项
    //修改导入地址表表项
	MessageBoxA(NULL, "Happyday", "hello", MB_YESNO);
	IATHook("MessageBoxA", (DWORD)MyMessageBox);
    MessageBoxA(NULL, "Happyday", "hello", MB_OK);

} 