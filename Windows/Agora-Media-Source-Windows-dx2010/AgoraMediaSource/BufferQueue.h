#pragma once

#include <atlcoll.h>

class CBufferQueue
{
public:
	CBufferQueue();
	~CBufferQueue();

	BOOL Create(int nUnitCount, SIZE_T nBytesPreUnit);
	BOOL Close();

	SIZE_T	GetBytesPreUnit() const { return m_nBytesPreUnit; };
	int		GetUnitCount() const { return m_nUnitCount; };
	int		GetCurrentCount() const { return m_nCurrentCount; };

	int		GetFreeCount() const;
	int		GetBusyCount() const;

	LPVOID	AllocBuffer(BOOL bForceAlloc = FALSE);

	LPVOID	GetBusyHead();

	BOOL	FreeBusyHead();
	void	FreeAllBusyBlock();

private:
	int			m_nUnitCount;		// 单元初始值
	int			m_nCurrentCount;	// 当前块数目
	SIZE_T		m_nBytesPreUnit;

	CAtlList<LPBYTE>	m_listFreeUnit;	// 闲置单元
	CRITICAL_SECTION	m_csAllocLock;	// 内存块分配锁
	CRITICAL_SECTION	m_csFreeLock;	// 内存块释放锁

	CAtlList<LPBYTE>	m_listBusyUnit;	// 非闲置单元
	CRITICAL_SECTION	m_csBusyLock;	// 忙表锁
};

