#pragma once


class CAGAudioCapture :
	public ISampleGrabberCB
{
public:
	CAGAudioCapture();
	~CAGAudioCapture();

	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	STDMETHODIMP SampleCB(double SampleTime, IMediaSample* pSample);
	STDMETHODIMP BufferCB(double dblSampleTime, BYTE* pBuffer, long lBufferSize);

private:
	UINT		m_nTimeStamp;

	ULONG		m_nRef;
};

