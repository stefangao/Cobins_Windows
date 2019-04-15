// Mp3Player.h: interface for the CMusicPlayer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MP3PLAYER_H__F9EAF4B4_527F_48D7_BE64_816F0C43469D__INCLUDED_)
#define AFX_MP3PLAYER_H__F9EAF4B4_527F_48D7_BE64_816F0C43469D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include  "vfw.h"

class CMusicPlayer  
{
    MCIDEVICEID m_wDeviceID;

public:
	CMusicPlayer();
	virtual ~CMusicPlayer();

public:
    BOOL Play(LPCSTR strPath);
    BOOL Pause();
    BOOL Stop();

	DWORD GetLength();
	DWORD GetPosition();
    DWORD GetPlayStatus();


};

#endif // !defined(AFX_MP3PLAYER_H__F9EAF4B4_527F_48D7_BE64_816F0C43469D__INCLUDED_)
