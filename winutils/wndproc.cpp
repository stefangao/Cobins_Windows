#include <Tlhelp32.h>
#include "wtermin.h"

__declspec(dllexport) DWORD WBS_GetProcessId(LPCSTR szExeFile) 
{
    PROCESSENTRY32 ProcessInfo;        
    BOOL bResult;
    
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == NULL)
        return 0;
    
    ProcessInfo.dwSize = sizeof(ProcessInfo); 
    bResult = Process32First(hSnapShot,&ProcessInfo);
    while (bResult)
    {     
        if (!stricmp(ProcessInfo.szExeFile, szExeFile))
        {
            return ProcessInfo.th32ProcessID;
        }
        
        bResult = Process32Next(hSnapShot, &ProcessInfo);
    }

    return 0;
}

__declspec(dllexport) DWORD WBS_GetProcessId2(LPCSTR szExeFilePart1, LPCSTR szExeFilePart2) 
{
    PROCESSENTRY32 ProcessInfo;        
    BOOL bResult;
    
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == NULL)
        return 0;
    
    ProcessInfo.dwSize = sizeof(ProcessInfo); 
    bResult = Process32First(hSnapShot,&ProcessInfo);
    while (bResult)
    {   
        int nPart1Len = strlen(szExeFilePart1);
        int nPart2Len = strlen(szExeFilePart2);
        if (!strnicmp(ProcessInfo.szExeFile, szExeFilePart1, nPart1Len) &&
            !strnicmp(ProcessInfo.szExeFile + nPart1Len, szExeFilePart2, nPart2Len))
        {
            return ProcessInfo.th32ProcessID;
        }
        
        bResult = Process32Next(hSnapShot, &ProcessInfo);
    }

    return 0;
}

typedef struct
{
    HWND   hWnd;
    LPCSTR lpWndTitle;

} ThreadWndInfo_t;

static BOOL CALLBACK EnumThreadWndProc(HWND hWnd, LPARAM lParam)
{
    ThreadWndInfo_t *ThreadWndInfo;
    char buf[256] = {0};

    ThreadWndInfo = (ThreadWndInfo_t*)lParam;
    if (ThreadWndInfo)
        ThreadWndInfo->hWnd = NULL;
    
    if (GetWindowText(hWnd, buf, 255) <= 0)
        return TRUE;

    if (!strstr(buf, (LPCSTR)ThreadWndInfo->lpWndTitle))
        return TRUE;

    ThreadWndInfo->hWnd = hWnd;
    
    return FALSE;
}

__declspec(dllexport) DWORD WBS_GetThreadId(DWORD OwnerProcessId, LPCSTR lpWndTitle, HWND &hMainWnd) 
{
    THREADENTRY32 ThreadInfo;
    ThreadWndInfo_t ThreadWndInfo;
    BOOL bResult;
    
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, OwnerProcessId);
    if (hSnapShot == NULL)
        return 0;
    
    ThreadInfo.dwSize = sizeof(ThreadInfo); 
    bResult = Thread32First(hSnapShot, &ThreadInfo);
    while (bResult)
    {     
        if (ThreadInfo.th32OwnerProcessID == OwnerProcessId)
        {
            //WT_Trace("threadid=%x\t", ThreadInfo.th32ThreadID);
            ThreadWndInfo.lpWndTitle = lpWndTitle;
            if (EnumThreadWindows(ThreadInfo.th32ThreadID, EnumThreadWndProc, (LPARAM)&ThreadWndInfo))
            {
                if (ThreadWndInfo.hWnd != NULL)
                    return ThreadInfo.th32ThreadID;            
            }
            else
            {
                if (ThreadWndInfo.hWnd != NULL)
                {
                    hMainWnd = ThreadWndInfo.hWnd;
                    return ThreadInfo.th32ThreadID;
                }
            }
        }
        bResult = Thread32Next(hSnapShot, &ThreadInfo);
    }
    
    return 0;
}

__declspec(dllexport) int WBS_GetProcessWindows(LPCSTR szExeFile, LPCSTR lpWndTitle,  DWORD wndArray[], int nArraySize)
{
    PROCESSENTRY32 ProcessInfo;        
    BOOL bResult;
	int  nWndNum = 0;
    
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == NULL)
        return -1;
    
    ProcessInfo.dwSize = sizeof(ProcessInfo); 
    bResult = Process32First(hSnapShot,&ProcessInfo);
    while (bResult && nWndNum < nArraySize)
    {     
        if (!stricmp(ProcessInfo.szExeFile, szExeFile))
        {
            //return ProcessInfo.th32ProcessID;
            HWND hMainWnd = NULL;
            if (WBS_GetThreadId(ProcessInfo.th32ProcessID, lpWndTitle, hMainWnd) != NULL && hMainWnd != NULL)
            {
                wndArray[nWndNum++] = (DWORD)hMainWnd;
            }
        }
        
        bResult = Process32Next(hSnapShot, &ProcessInfo);
    }

    return nWndNum;
}