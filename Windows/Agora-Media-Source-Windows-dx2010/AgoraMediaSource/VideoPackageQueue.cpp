#include "stdafx.h"
#include "VideoPackageQueue.h"


CVideoPackageQueue	*CVideoPackageQueue::m_lpVideoPackageQueue = NULL;

CVideoPackageQueue::CVideoPackageQueue()
{
	m_nPackageSize = 0;
	m_nBufferSize = 0x800000;
	m_bufQueue.Create(32, 0x800000);
}


CVideoPackageQueue::~CVideoPackageQueue()
{
	m_bufQueue.FreeAllBusyBlock();
	m_bufQueue.Close();
}

CVideoPackageQueue *CVideoPackageQueue::GetInstance()
{
	if (m_lpVideoPackageQueue == NULL)
		m_lpVideoPackageQueue = new CVideoPackageQueue();

	return m_lpVideoPackageQueue;
}

void CVideoPackageQueue::CloseInstance()
{
	if (m_lpVideoPackageQueue == NULL)
		return;

	delete m_lpVideoPackageQueue;
	m_lpVideoPackageQueue = NULL;
}

void CVideoPackageQueue::SetVideoFormat(const BITMAPINFOHEADER *lpInfoHeader)
{
	memcpy_s(&m_bmiHeader, sizeof(BITMAPINFOHEADER), lpInfoHeader, sizeof(BITMAPINFOHEADER));

	m_nPackageSize = m_bmiHeader.biWidth*m_bmiHeader.biWidth * 3 / 2;
	_ASSERT(m_nPackageSize <= m_nBufferSize);
}

void CVideoPackageQueue::GetVideoFormat(BITMAPINFOHEADER *lpInfoHeader)
{
	memcpy_s(lpInfoHeader, sizeof(BITMAPINFOHEADER), &m_bmiHeader, sizeof(BITMAPINFOHEADER));
}

BOOL CVideoPackageQueue::PushVideoPackage(LPCVOID lpVideoPackage, SIZE_T nPackageLen)
{
	if (m_bufQueue.GetFreeCount() == 0)
		m_bufQueue.FreeBusyHead();

	LPVOID lpBuffer = m_bufQueue.AllocBuffer(FALSE);
	if (lpBuffer == NULL) 
		return FALSE;

	_ASSERT(m_bufQueue.GetBytesPreUnit() >= nPackageLen);

	memcpy_s(lpBuffer, m_bufQueue.GetBytesPreUnit(), lpVideoPackage, nPackageLen);

	return TRUE;
}

BOOL CVideoPackageQueue::PopVideoPackage(LPVOID lpVideoPackage, SIZE_T *nPackageSize)
{
	_ASSERT(nPackageSize != NULL);

	if (nPackageSize == 0)
		return FALSE;

	if (m_bufQueue.GetBusyCount() == 0)
		return FALSE;

	if (*nPackageSize < m_nPackageSize) {
		*nPackageSize = m_nPackageSize;
		return FALSE;
	}

	LPVOID lpBuffer = m_bufQueue.GetBusyHead();
	memcpy_s(lpVideoPackage, *nPackageSize, lpBuffer, m_nPackageSize);
	*nPackageSize = m_nPackageSize;
	m_bufQueue.FreeBusyHead();

	return TRUE;
}