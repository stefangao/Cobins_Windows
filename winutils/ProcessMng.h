#ifndef __PROCESS_MODULE_H__
#define __PROCESS_MODULE_H__

#include <tlhelp32.h>
typedef HANDLE (WINAPI *CREATETOOLHELP32SNAPSHOT_PROC)(DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL (WINAPI *PROCESS32FIRST_PROC)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI *PROCESS32NEXT_PROC)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI *MODULE32FIRST_PROC)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef BOOL (WINAPI *MODULE32NEXT_PROC)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);

#define MAX_PROCESS_COUNT	512
#define MAX_MODULE_COUNT	500

class CProcessModule
{
public:
	CProcessModule();
	~CProcessModule();
	
private:
	char m_mod_name[MAX_MODULE_COUNT][100];  // max 100 modules in one process
	CREATETOOLHELP32SNAPSHOT_PROC pCreateToolhelp32Snapshot;
	PROCESS32FIRST_PROC pProcess32First;
	PROCESS32NEXT_PROC pProcess32Next;
	MODULE32FIRST_PROC pModule32First;
	MODULE32NEXT_PROC pModule32Next;

public:
	DWORD m_dwProcessIDs[MAX_PROCESS_COUNT];  // 
	DWORD m_dwProcessCount;
	HMODULE m_hMods[MAX_MODULE_COUNT];
	int m_nModCount;

	int EnumProcess();
	int EnumProcessModules(DWORD process_id);
	int GetModuleBaseNameA(int num, char *mod_name);
	void EndEnumProcessModules();

	HANDLE GetProcessModuleHandle(DWORD dwProcessId, LPCSTR lpModName);
    HMODULE GetProcessModuleHandleEx(DWORD dwProcessId, LPCSTR lpModName, LPTSTR lpModPathBuf, int nModPathBufLen);

	DWORD  GetProcessID(char *exe_name);

#ifdef WINNT
	BOOL ObtainSeDebugPrivilege(HANDLE hProcess);
#endif
};


#endif
