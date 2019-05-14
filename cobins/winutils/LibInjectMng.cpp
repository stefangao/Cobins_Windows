#include <windows.h>
#include "processmng.h"
#include "base/cobUtils.h"
#include "LibinjectMng.h"

BOOL LIM_EnableDebugPrivilege(HANDLE hProcess)
{
	TOKEN_PRIVILEGES tkp; 
	HANDLE hToken; 
	
	if (!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken)) 
	{ 
		COBLOG("OpenProcessToken failed!");
		return FALSE;
	}
	
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1; 
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	BOOL bResult = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS) 
	{ 
		WT_Trace("AdjustTokenPrivileges enable failed: err=%x", GetLastError());
		return FALSE;
	} 

	return TRUE;
}

HINSTANCE LIM_InjectLib(DWORD process_id, LPCSTR lib_path, DWORD &dwErrorCode)
{
	PTHREAD_START_ROUTINE pfnRemote =(PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA");
	if(pfnRemote ==NULL)
		return NULL;

	HANDLE hProcess =OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if(hProcess ==NULL)
	{
		return NULL;
	}

    LIM_EnableDebugPrivilege(GetCurrentProcess());

	int mem_size =strlen(lib_path)+1;
	void *premote_mem =VirtualAllocEx(hProcess, NULL, mem_size, MEM_COMMIT, PAGE_READWRITE);
	if(premote_mem ==NULL)
	{
		CloseHandle(hProcess);
		return NULL;
	}

	int ret =WriteProcessMemory(hProcess, premote_mem, (void*)lib_path, mem_size,NULL);
	if(ret == STATUS_ACCESS_VIOLATION || ret == false)
	{
        WT_Trace("LIM_InjectLib: WriteProcessMemory failed. lasterr=%x\n", GetLastError());
		VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
		CloseHandle(hProcess);
        dwErrorCode = 0x100;
		return NULL;
	}

	HANDLE hThread =CreateRemoteThread(hProcess, NULL, 0, pfnRemote, premote_mem, 0, NULL);
	if(hThread == NULL)
	{
        WT_Trace("LIM_InjectLib: CreateRemoteThread failed. lasterr=%x\n", GetLastError());
		VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
		CloseHandle(hProcess);
        dwErrorCode = 0x101;
		return NULL;
	}
	
    DWORD hInstance;
	WaitForSingleObject(hThread, INFINITE);
    GetExitCodeThread(hThread, &hInstance);

	VirtualFreeEx(hProcess, premote_mem, 0, MEM_RELEASE);
	CloseHandle(hProcess);
	CloseHandle(hThread);

	return (HINSTANCE)hInstance;
}

BOOL LIM_IsDllExist(LPCSTR lpDllName, HWND hDestWnd)
{
	CProcessModule pm;
    DWORD dwProcessId = NULL;

    if (hDestWnd == NULL || !IsWindow(hDestWnd))
        return FALSE;
    
    GetWindowThreadProcessId(hDestWnd, &dwProcessId);
	HANDLE hModule =pm.GetProcessModuleHandle(dwProcessId, (char*)lpDllName);
	if(hModule == NULL)
        return FALSE;

    return TRUE;
}

BOOL LIM_EjectLib(DWORD process_id, LPCSTR lib_name)
{
    DWORD bResult = FALSE;
	PTHREAD_START_ROUTINE pfnRemote =(PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle("Kernel32"), "FreeLibrary");
	
	if(pfnRemote ==NULL)
	{
		return FALSE;
	}

	HANDLE hProcess =OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	if(hProcess ==NULL)
	{
		return FALSE;
	}

	CProcessModule pm;
	HANDLE hmod =pm.GetProcessModuleHandle(process_id, (char*)lib_name);
	if(hmod ==NULL)
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	HANDLE hThread =CreateRemoteThread(hProcess, NULL, 0, 
			pfnRemote, hmod, 0, NULL);

	if(hThread ==NULL)
	{
		CloseHandle(hProcess);
		return FALSE;
	}
	WaitForSingleObject(hThread, INFINITE);
    GetExitCodeThread(hThread, &bResult);

	CloseHandle(hProcess);
	CloseHandle(hThread);

	return bResult;
}

HINSTANCE LIM_WinInjectLib(LPCSTR lpLibPath, HWND hTargetWnd, DWORD &dwErrorCode)
{
    DWORD dwProcessId = NULL;
    
    if (hTargetWnd == NULL || lpLibPath == NULL)
        return NULL;
    
    GetWindowThreadProcessId(hTargetWnd, &dwProcessId);
    if (dwProcessId == NULL)
        return NULL;

    return LIM_InjectLib(dwProcessId, lpLibPath, dwErrorCode);
}

BOOL LIM_WinEjectLib(LPCSTR lpLibName, HWND hTargetWnd)
{
    DWORD dwProcessId = NULL;

    if (hTargetWnd == NULL || lpLibName == NULL)
        return FALSE;

    GetWindowThreadProcessId(hTargetWnd, &dwProcessId);
    if (dwProcessId == NULL)
        return FALSE;
  
    return LIM_EjectLib(dwProcessId, lpLibName);
}

BOOL LIM_WinEjectLibEx(HWND hDestWnd, HMODULE hModule)
{
    DWORD dwProcessId = NULL;
    PTHREAD_START_ROUTINE pfnRemote = NULL;
    DWORD bResult = FALSE;
    
    if (hDestWnd == NULL || hModule == NULL)
        return FALSE;

    GetWindowThreadProcessId(hDestWnd, &dwProcessId);
    if (dwProcessId == NULL)
        return FALSE;
    
    pfnRemote = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("Kernel32"), "FreeLibrary");
	if(pfnRemote ==NULL)
		return FALSE;

    HANDLE hProcess =OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if(hProcess ==NULL)
		return FALSE;

	HANDLE hThread =CreateRemoteThread(hProcess, NULL, 0, 
			pfnRemote, hModule, 0, NULL);

	if(hThread == NULL)
	{
		CloseHandle(hProcess);
		return FALSE;
	}

	WaitForSingleObject(hThread, INFINITE);
    GetExitCodeThread(hThread, &bResult);

	CloseHandle(hProcess);
	CloseHandle(hThread);

    return bResult;
}

BOOL LIM_HookAPI(HWND hHookWnd, LPCSTR lpLibPath)
{
    DWORD dwProcessId = NULL;
    
    if (hHookWnd == NULL || lpLibPath == NULL)
        return FALSE;
    
    GetWindowThreadProcessId(hHookWnd, &dwProcessId);
    if (dwProcessId == NULL)
        return FALSE;

    DWORD dwErrCode;
    if (LIM_InjectLib(dwProcessId, lpLibPath, dwErrCode) < 0)
        return FALSE;

    return TRUE;
}

BOOL LIM_UnhookAPI(HWND hHookWnd, LPCSTR lpLibName)
{
    DWORD dwProcessId = NULL;

    if (hHookWnd == NULL || lpLibName == NULL)
        return FALSE;

    GetWindowThreadProcessId(hHookWnd, &dwProcessId);
    if (dwProcessId == NULL)
        return FALSE;
  
    return LIM_EjectLib(dwProcessId, lpLibName);
}

bool RemoteFreeLibrary(DWORD dwProcessID,LPCSTR lpszDll)
{
    //打开目标进程
    HANDLE hProcess;
	hProcess = OpenProcess( PROCESS_ALL_ACCESS,FALSE,dwProcessID );
    //向目标进程地址空间写入DLL名称
    DWORD dwSize, dwWritten;
    dwSize = lstrlenA( lpszDll ) + 1;
    LPVOID lpBuf = VirtualAllocEx( hProcess,NULL,dwSize,MEM_COMMIT,PAGE_READWRITE );
    if( NULL==lpBuf )
    {
        CloseHandle( hProcess );
        return false;
    }
    if( WriteProcessMemory(hProcess,lpBuf,(LPVOID)lpszDll,dwSize,&dwWritten) )
    {
        //要写入字节数与实际写入字节数不相等，仍属失败
        if( dwWritten!=dwSize )
        {
            VirtualFreeEx( hProcess,lpBuf,dwSize,MEM_DECOMMIT );
            CloseHandle( hProcess );
            return false;
        }
    }
    else
    {
        CloseHandle( hProcess );
        return false;
    }
    //使目标进程调用GetModuleHandle，获得DLL在目标进程中的句柄
    DWORD dwHandle, dwID;
    LPVOID pFunc = GetModuleHandleA;
    HANDLE hThread;
	hThread = CreateRemoteThread( hProcess,NULL,0,(LPTHREAD_START_ROUTINE)pFunc, 
		lpBuf,0,&dwID );
    //等待GetModuleHandle运行完毕
    WaitForSingleObject( hThread,INFINITE );
    //获得GetModuleHandle的返回值
    GetExitCodeThread( hThread,&dwHandle );
    //释放目标进程中申请的空间
    VirtualFreeEx( hProcess,lpBuf,dwSize,MEM_DECOMMIT );
    CloseHandle( hThread );
    //使目标进程调用FreeLibrary，卸载DLL
	pFunc = FreeLibrary;
    hThread = CreateRemoteThread( hProcess,NULL,0,(LPTHREAD_START_ROUTINE)pFunc, 
		(LPVOID)dwHandle,0,&dwID );
    //等待FreeLibrary卸载完毕
    WaitForSingleObject( hThread,INFINITE );
    CloseHandle( hThread );
    CloseHandle( hProcess );
    return true;
}