//============================================================================
// Name        : llConst.h
// Author      : Stefan Gao (stefan.gao@gmail.com)
// Version     :
// Copyright   : Your copyright notice
// Description :
//============================================================================

#ifndef __COB_CONST_H__
#define __COB_CONST_H__

#include <windows.h>

static const int WM_COBMSG_MIN = (WM_USER + 0x300);
static const int WM_COBMSG_MAX = (WM_USER + 0x350);

static const int WM_UI_WAIT = (WM_COBMSG_MIN + 1);


static const int WM_GMBS_INIT = (WM_COBMSG_MIN + 1);

static const int WM_GMBS_TIMER = (WM_COBMSG_MIN + 3);

static const int WM_GMBS_RPCSEND = (WM_COBMSG_MIN + 5);
static const int WM_GMBS_EXIT = (WM_COBMSG_MIN + 6);
static const int WM_GMBS_RPCANSWER = (WM_COBMSG_MIN + 7);

static const int WM_GMBS_RPCPOST = (WM_COBMSG_MIN + 8);

#endif
