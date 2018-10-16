#include "stdafx.h"
#include "CHookPlayerInstance.h"
#include "CicleBuffer.h"

CHookPlayerInstance* CHookPlayerInstance::m_lpHookPlayer = NULL;
CHookPlayerInstance::CGraba CHookPlayerInstance::m_Graba;

CHookPlayerInstance* CHookPlayerInstance::getHookInstance()
{
	if (nullptr == m_lpHookPlayer) {

		m_lpHookPlayer = new CHookPlayerInstance;
	}

	return m_lpHookPlayer;
}

void CHookPlayerInstance::releaseHookInstance()
{
	if (m_lpHookPlayer) {

		delete m_lpHookPlayer; 
		m_lpHookPlayer = nullptr;
	}
}

CHookPlayerInstance::CHookPlayerInstance():
m_lpPlayHooker(nullptr)
{
	CAgoraFormatStr::AgoraOutDebugStr(_T(__FUNCTION__));
	m_lpPlayHooker = createPlayerHookerInstance();
	
	m_lpHookAudioCicleBuffer = new CicleBuffer(48000 * 2 * 2, 0);
	m_lpHookAudioCicleBuffer->flushBuffer();
}

CHookPlayerInstance::~CHookPlayerInstance()
{
	CAgoraFormatStr::AgoraOutDebugStr(_T(__FUNCTION__));
	if (m_lpPlayHooker)
		destoryPlayerHookerInstance(m_lpPlayHooker);
	m_lpPlayHooker = nullptr;

	if (m_lpHookAudioCicleBuffer)
		delete m_lpHookAudioCicleBuffer;
	m_lpHookAudioCicleBuffer = nullptr;
}

int CHookPlayerInstance::startHook(TCHAR* playerPath)
{
	CAgoraFormatStr::AgoraOutDebugStr(_T(__FUNCTION__));

	if (m_lpPlayHooker) {

		m_lpPlayHooker->startHook(playerPath);
		return startAudioCapture();
	}
}

void CHookPlayerInstance::stopHook()
{
	CAgoraFormatStr::AgoraOutDebugStr(_T(__FUNCTION__));

	if (m_lpPlayHooker) {
		
		stopAudioCapture();
		return m_lpPlayHooker->stopHook();
	}
}

void CHookPlayerInstance::pushAudioBuffer(unsigned char* bufferIn, unsigned int bufferInLen)
{
	m_lpHookAudioCicleBuffer->writeBuffer(bufferIn, bufferInLen);
}

BOOL CHookPlayerInstance::popAudioBuffer(unsigned char* bufferOut, unsigned int bufferToReadLen,unsigned int bufferOutLen)
{
	return m_lpHookAudioCicleBuffer->readBuffer(bufferOut, bufferToReadLen, &bufferOutLen);
}

int CHookPlayerInstance::startAudioCapture()
{
	CAgoraFormatStr::AgoraOutDebugStr(_T(__FUNCTION__));

	if (m_lpPlayHooker)
		return m_lpPlayHooker->startAudioCapture(this);
}

void CHookPlayerInstance::stopAudioCapture()
{
	CAgoraFormatStr::AgoraOutDebugStr(_T(__FUNCTION__));

	if (m_lpPlayHooker)
		return m_lpPlayHooker->stopAudioCapture();
}

void CHookPlayerInstance::onCaptureStart()
{
	CAgoraFormatStr::AgoraOutDebugStr(_T(__FUNCTION__));
}

void CHookPlayerInstance::onCaptureStop()
{
	CAgoraFormatStr::AgoraOutDebugStr(_T(__FUNCTION__));
}

void CHookPlayerInstance::onCapturedData(void* data, UINT dataLen, WAVEFORMATEX* format)
{
	//CAgoraFormatStr::AgoraOutDebugStr(_T(__FUNCTION__));

	//pushAudioBuffer((unsigned char*)data, dataLen);
	m_lpHookAudioCicleBuffer->writeBuffer(data, dataLen);
}