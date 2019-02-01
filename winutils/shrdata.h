#ifndef __SHRDATA_H__
#define __SHRDATA_H__

#include <afxtempl.h>
#include "cntt_typedef.h"
#include "cntt_list.h"

#define WNDBASE_API    //__declspec(dllexport)

class WNDBASE_API CShrData
{
#define NODENAME_MAXLEN    64
    
    typedef enum
    {
        NTYPE_HASH,
        NTYPE_SCALAR,
        NTYPE_SEQUENCE,            
    } NodeType_t;
    
    typedef struct _DataNote_t
    {
        char         name[NODENAME_MAXLEN+1];
        NodeType_t   type;
        _DataNote_t *parent;
        cntt_list_t  childlist;

        CNTT_HANDLE  shmhandle;
        PVOID        pdata;
        int          ndatalen;
        
        cntt_list_t  list;  //与兄弟节点链接
    } NodeInfo_t;
    
private:
    CMap<CString, LPCTSTR, NodeInfo_t*, NodeInfo_t*> m_NodeMap;
    NodeInfo_t *m_pRootNode;
    
public:
    CShrData( );
   	virtual ~CShrData( );
    
public:
    BOOL   Open(LPCSTR xmlFile);
    void   Close( );
    
    int    GetData(LPCSTR lpDataUri, PBYTE pData, int nLen);
    int    SetData(LPCSTR lpDataUri, PBYTE pData, int nLen);

    BOOL   GetUri(NodeInfo_t *pNode, CString &strNodeUri);

private:
    BOOL   LoadNode(void *xmlNode, NodeInfo_t *pParent);
    BOOL   UnloadNode(NodeInfo_t *pNode);
    BOOL   AppendUri(NodeInfo_t *pCurNode, CString &strNodeUri);
    BOOL   SaveNode(LPCSTR xmlpath, LPCSTR lpDataUri, PBYTE pData, int nLen);
   
};

#endif
