#include "stdafx.h"
#include "AudioPlayPackageQueue.h"


CAudioPlayPackageQueue	*CAudioPlayPackageQueue::m_lpAudioPackageQueue = NULL;

// 
CAudioPlayPackageQueue::CAudioPlayPackageQueue()
{
	m_bufQueue.Create(32, 8192);
	m_nPackageSize = 8192;
}


CAudioPlayPackageQueue::~CAudioPlayPackageQueue()
{
	m_bufQueue.FreeAllBusyBlock();
	m_bufQueue.Close();
}

CAudioPlayPackageQueue *CAudioPlayPackageQueue::GetInstance()
{
	if (m_lpAudioPackageQueue == NULL)
		m_lpAudioPackageQueue = new CAudioPlayPackageQueue();

	return m_lpAudioPackageQueue;
}

void CAudioPlayPackageQueue::CloseInstance()
{
	if (m_lpAudioPackageQueue == NULL)
		return;

	delete m_lpAudioPackageQueue;
	m_lpAudioPackageQueue = NULL;
}

void CAudioPlayPackageQueue::SetAudioPackageSize(SIZE_T nPackageSize)
{
	_ASSERT(nPackageSize > 0 && nPackageSize <= m_bufQueue.GetBytesPreUnit());
	
	if (nPackageSize == 0 || nPackageSize > m_bufQueue.GetBytesPreUnit())
		return;

	m_nPackageSize = nPackageSize;
}

void CAudioPlayPackageQueue::SetAudioFormat(const WAVEFORMATEX *lpWaveInfo)
{
	memcpy_s(&m_waveFormat, sizeof(WAVEFORMATEX), lpWaveInfo, sizeof(WAVEFORMATEX));
}

void CAudioPlayPackageQueue::GetAudioFormat(WAVEFORMATEX *lpWaveInfo)
{
	memcpy_s(lpWaveInfo, sizeof(WAVEFORMATEX), &m_waveFormat, sizeof(WAVEFORMATEX));
}

BOOL CAudioPlayPackageQueue::PushAudioPackage(LPCVOID lpAudioPackage, SIZE_T nPackageSize)
{
	if (m_bufQueue.GetFreeCount() == 0)
		m_bufQueue.FreeBusyHead();

	LPVOID lpBuffer = m_bufQueue.AllocBuffer(FALSE);
	if (lpBuffer == NULL)
		return FALSE;

	_ASSERT(m_bufQueue.GetBytesPreUnit() >= nPackageSize);

	memcpy_s(lpBuffer, m_bufQueue.GetBytesPreUnit(), lpAudioPackage, nPackageSize);

	return TRUE;
}

BOOL CAudioPlayPackageQueue::PopAudioPackage(LPVOID lpAudioPackage, SIZE_T *nPackageSize)
{
	_ASSERT(nPackageSize != NULL);

	if (nPackageSize == NULL)
		return FALSE;

	if (m_bufQueue.GetFreeCount() == 0)
		return FALSE;

	if (*nPackageSize < m_nPackageSize) {
		*nPackageSize = m_nPackageSize;
		return FALSE;
	}

	LPVOID lpBuffer = m_bufQueue.GetBusyHead();
	memcpy_s(lpAudioPackage, *nPackageSize, lpAudioPackage, m_nPackageSize);
	*nPackageSize = m_nPackageSize;
	m_bufQueue.FreeBusyHead();
	
	return TRUE;
}