#include "wndshm.h"
#include "wtermin.h"

#define NAME_MAXLEN    256

typedef struct
{
    HANDLE hShmFile;
    HANDLE hMutex;
    void  *pData;
    int    nDataSize;

}SHMINFO_T;

void* WBS_Shm_Create(LPCSTR lpShmName, int nSize)
{
    char lpNewName[NAME_MAXLEN+1] = {0};
    HANDLE hShmFile, hMutex;
  	SHMINFO_T* pShminfo;
    void* pData;
    
    //Create Mutext (name is like "MTX&ABCDEF")
    strcpy(lpNewName, "MTX&");
    strncpy(lpNewName + 4, lpShmName, NAME_MAXLEN - 4);
    lpNewName[NAME_MAXLEN] = '\0';
    hMutex = CreateMutex(NULL, FALSE, lpNewName);
    if (hMutex == NULL)
        return NULL;

    //Create Shm (name is like "SHM&ABCDEF")
    strcpy(lpNewName, "SHM&");
    strncpy(lpNewName + 4, lpShmName, NAME_MAXLEN - 4);
    lpNewName[NAME_MAXLEN] = '\0';
    hShmFile = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, nSize, lpNewName);
	if (hShmFile == NULL)
    {
        CloseHandle(hMutex);
		return NULL;
    }

    pData = MapViewOfFile(hShmFile, FILE_MAP_WRITE, 0, 0, nSize);
    if (pData == NULL)
    {
        CloseHandle(hMutex);
        CloseHandle(hShmFile);
		return NULL;
    }
 
    //Malloc and store the context
    pShminfo = (SHMINFO_T*)malloc(sizeof(SHMINFO_T));
	if (pShminfo == NULL)
    {
        CloseHandle(hMutex);
        CloseHandle(hShmFile);
		return NULL;
    }

    pShminfo->hShmFile = hShmFile;
    pShminfo->pData = pData;
    pShminfo->nDataSize = nSize;
    pShminfo->hMutex = hMutex;

    return pShminfo;
}

BOOL WBS_Shm_Destroy(HANDLE hShm)
{
  	SHMINFO_T* pShminfo;

    pShminfo = (SHMINFO_T*)hShm;
    if (!UnmapViewOfFile(pShminfo->pData))
    {
        WT_Error("WBS_Shm_Destroy: UnmapViewOfFile lasterr=%d\n", GetLastError());
    }

    if (!CloseHandle(pShminfo->hShmFile))
    {
        WT_Error("WBS_Shm_Destroy: CloseHandle1 error\n");
   }

    if (!CloseHandle(pShminfo->hMutex))
    {
        WT_Error("WBS_Shm_Destroy: CloseHandle2 error\n");
    }

    free(pShminfo);

    return TRUE;
}

void* WBS_Shm_GetData(HANDLE hShm)
{
  	SHMINFO_T* pShminfo;

    pShminfo = (SHMINFO_T*)hShm;

    return pShminfo->pData;
}

void WBS_Shm_GetDataT(HANDLE hShm, PVOID pData)
{
  	SHMINFO_T* pShminfo;

    pShminfo = (SHMINFO_T*)hShm;

    WBS_Shm_Lock(hShm);
    memcpy(pData, pShminfo->pData, pShminfo->nDataSize);
    WBS_Shm_Unlock(hShm);
}

void* WBS_Shm_GetDataEx(HANDLE hShm, int offset)
{
  	SHMINFO_T* pShminfo;

    pShminfo = (SHMINFO_T*)hShm;

    return ((PBYTE)pShminfo->pData + offset);
}

void WBS_Shm_GetDataExT(HANDLE hShm, PVOID pData, int nDataLen, int offset)
{
    SHMINFO_T* pShminfo;
    
    pShminfo = (SHMINFO_T*)hShm;
    
    WBS_Shm_Lock(hShm);
    memcpy(pData, (PBYTE)pShminfo->pData + offset, min(nDataLen, pShminfo->nDataSize - offset));
    WBS_Shm_Unlock(hShm);
}

BOOL WBS_Shm_SetData(HANDLE hShm, void *pData)
{
  	SHMINFO_T* pShminfo;

    pShminfo = (SHMINFO_T*)hShm;

    WBS_Shm_Lock(hShm);
    memcpy(pShminfo->pData, pData, pShminfo->nDataSize);
    WBS_Shm_Unlock(hShm);

    return TRUE;
}

BOOL WBS_Shm_SetDataEx(HANDLE hShm, void *pData, int offset, int size)
{
  	SHMINFO_T* pShminfo;

    pShminfo = (SHMINFO_T*)hShm;

    WBS_Shm_Lock(hShm);
    memcpy((PBYTE)pShminfo->pData + offset, pData, min(size, pShminfo->nDataSize));
    WBS_Shm_Unlock(hShm);

    return TRUE;
}

BOOL WBS_Shm_Lock(HANDLE hShm)
{
  	SHMINFO_T* pShminfo;

    pShminfo = (SHMINFO_T*)hShm;
    if (WaitForSingleObject(pShminfo->hMutex, INFINITE) == WAIT_OBJECT_0)
		return TRUE;

	return FALSE;
}

BOOL WBS_Shm_Unlock(HANDLE hShm)
{
  	SHMINFO_T* pShminfo;

    pShminfo = (SHMINFO_T*)hShm;
	return ReleaseMutex(pShminfo->hMutex);
}
