#pragma once

#include "CicleBuffer.h"
class CicleBuffer;
class CHookPlayerInstance :public IAudioCaptureCallback
{
public:
	static CHookPlayerInstance* getHookInstance();
	static void releaseHookInstance();

public:
	int startHook(TCHAR* playerPath);
	void stopHook();

	void pushAudioBuffer(unsigned char* bufferIn, unsigned int bufferInLen);
	BOOL popAudioBuffer(unsigned char* bufferOut,unsigned int bufferToReadLen,unsigned int bufferOutLen);
	CicleBuffer* getCicleBuffer(){ return m_lpHookAudioCicleBuffer; }

protected:
	int startAudioCapture();
	void stopAudioCapture();

protected:
	CHookPlayerInstance();
	~CHookPlayerInstance();

	virtual void onCaptureStart();
	virtual void onCaptureStop();
	virtual void onCapturedData(void* data, UINT dataLen, WAVEFORMATEX* format);

	struct CGraba{
		CGraba(){ ; }
		~CGraba(){
			releaseHookInstance();
		}
	};

public:
	static CHookPlayerInstance* m_lpHookPlayer;
	static CGraba m_Graba;

	IPlayerHooker* m_lpPlayHooker;
	CicleBuffer* m_lpHookAudioCicleBuffer;
};