#include "stdafx.h"
#include "XAudioPlayout.h"


CXAudioPlayout::CXAudioPlayout()
	: m_ptrXAudio2(NULL)
	, m_ptrXAudio2MasterVoice(NULL)
	, m_ptrXAudio2SourceVoice(NULL)
{
	memset(m_szActiveDeviceID, 0, MAX_PATH*sizeof(TCHAR));
}


CXAudioPlayout::~CXAudioPlayout()
{
}

BOOL CXAudioPlayout::Create()
{
	HRESULT hResult = XAudio2Create(&m_ptrXAudio2, 0);

	return hResult == S_OK ? TRUE : FALSE;
}

void CXAudioPlayout::Close()
{
	m_ptrXAudio2 = NULL;
}

BOOL CXAudioPlayout::EnumDeviceList()
{
	UINT nDeviceCount = 0;
	AGORA_DEVICE_INFO		agDeviceInfo;
	XAUDIO2_DEVICE_DETAILS	xaDeviceDetail;

	m_listDeviceInfo.RemoveAll();

	HRESULT hResult = m_ptrXAudio2->GetDeviceCount(&nDeviceCount);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	for (int nIndex = 0; nIndex < static_cast<int>(nDeviceCount); nIndex++) {
		
		memset(&xaDeviceDetail, 0, sizeof(XAUDIO2_DEVICE_DETAILS));
		memset(&agDeviceInfo, 0, sizeof(AGORA_DEVICE_INFO));
		m_ptrXAudio2->GetDeviceDetails(nIndex, &xaDeviceDetail);

#ifdef UNICODE
		_tcscpy_s(agDeviceInfo.szDevicePath, MAX_PATH, xaDeviceDetail.DeviceID);
		_tcscpy_s(agDeviceInfo.szDeviceName, MAX_PATH, xaDeviceDetail.DisplayName);
#else
		::WideCharToMultiByte(CP_ACP, 0, xaDeviceDetail.DeviceID, -1, agDeviceInfo.szDevicePath, MAX_PATH, NULL, NULL);
		::WideCharToMultiByte(CP_ACP, 0, xaDeviceDetail.DisplayName, -1, agDeviceInfo.szDeviceName, MAX_PATH, NULL, NULL);
#endif

		m_listDeviceInfo.AddTail(agDeviceInfo);
	}

	return TRUE;
}

BOOL CXAudioPlayout::GetDeviceInfo(int nIndex, LPAGORA_DEVICE_INFO lpDeviceInfo)
{
	ATLASSERT(lpDeviceInfo != NULL);
	ATLASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_listDeviceInfo.GetCount()));

	POSITION pos = m_listDeviceInfo.FindIndex(nIndex);
	if (pos == NULL)
		return FALSE;

	AGORA_DEVICE_INFO &agDeviceInfo = m_listDeviceInfo.GetAt(pos);
	memcpy_s(lpDeviceInfo, sizeof(AGORA_DEVICE_INFO), &agDeviceInfo, sizeof(AGORA_DEVICE_INFO));

	return TRUE;
}

BOOL CXAudioPlayout::OpenDevice(int nIndex)
{
	ATLASSERT(m_ptrXAudio2 != NULL);
	if (m_ptrXAudio2 == NULL)
		return FALSE;
	
	HRESULT hResult = S_OK;
	XAUDIO2_DEVICE_DETAILS xAudioDeviceDtails;
	
	hResult = m_ptrXAudio2->GetDeviceDetails(nIndex, &xAudioDeviceDtails);
	ATLASSERT(SUCCEEDED(hResult));

#ifdef UNICODE
	_tcscpy_s(m_szActiveDeviceID, MAX_PATH, xAudioDeviceDtails.DeviceID);
#else
	::WideCharToMultiByte(CP_ACP, 0, xAudioDeviceDtails.DeviceID, -1, m_szActiveDeviceID, MAX_PATH, NULL, NULL);
#endif

	hResult = m_ptrXAudio2->CreateMasteringVoice(&m_ptrXAudio2MasterVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, nIndex);
	ATLASSERT(SUCCEEDED(hResult));

	return hResult == S_OK ? TRUE : FALSE;
}

BOOL CXAudioPlayout::OpenDevice(LPCTSTR lpDevicePath)
{
	POSITION pos = m_listDeviceInfo.GetHeadPosition();
	int nIndex = 0;

	while (pos != NULL) {
		AGORA_DEVICE_INFO &agDeviceInfo = m_listDeviceInfo.GetNext(pos);

		if (_tcsicmp(agDeviceInfo.szDevicePath, lpDevicePath) == 0)
			return OpenDevice(nIndex);

		nIndex++;
	}

	return FALSE;
}

BOOL CXAudioPlayout::GetCurrentDevice(LPTSTR lpDevicePath, SIZE_T *nDevicePathLen)
{
	int nDeviceLen = _tcslen(m_szActiveDeviceID);
	if (nDeviceLen >= static_cast<int>(*nDevicePathLen)) {
		*nDevicePathLen = nDeviceLen + 1;
		return FALSE;
	}

	if (nDeviceLen == 0)
		return FALSE;

	_tcscpy_s(lpDevicePath, *nDevicePathLen, m_szActiveDeviceID);
	*nDevicePathLen = nDeviceLen + 1;

	return TRUE;
}

void CXAudioPlayout::CloseDevice()
{
	if (m_ptrXAudio2MasterVoice != NULL)
		m_ptrXAudio2SourceVoice->DestroyVoice();
	
	if (m_ptrXAudio2SourceVoice != NULL)
		m_ptrXAudio2MasterVoice->DestroyVoice();
	
	m_ptrXAudio2MasterVoice = NULL;
	m_ptrXAudio2SourceVoice = NULL;
}

BOOL CXAudioPlayout::PlayoutControl(UINT nControlCode)
{
	HRESULT hResult = S_OK;

	ATLASSERT(m_ptrXAudio2SourceVoice != NULL);
	if (m_ptrXAudio2SourceVoice == NULL)
		return FALSE;

	switch (nControlCode)
	{
	case DEVICE_START:
		hResult = m_ptrXAudio2SourceVoice->Start();
		break;
	case DEVICE_STOP:
		hResult = m_ptrXAudio2SourceVoice->Stop();
		break;
	default:
		hResult = m_ptrXAudio2SourceVoice->Start();
		break;
	}

	return hResult == S_OK ? TRUE : FALSE;
}

BOOL CXAudioPlayout::SetAudioFormat(WAVEFORMATEX *lpWaveFormat, IXAudio2VoiceCallback *lpVoiceCallback)
{
	ATLASSERT(m_ptrXAudio2 != NULL);
	ATLASSERT(m_ptrXAudio2MasterVoice != NULL);

	if (m_ptrXAudio2 == NULL || m_ptrXAudio2MasterVoice == NULL)
		return FALSE;

	HRESULT hResult = m_ptrXAudio2->CreateSourceVoice(&m_ptrXAudio2SourceVoice, lpWaveFormat, 0, 2.0, lpVoiceCallback);
	ATLASSERT(SUCCEEDED(hResult));
	
	return hResult == S_OK ? TRUE : FALSE;
}

BOOL CXAudioPlayout::SetAudioSampleRate(DWORD dwSamplesPerSec)
{
	ATLASSERT(m_ptrXAudio2SourceVoice != NULL);
	if (m_ptrXAudio2SourceVoice == NULL)
		return FALSE;

	HRESULT hResult = m_ptrXAudio2SourceVoice->SetSourceSampleRate(dwSamplesPerSec);
	ATLASSERT(SUCCEEDED(hResult));

	return hResult == S_OK ? TRUE : FALSE;
}

DWORD CXAudioPlayout::GetAudioSamplerate()
{
	ATLASSERT(m_ptrXAudio2SourceVoice != NULL);
	if (m_ptrXAudio2SourceVoice == NULL)
		return 0;

	XAUDIO2_VOICE_DETAILS xaVoiceDetails;

	m_ptrXAudio2SourceVoice->GetVoiceDetails(&xaVoiceDetails);
	
	return xaVoiceDetails.InputSampleRate;
}

BOOL CXAudioPlayout::PushAudioData(LPCVOID lpData, SIZE_T nDataSize)
{
	XAUDIO2_BUFFER xaBuffer;

	memset(&xaBuffer, 0, sizeof(XAUDIO2_BUFFER));
	xaBuffer.AudioBytes = nDataSize;
	xaBuffer.pAudioData = reinterpret_cast<const BYTE*>(lpData);

	HRESULT hResult = m_ptrXAudio2SourceVoice->SubmitSourceBuffer(&xaBuffer);

	return hResult == S_OK ? TRUE : FALSE;
}

IXAudio2SourceVoice *CXAudioPlayout::GetSourceVoicePtr()
{
	return m_ptrXAudio2SourceVoice;
}