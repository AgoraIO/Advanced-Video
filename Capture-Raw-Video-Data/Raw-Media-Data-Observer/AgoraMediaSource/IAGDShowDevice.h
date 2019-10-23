#pragma once

#include <DShow.h>
#include "DShow/qedit.h"

#include <tchar.h>
#include <windows.h>

#pragma comment(lib, "DShow/strmbase.lib")


typedef struct _AGORA_DEVICE_INFO
{
	GUID	guidDevice;
	TCHAR	szDeviceName[MAX_PATH];
	TCHAR	szDevicePath[MAX_PATH];
	TCHAR	szDeviceDesc[MAX_PATH];

} AGORA_DEVICE_INFO, *PAGORA_DEVICE_INFO, *LPAGORA_DEVICE_INFO;

enum {
	DEVICE_START = 0,			// start capture
	DEVICE_STOP,				// stop capture
};

class IDShowCaptureDevice
{
public:

	virtual BOOL Create() = NULL;
	virtual void Close() = NULL;

	virtual BOOL EnumDeviceList() = NULL;
	virtual int GetDeviceCount() = NULL;
	virtual BOOL GetDeviceInfo(int nIndex, LPAGORA_DEVICE_INFO lpDeviceInfo) = NULL;

	virtual BOOL OpenDevice(int nIndex) = NULL;
	virtual BOOL OpenDevice(LPCTSTR lpDevicePath) = NULL;
	virtual int GetCurrentDevice(LPTSTR lpDevicePath, SIZE_T *nDevicePathLen) = NULL;
	virtual void CloseDevice() = NULL;

	virtual int GetMediaCapCount() = NULL;
	virtual BOOL GetMediaCap(int nIndex, AM_MEDIA_TYPE **ppMediaType, LPVOID lpMediaStreamConfigCaps, SIZE_T nSize) = NULL;
	virtual BOOL SelectMediaCap(int nIndex) = NULL;
	virtual int GetSelectedMediaCap() = NULL;

	virtual BOOL GetCaptureBuffer(SIZE_T *nBlockSize, SIZE_T *nBlockCount, SIZE_T *nAlign) = NULL;
	virtual BOOL SetGrabberCallback(ISampleGrabberCB *lpGrabber, long lSampleType) = NULL;
	virtual BOOL CaptureControl(int nControlCode) = NULL;
};