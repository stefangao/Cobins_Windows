#ifndef LIBINJECTMNG_H__
#define LIBINJECTMNG_H__

HINSTANCE LIM_InjectLib(DWORD process_id, LPCSTR lib_path, DWORD &dwErrorCode);
HINSTANCE LIM_WinInjectLib(LPCSTR lpLibPath, HWND hTargetWnd, DWORD &dwErrCode);

BOOL LIM_EjectLib(DWORD process_id, LPCSTR lib_name);
BOOL LIM_WinEjectLib(LPCSTR lpLibPath, HWND hTargetWnd);
BOOL LIM_WinEjectLibEx(HWND hDestWnd, HMODULE hModule);

BOOL LIM_HookAPI(HWND hHookWnd, LPCSTR lpLibPath);
BOOL LIM_UnhookAPI(HWND hHookWnd, LPCSTR lpLibName);

BOOL LIM_IsDllExist(LPCSTR lpDllName, HWND hDestWnd);

/*
HINSTANCE WINAPI LIM_WinInjectLib7(LPCSTR lpLibPath, HWND hTargetWnd);
BOOL WINAPI LIM_EnableDebugPrivilege(HANDLE hProcess);

HANDLE WINAPI LIM_WinInjectLibX(LPCSTR lpLibPath, HWND hDestWnd);
BOOL   WINAPI LIM_WinEjectLibX(LPCSTR lpLibPath, HWND hDestWnd);

HANDLE WINAPI LIM_RemoteHook(HWND hDestWnd, LPCSTR lpLibPath, PBYTE pHookFunc);
BOOL   WINAPI LIM_RemoteUnhook(HANDLE hInjectHook);
BOOL   WINAPI LIM_IsRemoteHooked(HWND hDestWnd);*/


#endif
