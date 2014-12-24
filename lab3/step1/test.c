#include <windows.h>

/*
DWORD GetIAFromImportTable(DWORD dwBase, LPCSTR lpszFuncName)
{

}
*/



  
DWORD GetIAFromImpDesc(DWORD dwBase, LPCSTR lpszName, PIMAGE_IMPORT_DESCRIPTOR pImpDesc) 
{
    PIMAGE_THUNK_DATA pthunk, pthunk2;
    PIMAGE_IMPORT_BY_NAME pOrdinalName;
    if (pImpDesc->Name == 0) return 0;
    pthunk = (PIMAGE_THUNK_DATA) (dwBase + pImpDesc->OriginalFirstThunk);
    pthunk2 = (PIMAGE_THUNK_DATA) (dwBase + pImpDesc->FirstThunk);
    for (; pthunk->u1.Function != 0; pthunk++, pthunk2++) {
        if (pthunk->u1.Ordinal & 0x80000000) continue;
        pOrdinalName = (PIMAGE_IMPORT_BY_NAME) (dwBase + pthunk->u1.AddressOfData);
        if (CompStr((LPSTR)lpszName, (LPSTR)&pOrdinalName->Name)) 
            return (DWORD)pthunk2;
    }
    return 0;
}




BOOL CompStr(LPSTR s1, LPSTR s2)
{
    PCHAR p, q;
    for (p = s1, q = s2; (*p != 0) && (*q != 0); p++, q++) {
        if (*p != *q) return FALSE;
    }
    return TRUE;
}


void main()
{
    //请在这里补全代码，包括调用GetIAFromImportTable() 
	DWORD r;
	DWORD dwBase;

    PIMAGE_DOS_HEADER pDosHeader;
    PIMAGE_NT_HEADERS pNtHeaders;
    PIMAGE_FILE_HEADER pFileHeader;
    PIMAGE_OPTIONAL_HEADER32 pOptHeader;

	DWORD dwRVAImpTbl;
	DWORD dwSizeOfImpTbl;

    PIMAGE_IMPORT_DESCRIPTOR pImpTbl, pImpDesc;
    PIMAGE_THUNK_DATA pthunk, pthunk2;
	PIMAGE_IMPORT_BY_NAME pOrdinalName;

    dwBase = (DWORD)GetModuleHandleA(NULL);

    pDosHeader = (PIMAGE_DOS_HEADER)dwBase;

    pNtHeaders = (PIMAGE_NT_HEADERS)(dwBase + pDosHeader->e_lfanew);

    pOptHeader = &(pNtHeaders->OptionalHeader);

	dwRVAImpTbl = pOptHeader->DataDirectory[1].VirtualAddress;

    dwSizeOfImpTbl = pOptHeader->DataDirectory[1].Size;

  	pImpTbl = (PIMAGE_IMPORT_DESCRIPTOR)(dwBase + dwRVAImpTbl);
    
	pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)pImpTbl;

	r = GetIAFromImpDesc(dwBase, "MessageBoxA", pImpDesc);

	printf("0x%08X ==> 0x%08X\n", r, *(PDWORD)r);
	MessageBoxA(NULL, "hello", "msg", MB_OK);
} 