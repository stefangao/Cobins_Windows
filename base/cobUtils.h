//============================================================================
// Name        : llUtils.h
// Author      : Stefan Gao (stefan.gao@gmail.com)
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#ifndef __COB_UTILS_H__
#define __COB_UTILS_H__

#include <string>
#include "cobMacros.h"

NS_COB_BEGIN

class Utils
{
public:
    static void __log(const char* format,...);
    static void __assert(const char* file, int lineno, const char* format,...);
};

extern const char* LL_STRING_EMPTY;

#ifdef COB_DEBUG
    #define COBASSERT(x, format, ...)\
        if (!(x))\
            Utils::__assert(__FILE__, __LINE__, format, ##__VA_ARGS__)
#else
    #define COBASSERT(x, msg)
#endif

#ifdef COB_DEBUG
    #define COBLOG(format,...) Utils::__log(format, ##__VA_ARGS__)
#else
    #define COBLOG(format)
#endif

NS_COB_END

#endif
