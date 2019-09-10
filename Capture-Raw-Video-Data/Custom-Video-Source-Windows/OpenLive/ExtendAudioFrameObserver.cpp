#include "stdafx.h"
#include "ExtendAudioFrameObserver.hpp"

#include <tchar.h>
#include "AudioCapturePackageQueue.hpp"


CExtendAudioFrameObserver::CExtendAudioFrameObserver()
{
	pPlayerData = new BYTE[0x800000];
	saveLog = true;
}

CExtendAudioFrameObserver::~CExtendAudioFrameObserver()
{
    if (pPlayerData) {
        delete[] pPlayerData;
        pPlayerData = NULL; 
    }
}

bool CExtendAudioFrameObserver::onRecordAudioFrame(AudioFrame& audioFrame)
{
	if (!pPlayerData)
		return false;
	
	SIZE_T nSize = audioFrame.channels*audioFrame.samples * audioFrame.bytesPerSample;
	unsigned int datalen = 0;

 

 CAudioCapturePackageQueue::GetInstance()->PopAudioPackage(pPlayerData, &nSize);


	if (nSize > 0 && datalen == nSize)
	{
		memcpy_s(audioFrame.buffer, datalen, pPlayerData, datalen);
	}
	
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame)
{
	SIZE_T nSize = audioFrame.channels*audioFrame.samples * 2;
	
	return true;
}

bool CExtendAudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame)
{
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame)
{
	return true;
}
