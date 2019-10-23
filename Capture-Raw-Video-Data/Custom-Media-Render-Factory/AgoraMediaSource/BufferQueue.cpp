#include "stdafx.h"
#include "BufferQueue.h"


CBufferQueue::CBufferQueue()
{
	::InitializeCriticalSection(&m_csAllocLock);
	::InitializeCriticalSection(&m_csFreeLock);
	::InitializeCriticalSection(&m_csBusyLock);
}


CBufferQueue::~CBufferQueue()
{
	::DeleteCriticalSection(&m_csAllocLock);
	::DeleteCriticalSection(&m_csFreeLock);
	::DeleteCriticalSection(&m_csBusyLock);
}


BOOL CBufferQueue::Create(int nUnitCount, SIZE_T nBytesPreUnit)
{
	_ASSERT(m_listFreeUnit.IsEmpty());

	if (nUnitCount < 0 || nBytesPreUnit == 0)
		return FALSE;

	if (!m_listFreeUnit.IsEmpty())
		return TRUE;

	LPBYTE	lpBuffer = NULL;

	::EnterCriticalSection(&m_csFreeLock);

	for (int nIndex = 0; nIndex < nUnitCount; nIndex++){
		lpBuffer = new BYTE[nBytesPreUnit];
		m_listFreeUnit.AddTail(lpBuffer);
	}

	::LeaveCriticalSection(&m_csFreeLock);

	m_nUnitCount = nUnitCount;
	m_nCurrentCount = nUnitCount;
	m_nBytesPreUnit = nBytesPreUnit;

	return TRUE;
}

BOOL CBufferQueue::Close()
{
	LPBYTE lpBuffer = NULL;

	_ASSERT(m_nUnitCount > 0 && m_nBytesPreUnit > 0);
	_ASSERT(m_listBusyUnit.IsEmpty());

	::EnterCriticalSection(&m_csAllocLock);
	::EnterCriticalSection(&m_csFreeLock);
	::EnterCriticalSection(&m_csBusyLock);

	if (m_listBusyUnit.GetCount() != 0){		
		::LeaveCriticalSection(&m_csBusyLock);
		::LeaveCriticalSection(&m_csFreeLock);
		::LeaveCriticalSection(&m_csAllocLock);

		return FALSE;
	}

	::LeaveCriticalSection(&m_csBusyLock);	

	while (m_listFreeUnit.GetCount() > 0){
		lpBuffer = m_listFreeUnit.RemoveHead();
		delete[] lpBuffer;
	}

	m_nBytesPreUnit = 0;
	m_nUnitCount = 0;
	m_nCurrentCount = 0;

	::LeaveCriticalSection(&m_csFreeLock);
	::LeaveCriticalSection(&m_csAllocLock);	

	return TRUE;
}


void CBufferQueue::Clear() {
	BOOL bRet = TRUE;

	do {
		bRet = FreeBusyHead();
	} while (bRet);
}

int	CBufferQueue::GetFreeCount() const
{
	return (int)m_listFreeUnit.GetCount();
}

int	CBufferQueue::GetBusyCount() const
{
	return (int)m_listBusyUnit.GetCount();
}

LPVOID	CBufferQueue::AllocBuffer(BOOL bForceAlloc)
{
	LPBYTE		lpBuffer = NULL;	
	POSITION	posHead = NULL;		
	POSITION	posTail = NULL;		

	_ASSERT(m_nUnitCount > 0 && m_nBytesPreUnit > 0);

	::EnterCriticalSection(&m_csAllocLock);				
	posHead = m_listFreeUnit.GetHeadPosition();			
	posTail = m_listFreeUnit.GetTailPosition();			

	if (posHead != NULL && posHead != posTail)			
		lpBuffer = m_listFreeUnit.RemoveHead();
	else if (posHead != NULL && posHead == posTail){	
		::EnterCriticalSection(&m_csFreeLock);
		lpBuffer = m_listFreeUnit.RemoveHead();
		::LeaveCriticalSection(&m_csFreeLock);
	}
	else{												
		if (bForceAlloc){								
			lpBuffer = new BYTE[m_nBytesPreUnit];
			m_nCurrentCount++;
		}
		else
			lpBuffer = NULL;
	}
	::LeaveCriticalSection(&m_csAllocLock);				

	if (lpBuffer == NULL)
		return NULL;

	::EnterCriticalSection(&m_csBusyLock);
	m_listBusyUnit.AddTail(lpBuffer);
	::LeaveCriticalSection(&m_csBusyLock);

	return lpBuffer;
}

LPVOID	CBufferQueue::GetBusyHead()
{
	LPBYTE lpBuffer = NULL;

	::EnterCriticalSection(&m_csBusyLock);
	if (!m_listBusyUnit.IsEmpty())
		lpBuffer = m_listBusyUnit.GetHead();
	::LeaveCriticalSection(&m_csBusyLock);

	return lpBuffer;
}

BOOL CBufferQueue::FreeBusyHead()
{
	BOOL bRet = FALSE;
	LPBYTE lpBuffer = NULL;

	_ASSERT(m_nUnitCount > 0 && m_nBytesPreUnit > 0);

	::EnterCriticalSection(&m_csBusyLock);
	if (!m_listBusyUnit.IsEmpty())
		lpBuffer = m_listBusyUnit.RemoveHead();
	::LeaveCriticalSection(&m_csBusyLock);

	if (lpBuffer != NULL) {
		::EnterCriticalSection(&m_csFreeLock);
		m_listFreeUnit.AddTail(lpBuffer);
		::LeaveCriticalSection(&m_csFreeLock);
	}

	return lpBuffer != NULL ? TRUE : FALSE;
}

void CBufferQueue::FreeAllBusyBlock()
{
	BOOL bRet = TRUE;

	do {
		bRet = FreeBusyHead();
	} while (!bRet);
}