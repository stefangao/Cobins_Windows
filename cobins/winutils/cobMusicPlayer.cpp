#include <windows.h>
#include "cobMusicPlayer.h"
#include  "vfw.h."

NS_COB_BEGIN

#pragma comment(lib, "winmm.lib")

MusicPlayer::MusicPlayer()
{
    m_wDeviceID = -1;
}

MusicPlayer::~MusicPlayer()
{
    
}

static void CALLBACK TimeOutProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
 
}

BOOL MusicPlayer::Play(LPCSTR strPath)
{
    MCI_OPEN_PARMS mciopenparms;
    MCI_PLAY_PARMS mciplayparms;

    if (m_wDeviceID != -1)
    {
        Stop();
        m_wDeviceID = -1;
    }

    mciopenparms.lpstrElementName = strPath;
    mciopenparms.lpstrDeviceType=NULL;
    mciSendCommand(0, MCI_OPEN, MCI_DEVTYPE_WAVEFORM_AUDIO, (DWORD)(LPVOID)&mciopenparms);
    m_wDeviceID = mciopenparms.wDeviceID;

    long cdlen = GetLength();                                                                                                                                                                                                                                            
    long cdfrom = MCI_MAKE_HMS(0,0,0);
    long cdto = MCI_MAKE_HMS(MCI_HMS_HOUR(cdlen),MCI_HMS_MINUTE(cdlen),MCI_HMS_SECOND(cdlen));  
    mciplayparms.dwFrom = cdfrom;
    mciplayparms.dwTo = cdto;
    mciSendCommand(mciopenparms.wDeviceID,MCI_PLAY,MCI_TO|MCI_FROM,(DWORD)(LPVOID)& mciplayparms);
    
    return TRUE;
}

BOOL MusicPlayer::Pause()
{
    if (mciSendCommand(m_wDeviceID, MCI_PAUSE, 0, NULL) == 0)
        return TRUE;

    return FALSE;
}

BOOL MusicPlayer::Stop()
{
    if (mciSendCommand(m_wDeviceID, MCI_CLOSE, 0, NULL) == 0)
        return TRUE;
    
    return FALSE;
}

DWORD MusicPlayer::GetLength()
{
    MCI_STATUS_PARMS mcistatusparms;
	mcistatusparms.dwCallback = (DWORD)NULL;
	mcistatusparms.dwItem = MCI_STATUS_LENGTH;
	mcistatusparms.dwReturn = 0;
	mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mcistatusparms);

    return mcistatusparms.dwReturn;
}

DWORD MusicPlayer::GetPosition()
{
    MCI_STATUS_PARMS mcistatusparms;
	mcistatusparms.dwCallback = (DWORD)NULL;
	mcistatusparms.dwItem = MCI_STATUS_POSITION;
	mcistatusparms.dwReturn = 0;
	mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mcistatusparms);

    return mcistatusparms.dwReturn;
}

// GetPlayStatusµÄ·µ»ØÖµ :
// MCI_MODE_NOT_READY
// MCI_MODE_PAUSE
// MCI_MODE_PLAY
// MCI_MODE_STOP
// MCI_MODE_OPEN
// MCI_MODE_RECORD
// MCI_MODE_SEEK
DWORD MusicPlayer::GetPlayStatus( )
{
    if (m_wDeviceID == -1)
        return MCI_MODE_NOT_READY;

    MCI_STATUS_PARMS mcistatusparms;
	mcistatusparms.dwCallback = (DWORD)NULL;
	mcistatusparms.dwItem = MCI_STATUS_MODE;
	mcistatusparms.dwReturn = 0;
	mciSendCommand(m_wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mcistatusparms);

    return mcistatusparms.dwReturn;
}

NS_COB_END