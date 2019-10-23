#include "stdafx.h"
#include "CBufferMgr.h"


CBufferMgr* CBufferMgr::m_pBufferMgr = nullptr;
CLock CBufferMgr::m_Lock;
CBufferMgr::CGraba CBufferMgr::m_graba;

CBufferMgr* CBufferMgr::getInstance()
{
	if (nullptr == m_pBufferMgr){
		CAutoLock al(&m_Lock);
		if (nullptr == m_pBufferMgr)
		m_pBufferMgr = new CBufferMgr;
	}

	return m_pBufferMgr;
}

void CBufferMgr::releaseInstance()
{
	if (nullptr != m_pBufferMgr){
		CAutoLock al(&m_Lock);
		if (nullptr != m_pBufferMgr){

			delete m_pBufferMgr;
			m_pBufferMgr = nullptr;
		}
	}
}

void CBufferMgr::threadProc(LPVOID lpParam){

	CBufferMgr* pBufferMgr = (CBufferMgr*)lpParam;
	CHighResoluteFrameCtrl gsfc;
	gsfc.setInterval(1000);

	while (pBufferMgr && !pBufferMgr->m_bIsTeminated){

		pBufferMgr->checkBufferMgr();
		gsfc.wait();
	}
	
	_endthread();
}

CBufferMgr::CBufferMgr() :m_pMemLock(nullptr), m_bIsTeminated(FALSE)
{
	m_pMemLock = new CLock;

	_beginthread(threadProc,0,(LPVOID)this);
}

CBufferMgr::~CBufferMgr()
{
	m_bIsTeminated = FALSE;
	for (std::map<UINT, CArrayBuffer*>::iterator it = m_mapUserBufferMgr.begin(); m_mapUserBufferMgr.end() != it;){
		delete it->second;
		it->second = nullptr;

		it = m_mapUserBufferMgr.erase(it);
	}
	m_mapUserBufferMgr.clear();
	m_mapUserTimeStamp.clear();

	if (m_pMemLock){

		delete m_pMemLock;
		m_pMemLock = nullptr;
	}
}

bool CBufferMgr::pushYUVBuffer(UINT uid,uint8_t* pBufferIn, int nBuffeLenIn,int nWidth,int nHeight){

	CAutoLock al(m_pMemLock);
	m_bIsTeminated = FALSE;
	std::map<UINT, CArrayBuffer*>::iterator it = m_mapUserBufferMgr.find(uid);
	if (m_mapUserBufferMgr.end() == it){
		CArrayBuffer *pArrayBuffer = new CArrayBuffer(nWidth, nHeight);
		//CAgoraWrapperUtilc::AgoraOutDebugStr(_T("pushYUVBuffer new CArrayBuffer."));
		if (pArrayBuffer){
			m_mapUserBufferMgr[uid] = pArrayBuffer;

			m_mapUserTimeStamp[uid] = OS_GetTickCount();
			return pArrayBuffer->push(nWidth, nHeight, pBufferIn, nBuffeLenIn);
		}
	}

	m_mapUserTimeStamp[uid] = OS_GetTickCount();
	return it->second->push(nWidth, nHeight, pBufferIn, nBuffeLenIn);
}

bool CBufferMgr::popYUVBuffer(UINT uid,uint8_t* pBufferOut, int &nBufferLenOut,int &nWidth,int &nHeight){

	CAutoLock al(m_pMemLock);
	std::map<UINT, CArrayBuffer*>::iterator it = m_mapUserBufferMgr.find(uid);
	if (m_mapUserBufferMgr.end() != it){

		return it->second->pop(pBufferOut, nBufferLenOut,nWidth,nHeight);
	}

	nBufferLenOut = 0;

	return false;
}

inline BOOL CBufferMgr::checkBufferMgr()
{
	//AutoClear
	CAutoLock al(m_pMemLock);
	LONGLONG lNowTimeStamp = OS_GetTickCount();
	for (std::map<UINT, LONGLONG>::iterator it = m_mapUserTimeStamp.begin(); m_mapUserTimeStamp.end() != it;){

		if (1000 < lNowTimeStamp - it->second){
			// 1s does not receive data, indicating that the data has been stopped

			//CAgoraWrapperUtilc::AgoraOutDebugStr(_T("checkBufferMgr delete CArrayBuffer."));
			std::map<UINT, CArrayBuffer*>::iterator itBuffer = m_mapUserBufferMgr.find(it->first);
			assert(m_mapUserBufferMgr.end() != itBuffer);
			if (m_mapUserBufferMgr.end() != itBuffer){
				m_mapUserBufferMgr.erase(itBuffer);
			}

			it = m_mapUserTimeStamp.erase(it);
		}
		else{
			it++;
		}
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CArrayBuffer::CArrayBuffer(const int &nWidth, const int &nHeight):
m_pBuffer(nullptr),
m_nWidth(nWidth),
m_nHeight(nHeight),
m_nLen(0),
m_nCapacity(0),
m_nByteWrited(0){
	m_nLen = m_nWidth * m_nHeight * 3 / 2;
	m_nCapacity = m_nLen * nKbufferCount;
	m_pBuffer = new uint8_t[m_nLen * nKbufferCount];
	ZeroMemory(m_pBuffer, m_nLen * nKbufferCount);
	
	m_nWritePoint = 0;
	m_nReadPoint = 0;
}

CArrayBuffer::~CArrayBuffer(){
	if (m_pBuffer){
		delete[] m_pBuffer;
		m_pBuffer = nullptr;
		m_nLen = 0;
		m_nWidth = 0;
		m_nHeight = 0;
		m_nCapacity = 0;
		m_nByteWrited = 0;

		m_nWritePoint = 0;
		m_nReadPoint = 0;
	}
}

bool CArrayBuffer::push(int nWidth, int nHeight, uint8_t* pBufferIn, const int &nBufferLen){

	assert(m_nWidth == nWidth && m_nHeight == nHeight);
	assert(nBufferLen == m_nLen);

	bool bRes = false;
	if (pBufferIn){

		CAutoLock al(&m_lock);
		memcpy(m_pBuffer + m_nWritePoint, (void*)pBufferIn, nBufferLen);
		if (m_nByteWrited < m_nCapacity) {

			m_nByteWrited += nBufferLen;
			m_nWritePoint += nBufferLen;
			if (m_nWritePoint == m_nCapacity)
				m_nWritePoint = 0;
		}
		else if (m_nByteWrited == m_nCapacity){

			m_nWritePoint += nBufferLen;
			if (m_nWritePoint == m_nCapacity){
				m_nWritePoint = 0;
				m_nReadPoint = 0;
			}
		}

		bRes = true;
	}

	//CAgoraWrapperUtilc::AgoraOutDebugStr(_T(" push %d , nWritePoint : %d, nByteWrited : %d"),nBufferLen,m_nWritePoint,m_nByteWrited);

	return bRes;
}

bool CArrayBuffer::pop(uint8_t* pBufferOut, int &nBufferLen, int &nWidth, int &nHeight){

	bool bRes = false;

	CAutoLock al(&m_lock);
	if (m_nByteWrited){

		bRes = true;
		nBufferLen = m_nLen;
		nWidth = m_nWidth;
		nHeight = m_nHeight;

		memcpy((void*)pBufferOut, m_pBuffer + m_nReadPoint, m_nLen);

		m_nByteWrited -= nBufferLen;
		if (m_nReadPoint == m_nCapacity){
			m_nReadPoint = 0;
		}
	}

	//CAgoraWrapperUtilc::AgoraOutDebugStr(_T(" pop %d , nReadPoint : %d, nByteWrited : %d"), nBufferLen, m_nReadPoint, m_nByteWrited);

	return bRes;
}


inline bool CArrayBuffer::isFull(){

	CAutoLock al(&m_lock);

	return m_nByteWrited == m_nCapacity;
}

inline bool CArrayBuffer::isEmpty(){

	CAutoLock al(&m_lock);

	return m_nByteWrited == 0;
}