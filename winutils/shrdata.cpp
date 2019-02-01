#include "cntt_util.h"
#include "GmbsUtil.h"
#include "shrdata.h"
#include "wndshm.H"
#include "wtermin.h"
#include "wndbase.h"

LPCSTR _afxPchNil = "";

CShrData::CShrData( ) 
{
    m_pRootNode = NULL;    
}

CShrData::~CShrData( )
{
    
}

BOOL CShrData::Open(LPCSTR xmlFile)
{
	xmlDocPtr   doc; 
	xmlNodePtr  curitem;

    if (m_pRootNode != NULL)
    {
        Close();
        m_pRootNode = NULL;
    }

	doc = xmlParseFile(xmlFile); 
	if (doc == NULL ) 
	{ 
		WT_Trace("Document not parsed successfully. \n"); 
		return FALSE; 
	} 

	curitem = xmlDocGetRootElement(doc); 
	if (curitem == NULL || xmlStrcmp(curitem->name, (xmlChar*)"ShareData")) 
	{ 
		WT_Trace("empty document or wrong root\n"); 
		xmlFreeDoc(doc); 
		return FALSE; 
    }

    return LoadNode(curitem, NULL);
}

void CShrData::Close( )
{
    m_NodeMap.RemoveAll();

    if (m_pRootNode != NULL)
        UnloadNode(m_pRootNode);
}

int CShrData::GetData(LPCSTR lpDataUri, PBYTE pData, int nLen)
{
    NodeInfo_t *pNodeInfo;
    int nDataLen = 0;

    ASSERT(lpDataUri != NULL);
    ASSERT(pData != NULL);

    if (!m_NodeMap.Lookup(lpDataUri, pNodeInfo))
        return -1;

    if (pNodeInfo->shmhandle == NULL || pNodeInfo->pdata == NULL)
        return -1;

    WBS_Shm_Lock(pNodeInfo->shmhandle);
    nDataLen = min(nLen, pNodeInfo->ndatalen);
    memcpy(pData, pNodeInfo->pdata, nDataLen);
    WBS_Shm_Unlock(pNodeInfo->shmhandle);

    return nDataLen;
}

int CShrData::SetData(LPCSTR lpDataUri, PBYTE pData, int nLen)
{
    NodeInfo_t *pNodeInfo;
    int nDataLen = 0;

    ASSERT(lpDataUri != NULL);
    ASSERT(pData != NULL);

    if (!m_NodeMap.Lookup(lpDataUri, pNodeInfo))
        return -1;

    if (pNodeInfo->shmhandle == NULL || pNodeInfo->pdata == NULL)
        return -1;

    WBS_Shm_Lock(pNodeInfo->shmhandle);
    nDataLen = min(nLen, pNodeInfo->ndatalen);
    memcpy(pNodeInfo->pdata, pData, nDataLen);
    WBS_Shm_Unlock(pNodeInfo->shmhandle);

    return nDataLen;
}

BOOL CShrData::GetUri(NodeInfo_t *pNode, CString &strNodeUri)
{
    ASSERT(pNode != NULL);

    strNodeUri.Empty();
    strNodeUri.Insert(0, pNode->name);

    return AppendUri(pNode->parent, strNodeUri);
}

BOOL CShrData::AppendUri(NodeInfo_t *pCurNode, CString &strNodeUri)
{
    if (pCurNode != NULL)
    {
        strNodeUri.Insert(0, "/");
        strNodeUri.Insert(0, pCurNode->name);
    }

    if (pCurNode->parent == NULL)
    {
        strNodeUri.Insert(0, "/");
        return TRUE;
    }

    return AppendUri(pCurNode->parent, strNodeUri);
}

BOOL CShrData::LoadNode(void *xmlNode, NodeInfo_t *pParent)
{
    xmlChar *xType, *xSize, *xInit;
    NodeInfo_t *pNodeInfo;
    xmlNodePtr curitem;
    
    if (xmlNode == NULL)
        return FALSE;

    pNodeInfo = (NodeInfo_t*)cntt_malloc(sizeof(NodeInfo_t));
    if (pNodeInfo == NULL)
    {
        WT_Trace("CShrData::LoadNode: cntt_malloc error\n");
        return FALSE;
    }

    curitem = (xmlNodePtr)xmlNode;
    strncpy(pNodeInfo->name, (char*)curitem->name, NODENAME_MAXLEN);
    pNodeInfo->parent = pParent;
    INIT_CNTT_LIST_HEAD(&pNodeInfo->childlist);
    pNodeInfo->shmhandle = NULL;
    pNodeInfo->pdata = NULL;
    if (pParent == NULL)
    {
        m_pRootNode = pNodeInfo;  //保留根节点引用
    }

    xType = xmlGetProp(curitem, (xmlChar*)"type");
    if (xType != NULL && xmlStrcmp(xType, (xmlChar*)"folder") != 0)
    {
        pNodeInfo->type = NTYPE_SCALAR;
        xType = xmlGetProp(curitem, (xmlChar*)"type");
        xSize = xmlGetProp(curitem, (xmlChar*)"size");
        xInit = xmlGetProp(curitem, (xmlChar*)"init");
        if (xmlStrcmp(xType, (xmlChar*)"value") == 0)
        {
            pNodeInfo->ndatalen = atoi((LPCSTR)xSize);
            if (pNodeInfo->ndatalen > 0)
            {
                pNodeInfo->shmhandle = WBS_Shm_Create((LPCSTR)xmlGetNodePath((xmlNodePtr)curitem), pNodeInfo->ndatalen);
                if (pNodeInfo->shmhandle == NULL)
                {
                    cntt_free(pNodeInfo);
                    WT_Trace("CShrData::LoadNode: WBS_Shm_Create error\n");
                    return FALSE;
                }
                
                pNodeInfo->pdata = WBS_Shm_GetData(pNodeInfo->shmhandle);
                int value = atoi((LPCSTR)xInit);
                memcpy(pNodeInfo->pdata, &value, pNodeInfo->ndatalen);
            }
        }
        else if (xmlStrcmp(xType, (xmlChar*)"string") == 0)
        {
            pNodeInfo->ndatalen = atoi((LPCSTR)xSize);
            if (pNodeInfo->ndatalen > 0)
            {
                pNodeInfo->shmhandle = WBS_Shm_Create((LPCSTR)xmlGetNodePath((xmlNodePtr)curitem), pNodeInfo->ndatalen);
                if (pNodeInfo->shmhandle == NULL)
                {
                    cntt_free(pNodeInfo);
                    WT_Trace("CShrData::LoadNode: WBS_Shm_Create error\n");
                    return FALSE;
                }
                
                pNodeInfo->pdata = WBS_Shm_GetData(pNodeInfo->shmhandle);
                strncpy((char*)pNodeInfo->pdata, (char*)xInit, pNodeInfo->ndatalen);
            }
        }
        else if (xmlStrcmp(xType, (xmlChar*)"stream") == 0)
        {



        }

        m_NodeMap.SetAt((LPCSTR)xmlGetNodePath((xmlNodePtr)curitem), pNodeInfo);
        if (pParent != NULL)
            cntt_list_insert_before(&pNodeInfo->list, &pParent->childlist);

    }
    else
    {
        pNodeInfo->type = NTYPE_HASH;
        m_NodeMap.SetAt((LPCSTR)xmlGetNodePath((xmlNodePtr)curitem), pNodeInfo);
        if (pParent != NULL)
            cntt_list_insert_before(&pNodeInfo->list, &pParent->childlist);

        curitem = curitem->xmlChildrenNode;
        while (curitem)
        {
            if (xmlStrcmp(curitem->name, (xmlChar*)"text") && xmlStrcmp(curitem->name, (xmlChar*)"comments"))
            {
                LoadNode(curitem, pNodeInfo);
            }
            
            curitem = curitem->next;
        }
    }
    
    return TRUE;    
}

BOOL CShrData::UnloadNode(NodeInfo_t *pNode)
{
    cntt_list_t *curchild;
    NodeInfo_t  *pChildNode;
    
    if (pNode == NULL)
        return FALSE;

    CNTT_LIST_FOREACH(curchild, &pNode->childlist)
    {
        cntt_list_t _copynode = *curchild;
        pChildNode = CNTT_LIST_ENTRY(curchild, NodeInfo_t, list);
        if (pChildNode != NULL)
        {
            UnloadNode(pChildNode);
        }
        curchild = &_copynode;
    }

    if (pNode->shmhandle != NULL)
        WBS_Shm_Destroy(pNode->shmhandle);

    cntt_free(pNode);
    
    return TRUE;    
}

BOOL CShrData::SaveNode(LPCSTR xmlpath, LPCSTR lpDataUri, PBYTE pData, int nLen)
{
    xmlDocPtr doc; 
    xmlNodePtr node;
    xmlNodeSetPtr nodeset;
    xmlXPathObjectPtr pNodeset;
    
    doc = xmlParseFile(xmlpath); 
    if (doc == NULL ) 
    { 
        WT_Trace("CShrData::SaveNode: failed to parse the document %s\n", xmlpath); 
        return FALSE; 
    } 
    
    pNodeset = WBS_GetXPathNodeset(doc, (xmlChar*)lpDataUri);
    if (pNodeset != NULL) 
    {
        nodeset = pNodeset->nodesetval;
        if (nodeset->nodeNr == 1)
        {
            node = nodeset->nodeTab[0];
            xmlSetProp(node, (const xmlChar*)"init", (const xmlChar *)"2012");
        }
    }

    if (xmlSaveFile(xmlpath, doc) == -1)
    {
        WT_Trace("保存文件失败\n");
    }
    
    xmlXPathFreeObject(pNodeset);
    xmlFreeDoc(doc);

    return TRUE;
}