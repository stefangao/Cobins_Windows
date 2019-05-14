#ifndef __MAGIC_SEARCH_H__
#define __MAGIC_SEARCH_H__

#include "windows.h"
#include "cobMacros.h"

NS_COB_BEGIN

#define MAX_BUF_SIZE 5000
#define MAX_PATTERN_SIZE 1024

typedef struct
{
    char* pBuf;
    int   nLen;

}MagicCode_t;

PBYTE WBS_MagicSearch(DWORD dwProcessId, MagicCode_t *pMagicCode, DWORD dwStartAddr, DWORD dwEndAddr);
int   WBS_MemFind(int iStartPosition, LPBYTE pDestBuffer, int iDestBufferLength, LPBYTE pPatternBuffer, int iPatternBufferLength);

NS_COB_END
#endif
