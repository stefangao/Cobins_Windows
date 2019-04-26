#ifndef __COB_API_HOOK_H__
#define __COB_API_HOOK_H__

#include <windows.h>
#include "cobMacros.h"

NS_COB_BEGIN

class ApiHook
{
private:
	PROC  m_pfnOrigin;					//Original function aaddress in callee
    PBYTE m_pOriginCode;
    int   m_nOriginCodeLen;

public:
	ApiHook();
	~ApiHook();

public:
	BOOL HookFunc(PSTR pszCalleeModName, PSTR pszFuncName, PROC pfnHook, int nHookSize = -1);
    BOOL HookCall(DWORD dwHookAddr, PROC pfnHook, int nHookSize = -1);
	BOOL UnhookFunc();
    BOOL UnhookCall() {return UnhookFunc();};
    PROC GetOriginFunc() {return m_pfnOrigin;};

    BOOL HookPause();
    BOOL HookResume();

public:
    int   Rehook(PBYTE pOriginAddr, void *pCbFunc, PBYTE pOriginCode, int nOriginCodeBufLen, int nHookSize);
    BOOL  Unhook(PBYTE pOriginAddr, PBYTE pOriginCode, int nOriginCodeLen);

    int   GetHookCodeSize(PBYTE pHookAddr, int nMinSize);
    PBYTE MarkFind(PBYTE pBuf, int nBufLen, const PBYTE pMark, int nMarkSize);
    DWORD GetOpCodeSize(PBYTE pStart);

    typedef struct
    {
        PBYTE        pOriginAddr;
        PVOID        pCbFunc;
        PBYTE        pOriginCode;
        int          nOriginCodeLen;
        PBYTE        pAddrFrom;
        PBYTE        pAddrTo;
    } HookInfo_t;
};

template <class FromType>
static PVOID GetMemberFuncAddr(FromType f)
{
    union
    {
        FromType _f;
        PVOID    _t;
    }ut;
    ut._f = f;
    return ut._t;
}

NS_COB_END

#endif