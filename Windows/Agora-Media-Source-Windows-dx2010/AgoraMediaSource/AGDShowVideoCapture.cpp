#include "stdafx.h"
#include "AGDShowVideoCapture.h"
#include "DShowPinHelper.h"

CAGDShowVideoCapture::CAGDShowVideoCapture()
	: m_ptrGraphBuilder(NULL)
	, m_ptrSampleGrabber(NULL)
	, m_ptrCaptureGraphBuilder2(NULL)
	, m_lpSampleGrabberCB(NULL)
	, m_nCapSelected(-1)
{
	memset(m_szActiveDeviceID, 0, MAX_PATH*sizeof(TCHAR));
}


CAGDShowVideoCapture::~CAGDShowVideoCapture()
{
	Close();
}

BOOL CAGDShowVideoCapture::Create()
{
	HRESULT			hResult = S_OK;

	hResult = m_ptrGraphBuilder.CoCreateInstance(CLSID_FilterGraph);
	ATLASSERT(SUCCEEDED(hResult));
	if (hResult != S_OK)
		return FALSE;

	hResult = m_ptrSampleGrabber.CoCreateInstance(CLSID_SampleGrabber);
	ATLASSERT(SUCCEEDED(hResult));
	if (hResult != S_OK)
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2.CoCreateInstance(CLSID_CaptureGraphBuilder2);
	ATLASSERT(SUCCEEDED(hResult));
	if (hResult != S_OK)
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->SetFiltergraph(m_ptrGraphBuilder);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrSampleGrabber->SetBufferSamples(TRUE);
	ATLASSERT(SUCCEEDED(hResult));
	if (hResult != S_OK)
		return FALSE;

	hResult = m_ptrSampleGrabber->SetOneShot(FALSE);
	ATLASSERT(SUCCEEDED(hResult));
	if (hResult != S_OK)
		return FALSE;

	CComQIPtr<IBaseFilter, &IID_IBaseFilter> ptrGrabberFilter(m_ptrSampleGrabber);
	hResult = m_ptrGraphBuilder->AddFilter(ptrGrabberFilter, L"Grabber");
	ATLASSERT(SUCCEEDED(hResult));
	if (hResult != S_OK)
		return FALSE;

	return TRUE;
}

void CAGDShowVideoCapture::Close()
{
	CloseDevice();

	m_ptrGraphBuilder = NULL;
	m_ptrSampleGrabber = NULL;
	m_ptrCaptureGraphBuilder2 = NULL;
}

BOOL CAGDShowVideoCapture::EnumDeviceList()
{
	HRESULT		hResult = S_OK;

	CComVariant		var;
	WCHAR			*wszDevicePath = NULL;

	CComPtr<ICreateDevEnum>		ptrCreateDevEnum = NULL;
	CComPtr<IEnumMoniker>		ptrEnumMoniker = NULL;
	CComPtr<IMoniker>			ptrMoniker = NULL;
	CComPtr<IPropertyBag>		ptrPropertyBag = NULL;

	AGORA_DEVICE_INFO			agDeviceInfo;

	hResult = ptrCreateDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum);
	if (FAILED(hResult))
		return FALSE;

	hResult = ptrCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &ptrEnumMoniker, 0);
	if (FAILED(hResult))
		return FALSE;

	m_listDeviceInfo.RemoveAll();

	do {
		ptrMoniker = NULL;
		hResult = ptrEnumMoniker->Next(1, &ptrMoniker, NULL);
		if (hResult != S_OK)
			break;

		ptrPropertyBag = NULL;
		hResult = ptrMoniker->BindToStorage(NULL, NULL, IID_IPropertyBag, (void**)(&ptrPropertyBag));
		if (hResult != S_OK)
			break;

		memset(&agDeviceInfo, 0, sizeof(AGORA_DEVICE_INFO));

		var.Clear();
		hResult = ptrPropertyBag->Read(L"FriendlyName", &var, NULL);
		if (SUCCEEDED(hResult)){
#ifdef UNICODE
			_tcscpy_s(agDeviceInfo.szDeviceName, var.bstrVal);
#else
			::WideCharToMultiByte(CP_ACP, 0, var.bstrVal, -1, agDeviceInfo.szDeviceName, MAX_PATH, NULL, NULL);
#endif
		}

		hResult = ptrMoniker->GetDisplayName(NULL, NULL, &wszDevicePath);
		if (SUCCEEDED(hResult)){
#ifdef UNICODE
			_tcscpy_s(agDeviceInfo.szDevicePath, wszDevicePath);
#else
			::WideCharToMultiByte(CP_ACP, 0, wszDevicePath, -1, agDeviceInfo.szDevicePath, MAX_PATH, NULL, NULL);
#endif
			::CoTaskMemFree(wszDevicePath);
		}

		m_listDeviceInfo.AddTail(agDeviceInfo);

	} while (TRUE);

	return TRUE;
}

BOOL CAGDShowVideoCapture::GetDeviceInfo(int nIndex, LPAGORA_DEVICE_INFO lpDeviceInfo)
{
	ATLASSERT(lpDeviceInfo != NULL);
	ATLASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_listDeviceInfo.GetCount()));

	POSITION pos = m_listDeviceInfo.FindIndex(nIndex);
	if (pos == NULL)
		return FALSE;

	AGORA_DEVICE_INFO &agDeviceInfo = m_listDeviceInfo.GetAt(pos);
	memcpy(lpDeviceInfo, &agDeviceInfo, sizeof(AGORA_DEVICE_INFO));

	return TRUE;
}

BOOL CAGDShowVideoCapture::OpenDevice(int nIndex)
{
	ATLASSERT(nIndex >= 0 && nIndex < static_cast<int>(m_listDeviceInfo.GetCount()) );

	m_nCapSelected = -1;
	POSITION pos = m_listDeviceInfo.FindIndex(nIndex);
	if (pos == NULL)
		return FALSE;

	LPCTSTR	lpDevicePath = m_listDeviceInfo.GetAt(pos).szDevicePath;

	return OpenDevice(lpDevicePath);
}

BOOL CAGDShowVideoCapture::OpenDevice(LPCTSTR lpDevicePath)
{
	ULONG		ulEaten = 0;
	HRESULT		hResult = S_OK;

	CComPtr<IBindCtx>		lpBindCtx = NULL;
	CComPtr<IMoniker>		ptrMoniker = NULL;
	CComPtr<IBaseFilter>	ptrSourceFilter = NULL;

	m_nCapSelected = -1;
	ATLASSERT(_tcslen(m_szActiveDeviceID) == 0);

	hResult = ::CreateBindCtx(0, &lpBindCtx);
	if (hResult != S_OK)
		return FALSE;

#ifdef UNICODE
	hResult = ::MkParseDisplayName(lpBindCtx, lpDevicePath, &ulEaten, &ptrMoniker);
#else
	WCHAR wszDeviceID[128];
	memset(wszDeviceID, 0, 128 * sizeof(WCHAR));
	::MultiByteToWideChar(CP_ACP, 0, lpDevicePath, -1, wszDeviceID, 128);
	hResult = ::MkParseDisplayName(lpBindCtx, wszDeviceID, &ulEaten, &ptrMoniker);
#endif

	if (hResult != S_OK)
		return FALSE;

	hResult = ptrMoniker->BindToObject(0, 0, IID_IBaseFilter, (void **)&ptrSourceFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (hResult != S_OK)
		return FALSE;
	
	hResult = m_ptrGraphBuilder->AddFilter(ptrSourceFilter, L"Source");
	ATLASSERT(SUCCEEDED(hResult));
	if (hResult != S_OK)
		return FALSE;

	_tcscpy_s(m_szActiveDeviceID, MAX_PATH, lpDevicePath);

	SelectMediaCap(0);

	return TRUE;
}

BOOL CAGDShowVideoCapture::GetCurrentDevice(LPTSTR lpDevicePath, SIZE_T *nDevicePathLen)
{
	int nDeviceLen = _tcslen(m_szActiveDeviceID);
	if (nDeviceLen >= static_cast<int>(*nDevicePathLen)) {
		*nDevicePathLen = nDeviceLen+1;
		return FALSE;
	}

	if (nDeviceLen == 0)
		return FALSE;

	_tcscpy_s(lpDevicePath, *nDevicePathLen, m_szActiveDeviceID);
	*nDevicePathLen = nDeviceLen + 1;

	return TRUE;
}

void CAGDShowVideoCapture::CloseDevice()
{
	CComPtr<IBaseFilter>	ptrSourceFilter = NULL;

	ATLASSERT(_tcslen(m_szActiveDeviceID) > 0);
	if (_tcslen(m_szActiveDeviceID) == 0)
		return;

	CaptureControl(DEVICE_STOP);
	HRESULT hResult = m_ptrGraphBuilder->FindFilterByName(L"Source", &ptrSourceFilter);

	if (SUCCEEDED(hResult)) {
		m_ptrGraphBuilder->RemoveFilter(ptrSourceFilter);
		m_nCapSelected = -1;
	}

	memset(m_szActiveDeviceID, 0, MAX_PATH*sizeof(TCHAR));
}

BOOL CAGDShowVideoCapture::SetGrabberCallback(ISampleGrabberCB *lpGrabber, long lSampleType)
{
	HRESULT hResult = m_ptrSampleGrabber->SetCallback(lpGrabber, lSampleType);

	return SUCCEEDED(hResult);
}

int CAGDShowVideoCapture::GetMediaCapCount()
{
	int		nCount = 0;
	int		nSize = 0;
	HRESULT hResult = S_OK;

	CComPtr<IBaseFilter>			ptrCaptureFilter = NULL;
	CComPtr<IAMStreamConfig>		ptrStreamConfig = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(L"Source", &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return 0;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return 0;

	hResult = ptrStreamConfig->GetNumberOfCapabilities(&nCount, &nSize);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return 0;

	return nCount;
}

BOOL CAGDShowVideoCapture::GetMediaCap(int nIndex, AM_MEDIA_TYPE **ppMediaType, LPVOID lpMediaStreamConfigCaps, SIZE_T nSize)
{
	int		nCount = 0;
	int		nCapSize = 0;
	HRESULT hResult = S_OK;

	CComPtr<IBaseFilter>			ptrCaptureFilter = NULL;
	CComPtr<IAMStreamConfig>		ptrStreamConfig = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(L"Source", &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = ptrStreamConfig->GetNumberOfCapabilities(&nCount, &nCapSize);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	ATLASSERT(nCapSize <= static_cast<int>(nSize));
	if (nCapSize > static_cast<int>(nSize))
		return FALSE;

	hResult = ptrStreamConfig->GetStreamCaps(nIndex, ppMediaType, reinterpret_cast<BYTE*>(lpMediaStreamConfigCaps));
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	return TRUE;
}

BOOL CAGDShowVideoCapture::GetVideoCap(int nIndex, VIDEOINFOHEADER *lpVideoInfo)
{
	int		nCount = 0;
	int		nSize = 0;

	AM_MEDIA_TYPE *lpAMMediaType = NULL;
	VIDEO_STREAM_CONFIG_CAPS videoStreamCfgCaps;

	BOOL bSuccess = GetMediaCap(nIndex, &lpAMMediaType, &videoStreamCfgCaps, sizeof(VIDEO_STREAM_CONFIG_CAPS));

	if (lpAMMediaType->formattype == FORMAT_VideoInfo) {
		VIDEOINFOHEADER* pVideoInfo = reinterpret_cast<VIDEOINFOHEADER*>(lpAMMediaType->pbFormat);
		memcpy_s(lpVideoInfo, sizeof(VIDEOINFOHEADER), pVideoInfo, sizeof(VIDEOINFOHEADER));

		bSuccess = TRUE;
	}
	else if (lpAMMediaType->formattype == FORMAT_VideoInfo2) {
		VIDEOINFOHEADER2* pVideoInfo2 = reinterpret_cast< VIDEOINFOHEADER2*>(lpAMMediaType->pbFormat);
		memcpy_s(&lpVideoInfo->bmiHeader, sizeof(BITMAPINFOHEADER), &pVideoInfo2->bmiHeader, sizeof(BITMAPINFOHEADER));
		lpVideoInfo->AvgTimePerFrame = pVideoInfo2->AvgTimePerFrame;
		lpVideoInfo->dwBitErrorRate = pVideoInfo2->dwBitErrorRate;
		lpVideoInfo->dwBitRate = pVideoInfo2->dwBitRate;
		bSuccess = TRUE;
	}
	else
		bSuccess = FALSE;

	FreeMediaType(lpAMMediaType);

	return bSuccess;
}

BOOL CAGDShowVideoCapture::GetCurrentVideoCap(VIDEOINFOHEADER *lpVideoInfo)
{
	BOOL			bSuccess = FALSE;
	HRESULT			hResult = S_OK;
	AM_MEDIA_TYPE	*lpAMMediaType = NULL;

	CComPtr<IBaseFilter>			ptrCaptureFilter = NULL;
	CComPtr<IAMStreamConfig>		ptrStreamConfig = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(L"Source", &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = ptrStreamConfig->GetFormat(&lpAMMediaType);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	if (lpAMMediaType->formattype == FORMAT_VideoInfo) {
		VIDEOINFOHEADER* pVideoInfo = reinterpret_cast<VIDEOINFOHEADER*>(lpAMMediaType->pbFormat);
		memcpy_s(lpVideoInfo, sizeof(VIDEOINFOHEADER), pVideoInfo, sizeof(VIDEOINFOHEADER));

		bSuccess = TRUE;
	}
	else if (lpAMMediaType->formattype == FORMAT_VideoInfo2) {
		VIDEOINFOHEADER2* pVideoInfo2 = reinterpret_cast< VIDEOINFOHEADER2*>(lpAMMediaType->pbFormat);
		memcpy_s(&lpVideoInfo->bmiHeader, sizeof(BITMAPINFOHEADER), &pVideoInfo2->bmiHeader, sizeof(BITMAPINFOHEADER));
		lpVideoInfo->AvgTimePerFrame = pVideoInfo2->AvgTimePerFrame;
		lpVideoInfo->dwBitErrorRate = pVideoInfo2->dwBitErrorRate;
		lpVideoInfo->dwBitRate = pVideoInfo2->dwBitRate;
		bSuccess = TRUE;
	}
	else
		bSuccess = FALSE;

	FreeMediaType(lpAMMediaType);

	return bSuccess;
}

BOOL CAGDShowVideoCapture::SelectMediaCap(int nIndex)
{
	int		nCount = 0;
	int		nSize = 0;
	HRESULT hResult = S_OK;

	AM_MEDIA_TYPE	*lpMediaType = NULL;

	CComPtr<IBaseFilter>		ptrCaptureFilter = NULL;
	CComPtr<IAMStreamConfig>	ptrStreamConfig = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(L"Source", &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, ptrCaptureFilter, IID_IAMStreamConfig, (void**)&ptrStreamConfig);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = ptrStreamConfig->GetNumberOfCapabilities(&nCount, &nSize);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	ATLASSERT(nIndex >= 0 && nIndex < nCount);
	if (nIndex < 0 || nIndex >= nCount)
		nIndex = 0;

	ATLASSERT(nSize <= sizeof(VIDEO_STREAM_CONFIG_CAPS));

	do {
		hResult = ptrStreamConfig->GetStreamCaps(nIndex, &lpMediaType, reinterpret_cast<BYTE*>(&m_vscStreamCfgCaps));
		ATLASSERT(SUCCEEDED(hResult));
		if (FAILED(hResult))
			break;

		hResult = ptrStreamConfig->SetFormat(lpMediaType);
		ATLASSERT(SUCCEEDED(hResult));
		if (FAILED(hResult))
			break;

		hResult = m_ptrSampleGrabber->SetMediaType(lpMediaType);
		ATLASSERT(SUCCEEDED(hResult));
		if (FAILED(hResult))
			break;

		m_nCapSelected = nIndex;

	} while (FALSE);

	FreeMediaType(lpMediaType);

	return SUCCEEDED(hResult);
}

BOOL CAGDShowVideoCapture::GetCaptureBuffer(SIZE_T *nBlockSize, SIZE_T *nBlockCount, SIZE_T *nAlign)
{
	HRESULT							hResult = S_OK;
	CComPtr<IBaseFilter>			ptrCaptureFilter = NULL;
	CComPtr<IAMBufferNegotiation>	ptrBufferNegotiation = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(L"Source", &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, ptrCaptureFilter, IID_IAMBufferNegotiation, (void**)&ptrBufferNegotiation);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	ALLOCATOR_PROPERTIES alcProper;

	memset(&alcProper, 0, sizeof(ALLOCATOR_PROPERTIES));
	hResult = ptrBufferNegotiation->GetAllocatorProperties(&alcProper);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	*nBlockSize = alcProper.cbBuffer;
	*nBlockCount = alcProper.cBuffers;
	*nAlign = alcProper.cbAlign;

	return TRUE;
}

BOOL CAGDShowVideoCapture::SetCaptureBuffer(SIZE_T nBlockSize, SIZE_T nBlockCount, SIZE_T nAlign)
{
	HRESULT							hResult = S_OK;
	CComPtr<IBaseFilter>			ptrCaptureFilter = NULL;
	CComPtr<IAMBufferNegotiation>	ptrBufferNegotiation = NULL;

	hResult = m_ptrGraphBuilder->FindFilterByName(L"Source", &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, ptrCaptureFilter, IID_IAMBufferNegotiation, (void**)&ptrBufferNegotiation);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	ALLOCATOR_PROPERTIES alcProper;

	alcProper.cbBuffer = nBlockSize;
	alcProper.cBuffers = nBlockCount;
	alcProper.cbAlign = nAlign;
	alcProper.cbPrefix = 0;

	hResult = ptrBufferNegotiation->SuggestAllocatorProperties(&alcProper);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	return TRUE;
}

BOOL CAGDShowVideoCapture::CaptureControl(int nControlCode)
{
	CComPtr<IPin> ptrInputPin = NULL;
	CComPtr<IPin> ptrOutputPin = NULL;

	CComQIPtr<IMediaControl, &IID_IMediaControl> ptrControl(m_ptrGraphBuilder);

	FindPin(&ptrInputPin, &ptrOutputPin);

	HRESULT hResult = S_OK;
	
	switch (nControlCode)
	{
	case DEVICE_START:
		hResult = m_ptrGraphBuilder->Connect(ptrInputPin, ptrOutputPin);
		ATLASSERT(SUCCEEDED(hResult));
		if (FAILED(hResult))
			return FALSE;

		hResult = ptrControl->Run();
		break;
	case DEVICE_STOP:
		hResult = ptrControl->Stop();
		ATLASSERT(SUCCEEDED(hResult));
		if (FAILED(hResult))
			return FALSE;

		hResult = m_ptrGraphBuilder->Disconnect(ptrInputPin);
		hResult = m_ptrGraphBuilder->Disconnect(ptrOutputPin);
		break;
	default:
		ATLASSERT(FALSE);
		break;
	}

	ATLASSERT(SUCCEEDED(hResult));

	return hResult == S_OK ? TRUE : FALSE;
}

BOOL CAGDShowVideoCapture::FindPin(IPin **ppInputPin, IPin **ppOutputPin)
{
	HRESULT hResult = S_OK;

	CComPtr<IBaseFilter> ptrCaptureFilter = NULL;
	CComPtr<IBaseFilter> ptrSourceFilter = NULL;
	CDShowPinHelper	pinHelper;

	hResult = m_ptrGraphBuilder->FindFilterByName(L"Grabber", &ptrCaptureFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	hResult = m_ptrGraphBuilder->FindFilterByName(L"Source", &ptrSourceFilter);
	ATLASSERT(SUCCEEDED(hResult));
	if (FAILED(hResult))
		return FALSE;

	if (pinHelper.Open(ptrSourceFilter)) {
		pinHelper.FindUnconnectedPin(PIN_DIRECTION::PINDIR_OUTPUT, ppOutputPin);
		pinHelper.Close();
	}

	if (pinHelper.Open(ptrCaptureFilter)) {
		pinHelper.FindUnconnectedPin(PIN_DIRECTION::PINDIR_INPUT, ppInputPin);
		pinHelper.Close();
	}

	return TRUE;
}

void CAGDShowVideoCapture::FreeMediaType(AM_MEDIA_TYPE *lpAMMediaType)
{
	if (lpAMMediaType == NULL)
		return;

	if (lpAMMediaType->cbFormat != 0) {
		::CoTaskMemFree((PVOID)lpAMMediaType->pbFormat);
		lpAMMediaType->cbFormat = 0;
		lpAMMediaType->pbFormat = NULL;
	}

	if (lpAMMediaType->pUnk != NULL) {
		lpAMMediaType->pUnk->Release();
		lpAMMediaType->pUnk = NULL;
	}

	::CoTaskMemFree(lpAMMediaType);
}
