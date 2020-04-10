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

	// 已经分配过了
	if (!m_listFreeUnit.IsEmpty())
		return TRUE;

	LPBYTE	lpBuffer = NULL;

	::EnterCriticalSection(&m_csFreeLock);

	// 初始化内存池
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

	::EnterCriticalSection(&m_csAllocLock);		// 进入分配临界，防止再分配
	::EnterCriticalSection(&m_csFreeLock);
	::EnterCriticalSection(&m_csBusyLock);		// 进入映射临界，检查是否有忙块

	if (m_listBusyUnit.GetCount() != 0){			// 还有忙块，禁止释放
		::LeaveCriticalSection(&m_csBusyLock);
		::LeaveCriticalSection(&m_csFreeLock);
		::LeaveCriticalSection(&m_csAllocLock);

		return FALSE;
	}

	::LeaveCriticalSection(&m_csBusyLock);		// 检查映射完毕，退出临界

	while (m_listFreeUnit.GetCount() > 0){
		lpBuffer = m_listFreeUnit.RemoveHead();
		delete[] lpBuffer;
	}

	m_nBytesPreUnit = 0;
	m_nUnitCount = 0;
	m_nCurrentCount = 0;

	::LeaveCriticalSection(&m_csFreeLock);
	::LeaveCriticalSection(&m_csAllocLock);		// 池内缓冲全部销毁完毕，退出临界

	return TRUE;
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
	LPBYTE		lpBuffer = NULL;	// 缓冲区
	POSITION	posHead = NULL;		// 链表头
	POSITION	posTail = NULL;		// 链表尾

	_ASSERT(m_nUnitCount > 0 && m_nBytesPreUnit > 0);

	::EnterCriticalSection(&m_csAllocLock);				// 进入分配临界
	posHead = m_listFreeUnit.GetHeadPosition();			// 检查链表是否为空
	posTail = m_listFreeUnit.GetTailPosition();			// 链表尾也要检查

	if (posHead != NULL && posHead != posTail)			// 链表非空且头尾不同
		lpBuffer = m_listFreeUnit.RemoveHead();
	else if (posHead != NULL && posHead == posTail){	// 只有一个空闲
		::EnterCriticalSection(&m_csFreeLock);
		lpBuffer = m_listFreeUnit.RemoveHead();
		::LeaveCriticalSection(&m_csFreeLock);
	}
	else{												// 链表无空闲
		if (bForceAlloc){								// 允许强制分配
			lpBuffer = new BYTE[m_nBytesPreUnit];
			m_nCurrentCount++;
		}
		else
			lpBuffer = NULL;
	}
	::LeaveCriticalSection(&m_csAllocLock);				// 退出分配临界

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