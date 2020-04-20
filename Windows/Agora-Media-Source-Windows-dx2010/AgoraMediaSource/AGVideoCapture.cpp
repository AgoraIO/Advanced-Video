#include "stdafx.h"
#include "AGVideoCapture.h"
#include "VideoPackageQueue.h"
#include "YUVTrans.h"


CAGVideoCapture::CAGVideoCapture()
	: m_nTimeStamp(0)
	, m_nRef(0)
{
	m_lpYUVBuffer = new BYTE[0x800000];
}


CAGVideoCapture::~CAGVideoCapture()
{
	delete[] m_lpYUVBuffer;
}

STDMETHODIMP_(ULONG) CAGVideoCapture::AddRef()
{
	m_nRef++;

	return m_nRef;
}

STDMETHODIMP_(ULONG) CAGVideoCapture::Release()
{
	m_nRef--;

	return m_nRef;
}

STDMETHODIMP CAGVideoCapture::QueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown)
	{
		*ppv = (void *) static_cast<ISampleGrabberCB*> (this);
		return NOERROR;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP CAGVideoCapture::SampleCB(double SampleTime, IMediaSample* pSample)
{
	return S_OK;
}

STDMETHODIMP CAGVideoCapture::BufferCB(double dblSampleTime, BYTE *pBuffer, long lBufferSize)
{
	CVideoPackageQueue *lpPackageQueue = CVideoPackageQueue::GetInstance();
	BITMAPINFOHEADER bmiHeader;

	if (lpPackageQueue->GetBufferSize() < static_cast<SIZE_T>(lBufferSize))
		return E_OUTOFMEMORY;

	lpPackageQueue->GetVideoFormat(&bmiHeader);

	m_lpY = m_lpYUVBuffer;
	m_lpU = m_lpY + bmiHeader.biWidth*bmiHeader.biHeight;
	m_lpV = m_lpU + bmiHeader.biWidth*bmiHeader.biHeight / 4;
	SIZE_T nYUVSize = bmiHeader.biWidth*bmiHeader.biHeight * 3 / 2;

	switch (bmiHeader.biCompression)
	{
	case 0x00000000:	// RGB24

		CYUVTrans::RGB24ToI420(pBuffer, m_lpYUVBuffer, 0x800000, bmiHeader.biWidth, bmiHeader.biHeight);
		break;
	case MAKEFOURCC('I', '4', '2', '0'):	// I420
		memcpy_s(m_lpYUVBuffer, 0x800000, pBuffer, lBufferSize);
		break;
	case MAKEFOURCC('Y', 'U', 'Y', '2'):	// YUY2
		CYUVTrans::YUY2ToI420(pBuffer, m_lpYUVBuffer, 0x800000, bmiHeader.biWidth, bmiHeader.biHeight);
		break;
	case MAKEFOURCC('M', 'J', 'P', 'G'):	// MJPEG NOT SUPPORTED!!!!!
		VERIFY(FALSE);
		break;
	case MAKEFOURCC('U', 'Y', 'V', 'Y'):	// UYVY
		CYUVTrans::UYVYToI420(pBuffer, m_lpYUVBuffer, 0x800000, bmiHeader.biWidth, bmiHeader.biHeight);
		break;
	default:
		ATLASSERT(FALSE);
		break;
	}

	lpPackageQueue->PushVideoPackage(m_lpYUVBuffer, nYUVSize);

	return S_OK;
}