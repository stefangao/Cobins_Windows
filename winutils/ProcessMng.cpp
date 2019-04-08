#include <windows.h>
#include "ProcessMng.h"
#include "wtermin.h"

CProcessModule::CProcessModule()
{
    HMODULE hModule;

	m_dwProcessCount =0;
	m_nModCount =0;
	memset(&m_mod_name, 0, sizeof(m_mod_name));

	hModule = GetModuleHandle("kernel32.dll");
	if (hModule == NULL)
		return;

	pCreateToolhelp32Snapshot = (CREATETOOLHELP32SNAPSHOT_PROC)GetProcAddress(hModule, "CreateToolhelp32Snapshot");

	if (pCreateToolhelp32Snapshot == NULL)
		return;

	pProcess32First = (PROCESS32FIRST_PROC)GetProcAddress(hModule, "Process32First");

	if (pProcess32First == NULL)
		return;

	pProcess32Next = (PROCESS32NEXT_PROC)GetProcAddress(hModule, "Process32Next");

	if (pProcess32Next == NULL)
		return;

	pModule32First = (MODULE32FIRST_PROC)GetProcAddress(hModule, "Module32First");

	if (pModule32First == NULL)
		return;

	pModule32Next = (MODULE32NEXT_PROC)GetProcAddress(hModule, "Module32Next");

	if (pModule32Next == NULL)
		return;
}

CProcessModule::~CProcessModule()
{

}

int CProcessModule::EnumProcess()
{
	HANDLE hSnapShot;

	m_dwProcessCount =0;
	PROCESSENTRY32 ProcessEntry32;
	BOOL Result;

	hSnapShot = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapShot == (HANDLE)-1)
		return FALSE;

	ProcessEntry32.dwSize = sizeof(PROCESSENTRY32);

	Result = pProcess32First(hSnapShot, &ProcessEntry32);

	if (Result != TRUE)
	{
		CloseHandle(hSnapShot);
		return FALSE;
	}

	do
	{
		m_dwProcessIDs[m_dwProcessCount] = ProcessEntry32.th32ProcessID;
		m_dwProcessCount ++;
	} while (pProcess32Next(hSnapShot, &ProcessEntry32) && m_dwProcessCount < MAX_PROCESS_COUNT);

	CloseHandle(hSnapShot);

//#endif

	return m_dwProcessCount;
}

int CProcessModule::EnumProcessModules(DWORD process_id)
{
	HANDLE hSnapShot;
	MODULEENTRY32 ModuleEntry32;
	BOOL Result;

	m_nModCount =0;
 
	hSnapShot = pCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, process_id);
	if (hSnapShot == (HANDLE)-1)
		return -1;

	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

	Result = pModule32First(hSnapShot, &ModuleEntry32);

	if (Result != TRUE)
	{
		CloseHandle(hSnapShot);
		return -1;
	}

	do
	{
		strcpy(m_mod_name[m_nModCount], ModuleEntry32.szModule);
		m_hMods[m_nModCount] =ModuleEntry32.hModule;
		m_nModCount++;

	} while (pModule32Next(hSnapShot, &ModuleEntry32) && m_nModCount <MAX_MODULE_COUNT);

	CloseHandle(hSnapShot);

	return m_nModCount;
}

int CProcessModule::GetModuleBaseNameA(int num, char *mod_name)
{
	strcpy(mod_name, m_mod_name[num]);
	return 0;
}

void CProcessModule::EndEnumProcessModules()
{

}

HANDLE CProcessModule::GetProcessModuleHandle(DWORD dwProcessId, LPCSTR lpModName)
{
	int mod_count = EnumProcessModules(dwProcessId);
	char mod_base_name[100];

	for(int i =0; i<mod_count; i++)
	{
		GetModuleBaseNameA(i, mod_base_name);
		if(strcmpi(mod_base_name, lpModName) ==0)
		{
			EndEnumProcessModules();
			return m_hMods[i];
		}
	}

	EndEnumProcessModules();

	return NULL;
}

HMODULE CProcessModule::GetProcessModuleHandleEx(DWORD dwProcessId, LPCSTR lpModName, LPTSTR lpModPathBuf, int nModPathBufLen)
{
	MODULEENTRY32 ModuleEntry32;
	HANDLE hSnapShot;
    HMODULE hModule = NULL;
	BOOL bResult;

	hSnapShot = pCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (hSnapShot == (HANDLE)-1)
		return NULL;

	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

	bResult = pModule32First(hSnapShot, &ModuleEntry32);
	if (!bResult)
	{
		CloseHandle(hSnapShot);
		return NULL;
	}

	do
	{
        if (stricmp(ModuleEntry32.szModule, lpModName) == 0)
        {
            strncpy(lpModPathBuf, ModuleEntry32.szExePath, nModPathBufLen);
            hModule = ModuleEntry32.hModule;
            break;
        }

	} while (pModule32Next(hSnapShot, &ModuleEntry32) && m_nModCount <MAX_MODULE_COUNT);

	CloseHandle(hSnapShot);

	return hModule;
}

DWORD CProcessModule::GetProcessID(char *exe_name)
{
	EnumProcess();

    for(int i =0; i<(int)m_dwProcessCount; i++)
	{
		int mod_count =EnumProcessModules(m_dwProcessIDs[i]);
		char mod_base_name[100];

        for(int j =0; j<mod_count; j++)
		{
			GetModuleBaseNameA(j, mod_base_name);
			if(strcmpi(mod_base_name, exe_name) ==0)
			{
				EndEnumProcessModules();
				return m_dwProcessIDs[i];
			}
		}
	}


	EndEnumProcessModules();

	return NULL;
}

#ifdef WINNT
BOOL CProcessModule::ObtainSeDebugPrivilege(HANDLE hProcess)
{
	BOOL Result;
	TOKEN_PRIVILEGES TokenPrivileges;
	TOKEN_PRIVILEGES PreviousTokenPrivileges;
	LUID luid;
	HANDLE hToken;
	DWORD dwPreviousTokenPrivilegesSize = sizeof(TOKEN_PRIVILEGES);

	Result = OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

	if (Result == FALSE)
		return false;

	Result = LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

	if (Result == FALSE)
		return false;
  
	TokenPrivileges.PrivilegeCount            = 1;
	TokenPrivileges.Privileges[0].Luid        = luid;
	TokenPrivileges.Privileges[0].Attributes  = 0;

	AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES),
				&PreviousTokenPrivileges, &dwPreviousTokenPrivilegesSize);

	if (GetLastError() != ERROR_SUCCESS)
		return false;

	PreviousTokenPrivileges.PrivilegeCount             = 1;
	PreviousTokenPrivileges.Privileges[0].Luid         = luid;
	PreviousTokenPrivileges.Privileges[0].Attributes  |= SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &PreviousTokenPrivileges,
		dwPreviousTokenPrivilegesSize, NULL, NULL);

	if (GetLastError() != ERROR_SUCCESS)
		return false;

	CloseHandle(hToken);
	return true;
}
#endif
