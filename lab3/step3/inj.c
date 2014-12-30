//inj.c
#include <windows.h>
PBYTE pRemoteCode, pCode, pOrignCode;
DWORD dwSizeOfCode;

//下面是即将被注入的代码
void code_start();              // code_start是二进制码的开始标记
void _str_msgboxa();            // _str_msgboxa标记存放字符串的地址
void _str_mymsgboxa();
void _addr_GetModuleHandleA();	//_addr_GetModuleHandleA 标记存放API入口地址的地址
void MyMessageBoxA();
BOOL CompStr(LPSTR s1, LPSTR s2);
DWORD GetIAFromImportTable(DWORD dwBase, LPCSTR lpszFuncName); // 寻找IA的函数

//... ... ...
//这里请填入需要一并被注入的函数代码
void code_end();                // code_end是二进制码的开始标记
// 上面是即将被注入的代码


__declspec(naked) void code_start()
{
    __asm {
        push ebp
        mov  ebp, esp
        push ebx
//Local variables
        sub  esp, 0x10
        // ebp - 0x0C ===> ImageBase ???
// self-locating 自定位 请阅读并理解下面3条指令的含义
        call _delta
_delta:
        pop  ebx
        sub  ebx, offset _delta
// 调用GetModuleHandleA()
        push 0
        lea  ecx, [ebx + _addr_GetModuleHandleA]
        call dword ptr [ecx]
        cmp  eax, 0x0
        jne  _cont1
        mov  eax, 0x1
        jmp  _ret
_cont1:
        mov  [ebp-0x0C], eax
// 调用GetIAFromImportTable();
        lea  ecx, [ebx + _str_msgboxa]
// push "MeassageBoxA"
        push ecx
// push ImageBase
        push [ebp-0x0C]
//调用GetIAFromImportTable
        call offset GetIAFromImportTable
//???????????
        add  esp, 0x8
        cmp  eax, 0x0
        jne  _ret
        mov  eax, 0x2
_ret:
        add  esp, 0x20
        pop  ebx
        mov  esp, ebp
        pop  ebp
        ret
    }
}


// _str_msgboxa是字符串”MessageBoxA”的地址
__declspec(naked) void _str_msgboxa()
{
    __asm {
        _emit 'M'
        _emit 'e'
        _emit 's'
        _emit 's'
        _emit 'a'
        _emit 'g'
        _emit 'e'
        _emit 'B'
        _emit 'o'
        _emit 'x'
        _emit 'A'
        _emit 0x0
    }
}


// _str_msgboxa是字符串”MyMessageBoxA”的地址
__declspec(naked) void _str_mymsgboxa()
{
    __asm {
		_emit 'M'
		_emit 'y'
        _emit 'M'
        _emit 'e'
        _emit 's'
        _emit 's'
        _emit 'a'
        _emit 'g'
        _emit 'e'
        _emit 'B'
        _emit 'o'
        _emit 'x'
        _emit 'A'
        _emit 0x0
    }
}

// _addr_GetModuleHandleA是存放GetModuleHandleA()的全局变量
__declspec(naked) void _addr_GetModuleHandleA()
{
    __asm {
        _emit 0xAA
        _emit 0xBB
        _emit 0xAA
        _emit 0xEE
    }
}

//MyMessageBoxA()
__declspec(naked) void MyMessageBoxA()
{
    __asm {
        push ebp
        mov  ebp, esp
        push ebx
        
        call _delta2
_delta2:
        pop ebx
        sub ebx, offset _delta2
        
        push [ebp + 0x14]
        push [ebp + 0x10]
        lea  ecx, [ebx + _str_hacked]
        push ecx
        push [ebp + 0x08]
        lea  eax, [ebx + _addr_MessageBoxA]   //need relocation
        call dword ptr [eax]
        pop  ebx
        mov  esp, ebp
        pop  ebp
        ret  16
_str_hacked:
        _emit 'I'
        _emit '\''
        _emit 'm'
        _emit ' '
        _emit 'h'
        _emit 'a'
        _emit 'c'
        _emit 'k'
        _emit 'e'
        _emit 'd'
        _emit '!'
        _emit 0x0
_addr_MessageBoxA:
        _emit 0x11
        _emit 0xEA
        _emit 0x1F
        _emit 0x76
   }
}

// 这里请填入GetIAFromImportTable()函数的相关代码

BOOL CompStr(LPSTR s1, LPSTR s2)
{
    PCHAR p, q;
    for (p = s1, q = s2; (*p != 0) && (*q != 0); p++, q++) {
        if (*p != *q) return FALSE;
    }
    return TRUE;
}

DWORD GetIAFromImportTable(DWORD dwBase, LPCSTR lpszFuncName)
{
	PIMAGE_DOS_HEADER pDosHeader;
    PIMAGE_NT_HEADERS pNtHeaders;
    PIMAGE_FILE_HEADER pFileHeader;
    PIMAGE_OPTIONAL_HEADER32 pOptHeader;
	
	DWORD dwRVAImpTbl;
	DWORD dwSizeOfImpTbl;

	PIMAGE_IMPORT_DESCRIPTOR pImpTbl, pImpDesc;
	PIMAGE_THUNK_DATA pthunk, pthunk2;
    PIMAGE_IMPORT_BY_NAME pOrdinalName;

	pDosHeader = (PIMAGE_DOS_HEADER)dwBase;
    pNtHeaders = (PIMAGE_NT_HEADERS)(dwBase + pDosHeader->e_lfanew);
    pOptHeader = &(pNtHeaders->OptionalHeader);
	dwRVAImpTbl = pOptHeader->DataDirectory[1].VirtualAddress;
    dwSizeOfImpTbl = pOptHeader->DataDirectory[1].Size;
  	pImpTbl = (PIMAGE_IMPORT_DESCRIPTOR)(dwBase + dwRVAImpTbl);
	pImpDesc = (PIMAGE_IMPORT_DESCRIPTOR)pImpTbl;

    if (pImpDesc->Name == 0) return 0;
    pthunk = (PIMAGE_THUNK_DATA) (dwBase + pImpDesc->OriginalFirstThunk);
    pthunk2 = (PIMAGE_THUNK_DATA) (dwBase + pImpDesc->FirstThunk);
    for (; pthunk->u1.Function != 0; pthunk++, pthunk2++) {
        if (pthunk->u1.Ordinal & 0x80000000) continue;
        pOrdinalName = (PIMAGE_IMPORT_BY_NAME) (dwBase + pthunk->u1.AddressOfData);
        if (CompStr((LPSTR)lpszFuncName, (LPSTR)&pOrdinalName->Name)) 
            return (DWORD)pthunk2->u1.Function;
    }
    return 0;

}


// code_end是二进制码的结束标记
__declspec(naked) void code_end()
{
    __asm _emit 0xCC
}


// make_code()函数是将开始标记code_start和结束标记code_end之间的所有二进制数据拷贝到一个缓冲区中
DWORD make_code()
{
    int off; 
    DWORD func_addr;
    HMODULE hModule;
    __asm {
        mov edx, offset code_start
        mov dword ptr [pOrignCode], edx
        mov eax, offset code_end
        sub eax, edx
        mov dword ptr [dwSizeOfCode], eax
    }
    pCode = VirtualAlloc(NULL, dwSizeOfCode, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (pCode== NULL) {
        printf("[E]: VirtualAlloc failed\n");
        return 0;
    }
    printf("[I]: VirtualAlloc ok --> at 0x%08x.\n", pCode);
    for (off = 0; off<dwSizeOfCode; off++) {
        *(pCode+off) = *(pOrignCode+off);
    }
    printf("[I]: Copy code ok --> from 0x%08x to 0x%08x with size of 0x%08x.\n", 
        pOrignCode, pCode, dwSizeOfCode);
    hModule = LoadLibrary("kernel32.dll");
    if (hModule == NULL) {
        printf("[E]: kernel32.dll cannot be loaded. \n");
        return 0;
    }
    func_addr = (DWORD)GetProcAddress(hModule, "GetModuleHandleA");
    if (func_addr == 0) {
        printf("[E]: GetModuleHandleA not found. \n");
        return 0;
    }
    off = (DWORD)pCode - (DWORD)pOrignCode;
    *(PDWORD)((PBYTE)_addr_GetModuleHandleA + off) = func_addr;
    return dwSizeOfCode;
}


// inject_code()函数是存放在pCode所指向的缓冲区中的二进制代码注入到远程进程中
int inject_code(DWORD pid)
{
    //请填入代码，完成注入过程
	int     PID         = 0;
    HANDLE  hProcess    = 0; 
    PBYTE   pCodeRemote = NULL;
    DWORD   dwNumBytesXferred = 0;
    
    HANDLE  hThread	   = 0;
    DWORD   dwThreadId = 0;
    int	    exitcode   = 0;

    PID = (int)pid;
    if (PID <= 0) {
        printf("[E]: pid should be greater than zero!\n"); 
        return -1;
    }

    printf("[I]: Opening remote process %d......", PID); 
    hProcess = OpenProcess(PROCESS_CREATE_THREAD 
        | PROCESS_QUERY_INFORMATION
        | PROCESS_VM_OPERATION 
        | PROCESS_VM_WRITE 
        | PROCESS_VM_READ,
        FALSE, PID);
        
    if (hProcess == NULL) {
        printf("failed.\n"); 
        return -1;
    }   
    printf("ok.\n");

    printf("[I]: Allocating remote memory with size of 0x%08x ......", 
        dwSizeOfCode);

    pCodeRemote = (PBYTE) VirtualAllocEx(hProcess, 
            0, 
            dwSizeOfCode, 
            MEM_COMMIT, 
            PAGE_EXECUTE_READWRITE);		
    if (pCodeRemote == NULL) {
        printf("failed.\n");
        CloseHandle(hProcess);
        return -1;
    }
    printf("ok at 0x%08x.\n", pCodeRemote);

    printf("[I]: Writing code ......");
    if (WriteProcessMemory(hProcess, 
            pCodeRemote, 
            pCode, 
            dwSizeOfCode, 
            &dwNumBytesXferred) == 0) {
        printf("failed.\n");
        VirtualFreeEx(hProcess, pCodeRemote,
                dwSizeOfCode, MEM_RELEASE);
        CloseHandle(hProcess);
        return -1;
    };
    printf("ok (%d bytes were written).\n", dwNumBytesXferred);
        
    printf("[I]: Creating a remote thread ......");
    hThread = CreateRemoteThread(hProcess, NULL, 0, 
            (LPTHREAD_START_ROUTINE) pCodeRemote,
            pCodeRemote, 0 , &dwThreadId);
    if (hThread == 0) {
        printf("failed.\n");
        if ( pCodeRemote != 0 )	
            VirtualFreeEx(hProcess, pCodeRemote, 0, MEM_RELEASE);
        if ( hThread != 0 )			
            CloseHandle(hThread);
        return -1;
    }
    printf("ok.\n");
 
    printf("[I]: Waiting the remote thread ......");
    WaitForSingleObject(hThread, INFINITE);
    GetExitCodeThread(hThread, (PDWORD) &exitcode);
    printf("exited with 0x%08X\n", exitcode);

//	printf("MessageBoxA: 0x%08X-->0x%08X", exitcode, *(int *)exitcode);
    VirtualFreeEx(hProcess, pCodeRemote, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return exitcode;
}


int main(int argc, char *argv[])
{
    DWORD pid = 0;
	int r;
    // 为pid赋值为hello.exe的进程ID
	if (argc < 2) {
        printf("Usage: %s pid\n", argv[0]);
        return -1;
    }
    pid = atoi(argv[1]);

    make_code();
    r = inject_code(pid);

	printf("MessageBoxA: 0x%08X", r);
    return 0;
}