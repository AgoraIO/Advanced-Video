#pragma once
#include <atlbase.h>
#include <atlcoll.h>

#include <DShow.h>


#include "IAGDShowDevice.h"

class CAGDShowAudioCapture
	: public IDShowCaptureDevice
{
public:

	CAGDShowAudioCapture();
	~CAGDShowAudioCapture();

	virtual BOOL Create();
	virtual void Close();

	virtual BOOL EnumDeviceList();
	virtual int  GetDeviceCount(){ return m_listDeviceInfo.GetCount(); };
	virtual BOOL GetDeviceInfo(int nIndex, LPAGORA_DEVICE_INFO lpDeviceInfo);

	virtual BOOL OpenDevice(int nIndex);
	virtual BOOL OpenDevice(LPCTSTR lpDevicePath);
	virtual BOOL GetCurrentDevice(LPTSTR lpDevicePath, SIZE_T *nDevicePathLen);
	virtual void CloseDevice();

	virtual int GetMediaCapCount();
	virtual BOOL GetMediaCap(int nIndex, AM_MEDIA_TYPE **ppMediaType, LPVOID lpMediaStreamConfigCaps, SIZE_T nSize);
	virtual BOOL SelectMediaCap(int nIndex);
	virtual int GetSelectedMediaCap() { return m_nCapSelected; };

	virtual BOOL GetCaptureBuffer(SIZE_T *nBlockSize, SIZE_T *nBlockCount, SIZE_T *nAlign);
	virtual BOOL SetCaptureBuffer(SIZE_T nBlockSize, SIZE_T nBlockCount, SIZE_T nAlign);

	virtual BOOL CaptureControl(int nControlCode = DEVICE_START);
	virtual BOOL SetGrabberCallback(ISampleGrabberCB *lpGrabber, long lSampleType);

	BOOL GetAudioCap(int nIndex, WAVEFORMATEX *lpWaveInfo);
	BOOL GetCurrentAudioCap(WAVEFORMATEX *lpWaveInfo);

	static void FreeMediaType(AM_MEDIA_TYPE *lpAMMediaType);

protected:
	BOOL FindPin(IPin **ppInputPin, IPin **ppOutputPin);

private:
	TCHAR						m_szActiveDeviceID[MAX_PATH];
	CAtlList<AGORA_DEVICE_INFO> m_listDeviceInfo;

	AUDIO_STREAM_CONFIG_CAPS	m_ascStreamCfgCaps;
	int							m_nCapSelected;

	// private dshow object
private:
	ISampleGrabberCB				*m_lpSampleGrabberCB;

	CComPtr<IGraphBuilder>			m_ptrGraphBuilder;
	CComPtr<ISampleGrabber>			m_ptrSampleGrabber;
	CComPtr<ICaptureGraphBuilder2>	m_ptrCaptureGraphBuilder2;
};
