#include "stdafx.h"
#include "ExtendVideoFrameObserver.h"


VIDEO_BUFFER		buffer;
CExtendVideoFrameObserver::CExtendVideoFrameObserver()
{
	m_lpImageBuffer = new BYTE[VIDEO_BUF_SIZE];
}


CExtendVideoFrameObserver::~CExtendVideoFrameObserver()
{
    if (m_lpImageBuffer) {
        delete[] m_lpImageBuffer;
        m_lpImageBuffer = nullptr;
    }
}

bool CExtendVideoFrameObserver::onCaptureVideoFrame(VideoFrame& videoFrame)
{
    int bufSize = videoFrame.width*videoFrame.height * 3 / 2;
    
    int timestamp = GetTickCount();
    if (CAgVideoBuffer::GetInstance()->readBuffer(m_lpImageBuffer, bufSize, timestamp)) {
        memcpy_s(buffer.m_lpImageBuffer, bufSize, m_lpImageBuffer, bufSize);
        buffer.timestamp = timestamp;
    }
    else
        OutputDebugString(L"readBuffer failed");

	/*BOOL bSuccess = CVideoPackageQueue::GetInstance()->PopVideoPackage(m_lpImageBuffer, &nBufferSize);
	if (!bSuccess)
		return false;*/

	m_lpY = buffer.m_lpImageBuffer;
	m_lpU = buffer.m_lpImageBuffer + videoFrame.height*videoFrame.width;
	m_lpV = buffer.m_lpImageBuffer + 5 * videoFrame.height*videoFrame.width / 4;

	memcpy_s(videoFrame.yBuffer, videoFrame.height*videoFrame.width, m_lpY, videoFrame.height*videoFrame.width);
	videoFrame.yStride = videoFrame.width;
	
	memcpy_s(videoFrame.uBuffer, videoFrame.height*videoFrame.width / 4, m_lpU, videoFrame.height*videoFrame.width / 4);
	videoFrame.uStride = videoFrame.width/2;

	memcpy_s(videoFrame.vBuffer, videoFrame.height*videoFrame.width / 4, m_lpV, videoFrame.height*videoFrame.width / 4);
	videoFrame.vStride = videoFrame.width/2;

	videoFrame.type = FRAME_TYPE_YUV420;
	videoFrame.rotation = 0;

	return true;
}

bool CExtendVideoFrameObserver::onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame)
{
	return true;
}
