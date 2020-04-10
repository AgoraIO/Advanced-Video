#pragma once
#include <DShow.h>



class CAGVideoCapture :
	public ISampleGrabberCB
{
public:
	CAGVideoCapture();
	~CAGVideoCapture();

	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	STDMETHODIMP SampleCB(double SampleTime, IMediaSample* pSample);
	STDMETHODIMP BufferCB(double dblSampleTime, BYTE* pBuffer, long lBufferSize);

//	BOOL SetVideoInfo(const VIDEOINFOHEADER *lpVideoInfo);

private:
	UINT		m_nTimeStamp;
	UINT		m_nFPS;

	ULONG		m_nRef;

	LPBYTE		m_lpYUVBuffer;
	LPBYTE		m_lpY;
	LPBYTE		m_lpU;
	LPBYTE		m_lpV;

};

