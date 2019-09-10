#include "stdafx.h"
#include "ExtendVideoFrameObserver.hpp"
#include <TCHAR.h>
//#include <timeapi.h>
#include <stdio.h>


CExtendVideoFrameObserver::CExtendVideoFrameObserver()
    :saveLog(true)
{
	   m_lpImageBuffer = new unsigned char[0x800000];
    videoBuffer.init(10);
}


CExtendVideoFrameObserver::~CExtendVideoFrameObserver()
{
    if (m_lpImageBuffer) {
        delete[] m_lpImageBuffer;
        m_lpImageBuffer = NULL;
    }
    videoBuffer.uninit();
}

int timeinc = 0;
bool CExtendVideoFrameObserver::onCaptureVideoFrame(VideoFrame& videoFrame)
{
	if ((int*)m_lpImageBuffer == (int*)0xfeeefeee){
		m_lpImageBuffer = NULL;
		return false;
	}

	if (!m_lpImageBuffer)
		return false;


	int nSize = videoFrame.height * videoFrame.yStride * 3 / 2;;
	videoBuffer.pop((char*)m_lpImageBuffer, nSize);

	if (nSize != videoFrame.height * videoFrame.yStride * 3 / 2)
		return false;


	int nUvLen = videoFrame.height * videoFrame.yStride / 4;
	int nYLen = nUvLen * 4;

	m_lpY = m_lpImageBuffer;
	m_lpU = m_lpImageBuffer + nUvLen * 4;
	m_lpV = m_lpImageBuffer + 5 * nUvLen;

	memcpy_s(videoFrame.yBuffer, nYLen, m_lpY, nYLen);
	videoFrame.yStride = videoFrame.width;

	memcpy_s(videoFrame.uBuffer, nUvLen, m_lpU, nUvLen);
	videoFrame.uStride = videoFrame.width / 2;

	memcpy_s(videoFrame.vBuffer, nUvLen, m_lpV, nUvLen);
	videoFrame.vStride = videoFrame.width / 2;

	videoFrame.type = FRAME_TYPE_YUV420;
	videoFrame.rotation = 0;
	videoFrame.renderTimeMs = GetTickCount();
	return true;
}

bool CExtendVideoFrameObserver::onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame)
{
	return true;
}

void CExtendVideoFrameObserver::setVideoResolution(int w, int h)
{
	m_resolutionX = w;
	m_resolutionY = h;
}