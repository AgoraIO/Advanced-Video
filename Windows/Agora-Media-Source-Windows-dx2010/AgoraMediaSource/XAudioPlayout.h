#pragma once


#include <atlbase.h>
#include <atlcoll.h>

#include "IAGDShowDevice.h"


// xaudio 2.7 only!!!!!
#include "xaudio2.h"

// #pragma comment(lib, "Xaudio2")

class CXAudioPlayout
{
public:
	CXAudioPlayout();
	~CXAudioPlayout();

	BOOL Create();
	void Close();

	BOOL EnumDeviceList();
	int  GetDeviceCount() { return m_listDeviceInfo.GetCount(); };
	BOOL GetDeviceInfo(int nIndex, LPAGORA_DEVICE_INFO lpDeviceInfo);

	BOOL OpenDevice(int nIndex);
	BOOL OpenDevice(LPCTSTR lpDevicePath);
	BOOL GetCurrentDevice(LPTSTR lpDevicePath, SIZE_T *nDevicePathLen);

	void CloseDevice();

	BOOL PlayoutControl(UINT nControlCode = DEVICE_START);

	BOOL SetAudioFormat(WAVEFORMATEX *lpWaveFormat, IXAudio2VoiceCallback *lpVoiceCallback);

	BOOL SetAudioSampleRate(DWORD dwSamplesPerSec);
	DWORD GetAudioSamplerate();

	BOOL PushAudioData(LPCVOID lpData, SIZE_T nDataSize);

	IXAudio2SourceVoice *GetSourceVoicePtr();
private:
	TCHAR						m_szActiveDeviceID[MAX_PATH];
	CAtlList<AGORA_DEVICE_INFO> m_listDeviceInfo;
	WAVEFORMATEX				m_wavFormat;

private:
	CComPtr<IXAudio2>			m_ptrXAudio2;
	IXAudio2MasteringVoice		*m_ptrXAudio2MasterVoice;
	IXAudio2SourceVoice			*m_ptrXAudio2SourceVoice;
};

