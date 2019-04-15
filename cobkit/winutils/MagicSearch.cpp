#include "MagicSearch.h"
#include "wtermin.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static int GetMagicPattenSize(MagicCode_t *pMagicCode)
{
    int nPattenSize = 0;

    if (pMagicCode == NULL)
        return -1;

    //去掉偏移控制长度
    if (pMagicCode->pBuf == NULL && pMagicCode->nLen != 0)
        pMagicCode++;

    for (; (pMagicCode->pBuf != NULL || pMagicCode->nLen > 0); pMagicCode++)
    {
        nPattenSize += pMagicCode->nLen;
    }

    return nPattenSize;
}

static int _MagicFind(int nStartPosition, LPBYTE pDestBuffer, int nDestBufferLength, MagicCode_t *pMagicCode)
{
	int nFoundPosition = -1;

    if (pMagicCode == NULL)
        return -1;

    if (pMagicCode->pBuf == NULL && pMagicCode->nLen == 0)
        return nStartPosition;

    if (pMagicCode->pBuf == NULL && pMagicCode->nLen > 0)
    {
        nFoundPosition = _MagicFind(nStartPosition + pMagicCode->nLen, pDestBuffer, nDestBufferLength, pMagicCode + 1);
        if (nFoundPosition >= 0)
        {
            nFoundPosition = nStartPosition;
        }
    }
    else
    {
        if (nStartPosition +  pMagicCode->nLen <= nDestBufferLength)
        {
            if (memcmp(&pDestBuffer[nStartPosition], pMagicCode->pBuf, pMagicCode->nLen) == 0)
            {
                nFoundPosition = _MagicFind(nStartPosition + pMagicCode->nLen, pDestBuffer, nDestBufferLength, pMagicCode + 1);
                if (nFoundPosition >= 0)
                {
                    nFoundPosition = nStartPosition;
                }
            }
        }
    }

	return nFoundPosition;
}

//匹配第一个magic
static int MagicFind(int nStartPosition, LPBYTE pDestBuffer, int nDestBufferLength, MagicCode_t *pMagicCode, DWORD &nTargetPosition)
{
	int nFoundPosition = -1, i = 0;

    if (pMagicCode == NULL)
        return -1;

    if (pMagicCode->pBuf == NULL && pMagicCode->nLen == 0)
        return nStartPosition;

    //第一个控制偏移 (如果pMagicCode->pBuf==NULL)
    if (pMagicCode->pBuf == NULL)
    {
        nFoundPosition = MagicFind(nStartPosition, pDestBuffer, nDestBufferLength, pMagicCode + 1, nTargetPosition);
        if (nFoundPosition >= 0)
        {
            nTargetPosition = nFoundPosition + pMagicCode->nLen;
        }
    }
    else
    {
        for (i = nStartPosition; i < nDestBufferLength; i++)
        {
            if (memcmp(&pDestBuffer[i], pMagicCode->pBuf, pMagicCode->nLen) == 0)
            {
                nFoundPosition = _MagicFind(i + pMagicCode->nLen, pDestBuffer, nDestBufferLength, pMagicCode + 1);
                if (nFoundPosition >= 0)
                {
                    nFoundPosition = i;
                    nTargetPosition = nFoundPosition;
                    break;
                }
            }
        }
    }

	return nFoundPosition;
}

PBYTE WBS_MagicSearch(DWORD dwProcessId, MagicCode_t *pMagicCode, DWORD dwStartAddr, DWORD dwEndAddr)
{
	MEMORY_BASIC_INFORMATION mbi;
	DWORD nPatternSize;
	DWORD dwBytesLeft, dwReadedBytes;
	DWORD dwOffset = 0;
	DWORD dwBaseAddress, dwCurrentPos;
	DWORD nPatternPos;
	BYTE  CacheBuffer[MAX_BUF_SIZE];
	int   nStartPos;
	
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if(hProcess == NULL)
    {
        WT_Error("WBS_MagicSearch: OpenProcess Failed");
        return NULL;
    }

	//WT_Trace("MinAddress = %X, MaxAddress = %X\n",  dwStartAddr, dwEndAddr);

    nPatternSize = GetMagicPattenSize(pMagicCode);
    dwBaseAddress = dwStartAddr;

	while(dwBaseAddress < dwEndAddr)
	{
	  	VirtualQueryEx(hProcess, (PVOID)dwBaseAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		/*
		if (mbi.Protect != PAGE_EXECUTE_READ && mbi.Protect != PAGE_EXECUTE_READWRITE)
			continue;*/
		
        if ((DWORD)mbi.BaseAddress < dwStartAddr)
        {
		    dwOffset = dwStartAddr - (DWORD)mbi.BaseAddress;
    		dwBytesLeft = mbi.RegionSize - dwOffset;
            dwReadedBytes = 0;
        }
        else
        {
            dwOffset = 0;
            dwBytesLeft = mbi.RegionSize;
            dwReadedBytes = 0;
        }
		
		while(dwBytesLeft >= nPatternSize)
		{			
			dwCurrentPos = (DWORD)mbi.BaseAddress + dwOffset;
            if(!ReadProcessMemory(hProcess, (PVOID)dwCurrentPos, CacheBuffer, min(dwBytesLeft, MAX_BUF_SIZE), &dwReadedBytes))
            {
                WT_Error("WBS_MagicSearch: ReadProcessMemory Failed");
               	CloseHandle(hProcess);
                return NULL;
            }

			nStartPos = 0;
			do
			{
                DWORD nTargetPosition = 0;
				nPatternPos = MagicFind(nStartPos, CacheBuffer, dwReadedBytes, pMagicCode, nTargetPosition);
				if(nPatternPos != -1)
				{
                    PBYTE pFoundAddr = NULL;
                    
                    pFoundAddr = (PBYTE)mbi.BaseAddress + dwOffset + nTargetPosition;
					nStartPos = nPatternPos + nPatternSize;

                    //WT_Trace("foundaddr=%x\n", pFoundAddr);

                  	CloseHandle(hProcess);
                    return (PBYTE)pFoundAddr;
				}
				
            } while(nPatternPos != -1);
			
			dwOffset = dwOffset +  dwReadedBytes - nPatternSize + 1;
			dwBytesLeft = mbi.RegionSize - dwOffset;
		}

        dwBaseAddress = (DWORD)mbi.BaseAddress + mbi.RegionSize;
	}
	
	CloseHandle(hProcess);

    return NULL;
}

//在一个内存块中搜索内容
//成功返回位置
//失败返回-1
int WBS_MemFind(int iStartPosition, LPBYTE pDestBuffer, int iDestBufferLength, LPBYTE pPatternBuffer, int iPatternBufferLength)
{
	
	signed int iFoundPosition, i;
	
	iFoundPosition = -1;
	if(iStartPosition > iDestBufferLength)return -1;
	
	for(i = iStartPosition; i < (iDestBufferLength + 1); i++)
	{
        if(memcmp(&pDestBuffer[i], pPatternBuffer, iPatternBufferLength) == 0)
        {
            iFoundPosition = i;
            break;
        }
    }
	return iFoundPosition;
}