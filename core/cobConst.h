//============================================================================
// Name        : llConst.h
// Author      : Stefan Gao (stefan.gao@gmail.com)
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#ifndef __COB_CONST_H__
#define __COB_CONST_H__

#include <vector>
#include <queue>
#include <string>

#ifdef __cplusplus
    #define NS_COB_BEGIN                     namespace cobins {
    #define NS_COB_END                       }
    #define USING_NS_COB                     using namespace cobins
#else
    #define NS_COB_BEGIN
    #define NS_COB_END
    #define USING_NS_COB
#endif

NS_COB_BEGIN

#define COB_DEBUG

NS_COB_END

#endif
