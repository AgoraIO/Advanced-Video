#include "stdafx.h"
#include "AGAudioCapture.h"
#include "AudioCapturePackageQueue.h"


CAGAudioCapture::CAGAudioCapture()
	: m_nTimeStamp(0)
	, m_nRef(0)
{
}


CAGAudioCapture::~CAGAudioCapture()
{
}


STDMETHODIMP_(ULONG) CAGAudioCapture::AddRef()
{
	m_nRef++;

	return m_nRef;
}

STDMETHODIMP_(ULONG) CAGAudioCapture::Release()
{
	m_nRef--;

	return m_nRef;
}

STDMETHODIMP CAGAudioCapture::QueryInterface(REFIID riid, void ** ppv)
{
	if (riid == IID_ISampleGrabberCB || riid == IID_IUnknown)
	{
		*ppv = (void *) static_cast<ISampleGrabberCB*> (this);
		return NOERROR;
	}

	return E_NOINTERFACE;
}

STDMETHODIMP CAGAudioCapture::SampleCB(double SampleTime, IMediaSample* pSample)
{
	return S_OK;
}

STDMETHODIMP CAGAudioCapture::BufferCB(double dblSampleTime, BYTE *pBuffer, long lBufferSize)
{
	CAudioCapturePackageQueue *lpPackageQueue = CAudioCapturePackageQueue::GetInstance();

	if (lpPackageQueue->GetAudioPackageSize() < lBufferSize)
		return E_OUTOFMEMORY;

	lpPackageQueue->PushAudioPackage(pBuffer, lBufferSize);

	return S_OK;
}