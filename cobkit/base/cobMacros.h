//============================================================================
// Name        : llConst.h
// Author      : Stefan Gao (stefan.gao@gmail.com)
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#ifndef __COB_MACROS_H__
#define __COB_MACROS_H__

#ifdef __cplusplus
    #define NS_COB_BEGIN                     namespace cobins {
    #define NS_COB_END                       }
    #define USING_NS_COB                     using namespace cobins
#else
    #define NS_COB_BEGIN
    #define NS_COB_END
    #define USING_NS_COB
#endif

#define COB_DEBUG
#define WT_TRACE

#define COB_SAFE_DELETE(p)    do { delete (p); (p) = nullptr; } while(0)

#endif
