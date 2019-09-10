#include "stdafx.h"
#include "AudioCapturePackageQueue.hpp"


CAudioCapturePackageQueue	*CAudioCapturePackageQueue::m_lpAudioPackageQueue = NULL;

// 
CAudioCapturePackageQueue::CAudioCapturePackageQueue()
{
	m_bufQueue.Create(32, 8192);
	m_nPackageSize = 8192;
}


CAudioCapturePackageQueue::~CAudioCapturePackageQueue()
{
	m_bufQueue.FreeAllBusyBlock();
	m_bufQueue.Close();
}

CAudioCapturePackageQueue *CAudioCapturePackageQueue::GetInstance()
{
	if (m_lpAudioPackageQueue == NULL)
		m_lpAudioPackageQueue = new CAudioCapturePackageQueue();

	return m_lpAudioPackageQueue;
}

void CAudioCapturePackageQueue::CloseInstance()
{
	if (m_lpAudioPackageQueue == NULL)
		return;

	delete m_lpAudioPackageQueue;
	m_lpAudioPackageQueue = NULL;
}

void CAudioCapturePackageQueue::SetAudioPackageSize(SIZE_T nPackageSize)
{
	_ASSERT(nPackageSize > 0 && nPackageSize <= m_bufQueue.GetBytesPreUnit());
	
	if (nPackageSize == 0 || nPackageSize > m_bufQueue.GetBytesPreUnit())
		return;

	m_nPackageSize = nPackageSize;
}

void CAudioCapturePackageQueue::SetAudioFormat(const WAVEFORMATEX *lpWaveInfo)
{
	memcpy(&m_waveFormat, lpWaveInfo, sizeof(WAVEFORMATEX));
}

void CAudioCapturePackageQueue::GetAudioFormat(WAVEFORMATEX *lpWaveInfo)
{
	memcpy(lpWaveInfo, &m_waveFormat, sizeof(WAVEFORMATEX));
}

BOOL CAudioCapturePackageQueue::PushAudioPackage(LPCVOID lpAudioPackage, SIZE_T nPackageSize)
{
	//LOG_INFO("%s %s(%d)\n", getLogTime().c_str(), __FUNCTION__, nPackageSize);

	if (nPackageSize != m_nPackageSize)
		return FALSE;

	if (m_bufQueue.GetFreeCount() == 0)
		m_bufQueue.FreeBusyHead(NULL, 0);

	LPVOID lpBuffer = m_bufQueue.AllocBuffer(FALSE);
	if (lpBuffer == NULL)
		return FALSE;

	_ASSERT(m_bufQueue.GetBytesPreUnit() >= nPackageSize);

	memcpy(lpBuffer, lpAudioPackage, nPackageSize);

	return TRUE;
}

BOOL CAudioCapturePackageQueue::PopAudioPackage(LPVOID lpAudioPackage, SIZE_T *nPackageSize)
{
//	LOG_INFO("%s %s(%d)\n", getLogTime().c_str(), __FUNCTION__, nPackageSize);

	_ASSERT(nPackageSize != NULL);

	if (nPackageSize == NULL)
		return FALSE;

	if (m_bufQueue.GetBusyCount() == 0)
		return FALSE;

	if (*nPackageSize < m_nPackageSize) {
		*nPackageSize = m_nPackageSize;
		return FALSE;
	}

	m_bufQueue.FreeBusyHead(lpAudioPackage, m_nPackageSize);

	*nPackageSize = m_nPackageSize;
	
	return TRUE;
}