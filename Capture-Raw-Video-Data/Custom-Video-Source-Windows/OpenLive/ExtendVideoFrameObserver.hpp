#pragma once
#include "IAgoraMediaEngine.h"
#include <windows.h>
#include "CircularBuffer.hpp"
class CExtendVideoFrameObserver :
	public agora::media::IVideoFrameObserver
{
public:
	CExtendVideoFrameObserver();
	~CExtendVideoFrameObserver();

	virtual bool onCaptureVideoFrame(VideoFrame& videoFrame);
	virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame);

	void setVideoResolution(int w, int h);
	unsigned char*				m_lpImageBuffer;

 bool saveLog;

 CircularBuffer videoBuffer;
private:
	LPBYTE				m_lpY;
	LPBYTE				m_lpU;
	LPBYTE				m_lpV;

	int                m_resolutionX;
	int                m_resolutionY;
};

