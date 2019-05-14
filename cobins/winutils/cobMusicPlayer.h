#ifndef __MUSIC_PLAYER__
#define __MUSIC_PLAYER__

#include  "vfw.h"
#include "cobMacros.h"

NS_COB_BEGIN

class MusicPlayer  
{
    MCIDEVICEID m_wDeviceID;

public:
	MusicPlayer();
	virtual ~MusicPlayer();

public:
    BOOL Play(LPCSTR strPath);
    BOOL Pause();
    BOOL Stop();

	DWORD GetLength();
	DWORD GetPosition();
    DWORD GetPlayStatus();


};

NS_COB_END

#endif
