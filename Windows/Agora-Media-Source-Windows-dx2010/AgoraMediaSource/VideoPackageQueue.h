#pragma once

#include "BufferQueue.h"

class CVideoPackageQueue
{
protected:
	CVideoPackageQueue();
	~CVideoPackageQueue();

public:
	static CVideoPackageQueue *GetInstance();
	static void CloseInstance();

	void SetVideoFormat(const BITMAPINFOHEADER *lpInfoHeader);
	void GetVideoFormat(BITMAPINFOHEADER *lpInfoHeader);

	BOOL PushVideoPackage(LPCVOID lpAudioPackage, SIZE_T nPackageLen);
	BOOL PopVideoPackage(LPVOID lpVideoPackage, SIZE_T *nPackageSize);

	SIZE_T GetVideoPackageSize() const { return m_nPackageSize; };
	SIZE_T GetBufferSize() const { return m_nBufferSize; };

private:
	CBufferQueue		m_bufQueue;
	BITMAPINFOHEADER	m_bmiHeader;
	SIZE_T				m_nPackageSize;
	SIZE_T				m_nBufferSize;

	static CVideoPackageQueue	*m_lpVideoPackageQueue;
};


