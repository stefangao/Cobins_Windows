#ifndef __WB_SHM_H__
#define	__WB_SHM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

/** @addtogroup groupwndbase
 *  @{
 */
HANDLE      WBS_Shm_Create(const char *lpShmName, int nSize);
BOOL        WBS_Shm_Destroy(HANDLE hShm);
void*       WBS_Shm_GetData(HANDLE hShm);
void*       WBS_Shm_GetDataEx(HANDLE hShm, int offset);
BOOL        WBS_Shm_SetData(HANDLE hShm, void *pData);
BOOL        WBS_Shm_SetDataEx(HANDLE hShm, void *pData, int offset, int size);
BOOL        WBS_Shm_Lock(HANDLE hShm);
BOOL        WBS_Shm_Unlock(HANDLE hShm);

/** @} */ // end of groupwndbase

#ifdef __cplusplus
}
#endif
#endif