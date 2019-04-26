#include "cobApiHook.h"
#include "cobUtils.h"
#include <map>

NS_COB_BEGIN

static std::map<DWORD, ApiHook::HookInfo_t> m_ApiHookMap;

ApiHook::ApiHook()
{
    m_pOriginCode = NULL;
    m_pfnOrigin = NULL;
}

ApiHook::~ApiHook()
{

}

static PVOID WhereToGoNext(DWORD dwOriginAddr, PBYTE pCallerAddr)
{
    ApiHook::HookInfo_t hookInfo;

    auto iter = m_ApiHookMap.find(dwOriginAddr);
    if (iter == m_ApiHookMap.end())
    {
        COBLOG("WhereToGo failed: [%x] not found in map\n", dwOriginAddr);
        return NULL;
    }

    hookInfo = iter->second;

    COBLOG("scope=(%x,%x) caller=%x\n", hookInfo.pAddrFrom, hookInfo.pAddrTo, pCallerAddr);

    COBLOG("dwOriginAddr=%x, pCallerAddr=%x, origincode=%x, cbfunc=%x\n", dwOriginAddr, pCallerAddr,
        hookInfo.pOriginCode, hookInfo.pCbFunc);

    if (pCallerAddr >= hookInfo.pAddrFrom && pCallerAddr <= hookInfo.pAddrTo)
        return hookInfo.pOriginCode;
    else
        return hookInfo.pCbFunc;
}


/*
Low
 ---------
| temp    |
 ---------
|originalcode|
 ---------
|hookcbfunc|
 ---------
| edxbak  |
 ---------
| ecxbak  |
 ---------
| eaxbak  |
 ---------
| ebpbak  |
 ---------
| espbak  |
 ---------
| origaddr|
 ---------  <--ebp within current func
| ebp     |
 ---------  <--esp when enter this func
| retaddr |
 ---------  <--espbak
High
*/
static void ApiHook_CallbackProc()
{
    DWORD origaddr = 0;
    DWORD espbak = 0;
    DWORD ebpbak = 0;
    DWORD eaxbak = 0;
    DWORD ecxbak = 0;
    DWORD edxbak = 0;

    DWORD ebxbak = 0;
    DWORD edibak = 0;
    DWORD esibak = 0;

    PVOID nexttogo = NULL;
    PBYTE calleraddr = 0;
    DWORD temp = 0;

    __asm
    {
        //make a mark
        push 0x11223344 

        //vc complier use ebp to access the variables in stack so should set ebp=esp at the beginning
        push ebp
        mov  ebp, esp

        //ebpbak & origaddr is stack variables so we have to access them after set ebp=esp
        pop  ebpbak
        pop  origaddr
        pop  calleraddr;
        push calleraddr;

        //store esp
        mov  espbak, esp
        //store flag register
        pushfd

        //the space size can't be less than 20 which equals sizeof(stack-variables)+sizeof(ebp)+sizeof(retaddr)) 
        sub  esp, 40h

        //store eax & ecx, which will be changed in GetHookCbFunc
        mov  eaxbak, eax
        mov  ecxbak, ecx
        mov  edxbak, edx
        mov  ebxbak, ebx
        mov  edibak, edi
        mov  esibak, esi

        sub  origaddr, 5  //after pushfd for possible affect flag
    }

    if ((nexttogo = WhereToGoNext(origaddr, calleraddr)) != NULL)
    {
        __asm
        {
            mov eax, eaxbak
            mov ebx, ebxbak
            mov ecx, ecxbak
            mov edx, edxbak
            mov esi, esibak
            mov edi, edibak
            
            add  esp, 40h
            popfd
            
            push nexttogo
            mov  ebp, ebpbak
            ret  
        }
    }
}

BOOL ApiHook::HookFunc(PSTR pszCalleeModName, PSTR pszFuncName, PROC pfnHook, int nHookSize)
{
    HMODULE hCalleeModule;
    PBYTE   pOrginFunc;
    DWORD   dwOldProtect;

    if (m_pOriginCode == NULL)
    {
        m_pOriginCode = (PBYTE)malloc(32);
        if (m_pOriginCode == NULL)
        {
            COBLOG("ApiHook::HookFunc: malloc failed\n");
            return FALSE;
        }
    }
    
    hCalleeModule = GetModuleHandle(pszCalleeModName);
    if (hCalleeModule == NULL)
    {
        COBLOG("ApiHook::HookFunc: GetModuleHandle(%s) failed\n", pszCalleeModName);
        return FALSE;
    }

    pOrginFunc = (PBYTE)GetProcAddress(hCalleeModule, pszFuncName);
    if (pOrginFunc == NULL)
    {
        COBLOG("GetProcAddress failed\n");
        return FALSE;
    }

    m_nOriginCodeLen = Rehook(pOrginFunc, ApiHook_CallbackProc, m_pOriginCode, 32, nHookSize);
    if (m_nOriginCodeLen == -1)
    {
        COBLOG("ApiHook::HookFun: Rehook failed\n");
        return FALSE;
    }

    //maybe not necessary but rather add it that look more reasonable
    VirtualProtect(m_pOriginCode, m_nOriginCodeLen + 6, PAGE_EXECUTE_READWRITE, &dwOldProtect); //PAGE_EXECUTE_READ cannot work
    m_pOriginCode[m_nOriginCodeLen] = 0x68;               //push
    *(DWORD*)&m_pOriginCode[m_nOriginCodeLen + 1] = (DWORD)(pOrginFunc + m_nOriginCodeLen);
    m_pOriginCode[m_nOriginCodeLen + 5] = 0xc3;           //ret

    //HOOK地址
    m_pfnOrigin = (PROC)pOrginFunc;

    //加入map表管理
    MEMORY_BASIC_INFORMATION mbi;
    ApiHook::HookInfo_t hookInfo = {(PBYTE)m_pfnOrigin, pfnHook, m_pOriginCode, m_nOriginCodeLen};
	VirtualQuery(pfnHook, &mbi, sizeof(mbi));
    hookInfo.pAddrFrom = (PBYTE)mbi.AllocationBase;
    hookInfo.pAddrTo = (PBYTE)mbi.BaseAddress + mbi.RegionSize; //must be BaseAddress instead of AllocationBase
    m_ApiHookMap.insert(std::make_pair((DWORD)m_pfnOrigin, hookInfo));

    COBLOG("OriginFunc=%s (%x), HookSpace=%d\n", pszFuncName, pOrginFunc, m_nOriginCodeLen);

    return TRUE;
}

static void CallHook_CallbackProc()
{
    DWORD origaddr = 0;
    DWORD espbak = 0;
    DWORD ebpbak = 0;
    DWORD eaxbak = 0;
    DWORD ecxbak = 0;
    DWORD edxbak = 0;

    DWORD ebxbak = 0;
    DWORD edibak = 0;
    DWORD esibak = 0;

    PVOID nexttogo = NULL;
    DWORD temp = 0;

    __asm
    {
        //make a mark
        push 0x11223344 

        //vc complier use ebp to access the variables in stack so should set ebp=esp at the beginning
        push ebp
        mov  ebp, esp

        //ebpbak & origaddr is stack variables so we have to access them after set ebp=esp
        pop  ebpbak
        pop  origaddr
        push origaddr

        //store esp
        mov  espbak, esp
        //store flag register
        pushfd

        //the space size can't be less than 20 which equals sizeof(stack-variables)+sizeof(ebp)+sizeof(retaddr)) 
        sub  esp, 40h

        //store eax & ecx, which will be changed in GetHookCbFunc
        mov  eaxbak, eax
        mov  ecxbak, ecx
        mov  edxbak, edx
        mov  ebxbak, ebx
        mov  edibak, edi
        mov  esibak, esi

        sub  origaddr, 5  //after pushfd for possible affect flag
    }

    if ((nexttogo = WhereToGoNext(origaddr, 0)) != NULL)
    {
        __asm
        {
            mov eax, eaxbak
            mov ebx, ebxbak
            mov ecx, ecxbak
            mov edx, edxbak
            mov esi, esibak
            mov edi, edibak
            
            add  esp, 40h
            popfd
            
            push nexttogo
            mov  ebp, ebpbak
            ret  
        }
    }
}

BOOL ApiHook::HookCall(DWORD dwHookAddr, PROC pfnHook, int nHookSize)
{
    DWORD   dwOldProtect;

    if (m_pOriginCode == NULL)
    {
        m_pOriginCode = (PBYTE)malloc(32);
        if (m_pOriginCode == NULL)
        {
            COBLOG("ApiHook::HookFunc: malloc failed\n");
            return FALSE;
        }
    }
    
    m_nOriginCodeLen = Rehook((PBYTE)dwHookAddr, CallHook_CallbackProc, m_pOriginCode, 32, nHookSize);
    if (m_nOriginCodeLen == -1)
    {
        COBLOG("ApiHook::HookFun: Rehook failed\n");
        return FALSE;
    }

    //maybe not necessary but rather add it that look more reasonable
    VirtualProtect(m_pOriginCode, m_nOriginCodeLen + 6, PAGE_EXECUTE_READWRITE, &dwOldProtect); //PAGE_EXECUTE_READ cannot work
    m_pOriginCode[m_nOriginCodeLen] = 0x68;               //push
    *(DWORD*)&m_pOriginCode[m_nOriginCodeLen + 1] = (DWORD)(dwHookAddr + m_nOriginCodeLen);
    m_pOriginCode[m_nOriginCodeLen + 5] = 0xc3;           //ret

    //HOOK地址
    m_pfnOrigin = (PROC)dwHookAddr;

    //加入map表管理
    MEMORY_BASIC_INFORMATION mbi;
    ApiHook::HookInfo_t hookInfo = {(PBYTE)m_pfnOrigin, pfnHook, m_pOriginCode, m_nOriginCodeLen};
	VirtualQuery(pfnHook, &mbi, sizeof(mbi));
    hookInfo.pAddrFrom = (PBYTE)mbi.AllocationBase;
    hookInfo.pAddrTo = (PBYTE)mbi.BaseAddress + mbi.RegionSize; //must be BaseAddress instead of AllocationBase
    m_ApiHookMap.insert(std::make_pair((DWORD)m_pfnOrigin, hookInfo));

    COBLOG("HookAddr=%x, HookSpace=%d, HookFunc=%p\n", dwHookAddr, m_nOriginCodeLen, pfnHook);

    return TRUE;
}

BOOL ApiHook::UnhookFunc()
{
    if (m_pfnOrigin != NULL && m_pOriginCode != NULL)
    {
        Unhook((PBYTE)m_pfnOrigin, m_pOriginCode, m_nOriginCodeLen);
        free(m_pOriginCode);
        m_pOriginCode = NULL;

        auto iter = m_ApiHookMap.find((DWORD)m_pfnOrigin);
        if (iter != m_ApiHookMap.end())
        {
            m_ApiHookMap.erase(iter);
            m_pfnOrigin = NULL;
        }
        return TRUE;
    }

    return FALSE;
}

BOOL ApiHook::HookPause()
{
    if (m_pfnOrigin != NULL && m_pOriginCode != NULL)
    {
        return Unhook((PBYTE)m_pfnOrigin, m_pOriginCode, m_nOriginCodeLen);
    }  
    
    return FALSE;
}

BOOL ApiHook::HookResume()
{
    if (m_pfnOrigin != NULL && m_pOriginCode != NULL)
    {
        m_nOriginCodeLen = Rehook((PBYTE)m_pfnOrigin, ApiHook_CallbackProc, m_pOriginCode, 32, m_nOriginCodeLen);
        if (m_nOriginCodeLen != -1)
            return TRUE; 
    }

    return FALSE;
}

PBYTE ApiHook::MarkFind(PBYTE pBuf, int nBufLen, const PBYTE pMark, int nMarkSize)
{
    for (int i = 0; i <= nBufLen - nMarkSize; i++)
    {
        if (memcmp(pBuf + i, pMark, nMarkSize) == 0)
            return pBuf + i;
    }

    return NULL;
}

int ApiHook::GetHookCodeSize(PBYTE pHookAddr, int nMinSize)
{
    int width = 0; //opcode width
    int size = 0;  //code size at hook address

    do
    {
        width = GetOpCodeSize(pHookAddr + size);
        size += width;

    } while (size < nMinSize);

    return size;
}

int ApiHook::Rehook(PBYTE pOriginAddr, void *pCbFunc, PBYTE pOriginCode, int nOriginCodeBufLen, int nHookSize)
{
    DWORD dwOldProtect;
    BYTE *entryaddr;
    DWORD offsetaddr; //跳到这里执行
    int   hooksize = 0;
    BYTE  markcode[] = { 0x68, 0x44, 0x33, 0x22, 0x11 };

    entryaddr = (BYTE*)MarkFind((PBYTE)pCbFunc, 256, markcode, sizeof(markcode));
    if (entryaddr == NULL)
    {
        COBLOG("ApiHook::Rehook: markcode not found ---\n");
        return -1;
    }

    //跳入标记的位置(即: markcode所标识码)
    entryaddr += 5; //跳过标识码

    if (nHookSize == -1)
    {
        hooksize = GetHookCodeSize(pOriginAddr, 5);
    }
    else
    {
        hooksize = nHookSize;
    }
    //COBLOG("pOriginAddr=%p, HookSpace=%d\n", pOriginAddr, hooksize);

    if (nOriginCodeBufLen < hooksize)
    {
        COBLOG("ApiHook::Rehook: nOriginCodeBuf is not enough ---\n");
        return -1;
    }

    BOOL bResult = VirtualProtect(pOriginAddr, hooksize, PAGE_READWRITE, &dwOldProtect);

    memcpy(pOriginCode, pOriginAddr, hooksize);    //store orginal code
    switch (pOriginCode[0])
    {
    case 0xe8:  //call
    case 0xe9:  //offset jmp
    {
        COBLOG("call offset_addr1=%x\n", *(long*)&pOriginCode[1]);
        *(long*)&pOriginCode[1] += (long)(pOriginAddr - pOriginCode);
        COBLOG("call offset_addr2=%x\n", *(long*)&pOriginCode[1]);
    }
    break;

    case 0xff:
    {

    }
    break;

    default:
        break;
    }

    offsetaddr = entryaddr - pOriginAddr - 5;

    pOriginAddr[0] = 0xe8;  //call offset addr (goto our func)
    memcpy(&pOriginAddr[1], &offsetaddr, 4);
    memset(&pOriginAddr[5], 0x90, hooksize - 5);

    bResult = VirtualProtect(pOriginAddr, hooksize, dwOldProtect, &dwOldProtect);

    return hooksize;
}

BOOL ApiHook::Unhook(PBYTE pOriginAddr, PBYTE pOriginCode, int nOriginCodeLen)
{
    DWORD dwOldProtect;

    VirtualProtect(pOriginAddr, nOriginCodeLen, PAGE_READWRITE, &dwOldProtect);
    memcpy(pOriginAddr, pOriginCode, nOriginCodeLen);
    VirtualProtect(pOriginAddr, nOriginCodeLen, dwOldProtect, &dwOldProtect);

    return TRUE;
}

static DWORD MaskTable[518] =
{
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00008000, 0x00008000, 0x00000000, 0x00000000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00008000, 0x00008000, 0x00000000, 0x00000000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00008000, 0x00008000, 0x00000000, 0x00000000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00008000, 0x00008000, 0x00000000, 0x00000000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00008000, 0x00008000, 0x00000008, 0x00000000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00008000, 0x00008000, 0x00000008, 0x00000000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00008000, 0x00008000, 0x00000008, 0x00000000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00008000, 0x00008000, 0x00000008, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00004000, 0x00004000,
  0x00000008, 0x00000008, 0x00001008, 0x00000018,
  0x00002000, 0x00006000, 0x00000100, 0x00004100,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000100, 0x00000100, 0x00000100, 0x00000100,
  0x00000100, 0x00000100, 0x00000100, 0x00000100,
  0x00000100, 0x00000100, 0x00000100, 0x00000100,
  0x00000100, 0x00000100, 0x00000100, 0x00000100,
  0x00004100, 0x00006000, 0x00004100, 0x00004100,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00002002, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000020, 0x00000020, 0x00000020, 0x00000020,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000100, 0x00002000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000100, 0x00000100, 0x00000100, 0x00000100,
  0x00000100, 0x00000100, 0x00000100, 0x00000100,
  0x00002000, 0x00002000, 0x00002000, 0x00002000,
  0x00002000, 0x00002000, 0x00002000, 0x00002000,
  0x00004100, 0x00004100, 0x00000200, 0x00000000,
  0x00004000, 0x00004000, 0x00004100, 0x00006000,
  0x00000300, 0x00000000, 0x00000200, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00000100, 0x00000100, 0x00000000, 0x00000000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00000100, 0x00000100, 0x00000100, 0x00000100,
  0x00000100, 0x00000100, 0x00000100, 0x00000100,
  0x00002000, 0x00002000, 0x00002002, 0x00000100,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000008, 0x00000000, 0x00000008, 0x00000008,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00004000, 0x00004000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0x00002000, 0x00002000, 0x00002000, 0x00002000,
  0x00002000, 0x00002000, 0x00002000, 0x00002000,
  0x00002000, 0x00002000, 0x00002000, 0x00002000,
  0x00002000, 0x00002000, 0x00002000, 0x00002000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00000000, 0x00000000, 0x00000000, 0x00004000,
  0x00004100, 0x00004000, 0xFFFFFFFF, 0xFFFFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00004000,
  0x00004100, 0x00004000, 0xFFFFFFFF, 0x00004000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0xFFFFFFFF, 0xFFFFFFFF, 0x00004100, 0x00004000,
  0x00004000, 0x00004000, 0x00004000, 0x00004000,
  0x00004000, 0x00004000, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0x00000000, 0x00000000, 0x00000000, 0x00000000,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xFFFFFFFF, 0xFFFFFFFF
};

static DWORD GetOpCodeSize_ASM_CODE(void* Start, void* Tlb)
{
    __asm {
        pushad
        mov   esi, [esp + 24h]
        mov   ecx, [esp + 28h]
        xor edx, edx
        xor   eax, eax
        L005 :
        and   dl, 0F7h
            mov   al, [ecx]
            inc   ecx
            or edx, [esi + eax * 4h]
            test   dl, 8h
            jnz L005
            cmp   al, 0F6h
            je L035
            cmp   al, 0F7h
            je L035
            cmp   al, 0CDh
            je L040
            cmp   al, 0Fh
            je L045
            L019 :
        test   dh, 80h
            jnz L052
            L021 :
        test   dh, 40h
            jnz L067
            L023 :
        test   dl, 20h
            jnz L057
            L025 :
        test   dh, 20h
            jnz L062
            L027 :
        mov   eax, ecx
            sub   eax, [esp + 28h]
            and edx, 707h
            add   al, dl
            add   al, dh
            L032 :
        mov[esp + 1Ch], eax
            popad
            retn
            L035 :
        or dh, 40h
            test   byte ptr[ecx], 38h
            jnz L019
            or dh, 80h
            jmp L019
            L040 :
        or dh, 1h
            cmp   byte ptr[ecx], 20h
            jnz L019
            or dh, 4h
            jmp L019
            L045 :
        mov   al, [ecx]
            inc   ecx
            or edx, [esi + eax * 4h + 400h]
            cmp   edx, -1h
            jnz L019
            mov   eax, edx
            jmp L032
            L052 :
        xor   dh, 20h
            test   al, 1h
            jnz L021
            xor   dh, 21h
            jmp L021
            L057 :
        xor   dl, 2h
            test   dl, 10h
            jnz L025
            xor   dl, 6h
            jmp L025
            L062 :
        xor   dh, 2h
            test   dh, 10h
            jnz L027
            xor   dh, 6h
            jmp L027
            L067 :
        mov   al, [ecx]
            inc   ecx
            mov   ah, al
            and   ax, 0C007h
            cmp   ah, 0C0h
            je L023
            test   dl, 10h
            jnz L090
            cmp   al, 4h
            jnz L080
            mov   al, [ecx]
            inc   ecx
            and   al, 7h
            L080 :
        cmp   ah, 40h
            je L088
            cmp   ah, 80h
            je L086
            cmp   ax, 5h
            jnz L023
            L086 :
        or dl, 4h
            jmp L023
            L088 :
        or dl, 1h
            jmp L023
            L090 :
        cmp   ax, 6h
            je L096
            cmp   ah, 40h
            je L088
            cmp   ah, 80h
            jnz L023
            L096 :
        or dl, 2h
            jmp L023
            retn
    }
}

DWORD ApiHook::GetOpCodeSize(PBYTE pStart)
{
    DWORD nLen = 0;

    __asm
    {
        push pStart
        push offset MaskTable
        call GetOpCodeSize_ASM_CODE
        mov  nLen, eax
        add  esp, 8
    }

    return nLen;
}

NS_COB_END