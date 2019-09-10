#include "stdafx.h"
#include "BufferQueue.hpp"


CBufferQueue::CBufferQueue()
{
	::InitializeCriticalSection(&m_csListLock);
}


CBufferQueue::~CBufferQueue()
{
	::DeleteCriticalSection(&m_csListLock);
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

	::EnterCriticalSection(&m_csListLock);

	// 初始化内存池
	for (int nIndex = 0; nIndex < nUnitCount; nIndex++){
		lpBuffer = new BYTE[nBytesPreUnit];
		m_listFreeUnit.AddTail(lpBuffer);
	}

	::LeaveCriticalSection(&m_csListLock);

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

	::EnterCriticalSection(&m_csListLock);

	if (m_listBusyUnit.GetCount() != 0){			// 还有忙块，禁止释放
		::LeaveCriticalSection(&m_csListLock);

		return FALSE;
	}

	while (m_listFreeUnit.GetCount() > 0){
		lpBuffer = m_listFreeUnit.RemoveHead();
		delete[] lpBuffer;
	}

	m_nBytesPreUnit = 0;
	m_nUnitCount = 0;
	m_nCurrentCount = 0;

	::LeaveCriticalSection(&m_csListLock);		// 池内缓冲全部销毁完毕，退出临界

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

	::EnterCriticalSection(&m_csListLock);				// 进入分配临界
	posHead = m_listFreeUnit.GetHeadPosition();			// 检查链表是否为空
	posTail = m_listFreeUnit.GetTailPosition();			// 链表尾也要检查

	if (posHead != NULL && posHead != posTail)			// 链表非空且头尾不同
		lpBuffer = m_listFreeUnit.RemoveHead();
	else if (posHead != NULL && posHead == posTail)		// 只有一个空闲
		lpBuffer = m_listFreeUnit.RemoveHead();
	else{												// 链表无空闲
		if (bForceAlloc){								// 允许强制分配
			lpBuffer = new BYTE[m_nBytesPreUnit];
			m_nCurrentCount++;
		}
		else
			lpBuffer = NULL;
	}

	if (lpBuffer == NULL) {
		::LeaveCriticalSection(&m_csListLock);
		return NULL;
	}

	m_listBusyUnit.AddTail(lpBuffer);

	::LeaveCriticalSection(&m_csListLock);

	return lpBuffer;
}

BOOL CBufferQueue::FreeBusyHead(LPVOID lpDestBuf, SIZE_T nBytesToCpoy)
{
	BOOL bRet = FALSE;
	LPBYTE lpBuffer = NULL;

	_ASSERT(m_nUnitCount > 0 && m_nBytesPreUnit > 0);

	::EnterCriticalSection(&m_csListLock);
	if (!m_listBusyUnit.IsEmpty())
		lpBuffer = m_listBusyUnit.RemoveHead();

	if (lpDestBuf != NULL && lpBuffer != NULL)
		memcpy(lpDestBuf, lpBuffer, nBytesToCpoy);

	if (lpBuffer != NULL)
		m_listFreeUnit.AddTail(lpBuffer);

	::LeaveCriticalSection(&m_csListLock);
	
	return lpBuffer != NULL ? TRUE : FALSE;
}

void CBufferQueue::FreeAllBusyBlock()
{
	BOOL bRet = TRUE;

	do {
		bRet = FreeBusyHead(NULL, 0);
	} while (bRet);
}