#if !defined(AFX_MAGICSEARCH_H__6C8F220F_A19F_11D6_98C2_C99B4152F509__INCLUDED_)
#define AFX_MAGICSEARCH_H__6C8F220F_A19F_11D6_98C2_C99B4152F509__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "windows.h"

#define MAX_BUF_SIZE 5000
#define MAX_PATTERN_SIZE 1024

typedef struct
{
    char* pBuf;
    int   nLen;

}MagicCode_t;

PBYTE WBS_MagicSearch(DWORD dwProcessId, MagicCode_t *pMagicCode, DWORD dwStartAddr, DWORD dwEndAddr);
int   WBS_MemFind(int iStartPosition, LPBYTE pDestBuffer, int iDestBufferLength, LPBYTE pPatternBuffer, int iPatternBufferLength);

#endif // !defined(AFX_MAGICSEARCH_H__6C8F220F_A19F_11D6_98C2_C99B4152F509__INCLUDED_)
