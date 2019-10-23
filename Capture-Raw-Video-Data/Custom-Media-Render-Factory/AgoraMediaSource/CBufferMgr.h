#ifndef __CBUFFERMGR_H__
#define __CBUFFERMGR_H__

#include "stdint.h"
#include <assert.h>
#include <synchapi.h>
#include <profileapi.h>
#include <sysinfoapi.h>
#include <timeapi.h>
#include <map>

class CLock
{
public:
	CLock(){
		m_pCriticalSection = new CRITICAL_SECTION;
		assert(m_pCriticalSection);
		InitializeCriticalSection(m_pCriticalSection);
	}
	~CLock(){
		if (m_pCriticalSection){
			DeleteCriticalSection(m_pCriticalSection);
			delete m_pCriticalSection;
			m_pCriticalSection = nullptr;
		}
	}

	void lock(){
		EnterCriticalSection(m_pCriticalSection);
	}

	void unlock(){
		LeaveCriticalSection(m_pCriticalSection);
	}

private:
	LPCRITICAL_SECTION m_pCriticalSection;
};

class CAutoLock
{
public:
	CAutoLock(CLock *pLock) :m_pLock(pLock){
		assert(m_pLock);
		m_pLock->lock();
	}
	~CAutoLock(){
		m_pLock->unlock();
	}

private:
	CLock *m_pLock;
};

class CHighResoluteFrameCtrl
{//Get frame rate in real time. More accurate.
public:
	CHighResoluteFrameCtrl(){
		tickInterval_ = 0;
		frameCount_ = 0;
		counterInterval_.QuadPart = 0;
		lfrequency_.QuadPart = 0;
		counterBegin_.QuadPart = 0;
		counterPiror_.QuadPart = 0;
		counterNext_.QuadPart = 0;
	}

	~CHighResoluteFrameCtrl(){
		tickInterval_ = 0;
		frameCount_ = 0;
		counterInterval_.QuadPart = 0;
		lfrequency_.QuadPart = 0;
		counterBegin_.QuadPart = 0;
		counterPiror_.QuadPart = 0;
		counterNext_.QuadPart = 0;
	}

	void setInterval(unsigned int interval){

		assert(interval && 0 < interval);
		if (tickInterval_ == interval)
		{
			return;
		}
		if (tickInterval_)
		{
			tickInterval_ = 0;
			frameCount_ = 0;
			counterInterval_.QuadPart = 0;
			lfrequency_.QuadPart = 0;
			counterBegin_.QuadPart = 0;
			counterPiror_.QuadPart = 0;
			counterNext_.QuadPart = 0;
		}
		tickInterval_ = interval;

		QueryPerformanceFrequency(&lfrequency_);
		QueryPerformanceCounter(&counterBegin_);
		counterPiror_ = counterBegin_;
		counterInterval_.QuadPart = lfrequency_.QuadPart * tickInterval_ / 1000;
	}

	float rate(){
		LARGE_INTEGER counterCurrent;
		QueryPerformanceCounter(&counterCurrent);
		return float(frameCount_ / ((counterCurrent.QuadPart - counterBegin_.QuadPart) / lfrequency_.QuadPart));
	}

	int wait(){

		if (!counterInterval_.QuadPart)
		{
			printf("please setInterval first!!!!!!\n");
			return false;
		}

		frameCount_++;
		LARGE_INTEGER counterCurrent;
		QueryPerformanceCounter(&counterCurrent);
		LONGLONG counterEscape = counterInterval_.QuadPart * frameCount_ - (counterCurrent.QuadPart - counterBegin_.QuadPart);
		LONGLONG res = counterEscape;

		//TO DO
		while (0 < counterEscape)
		{
			Sleep(1);
			QueryPerformanceCounter(&counterCurrent);
			counterEscape = counterInterval_.QuadPart * frameCount_ - (counterCurrent.QuadPart - counterBegin_.QuadPart);
		}

		return (int)res;
	}

	int getFrameCount(){

		return frameCount_;
	}

private:
	unsigned int tickInterval_;
	int frameCount_;
	LARGE_INTEGER lfrequency_;
	LARGE_INTEGER counterInterval_;
	LARGE_INTEGER counterBegin_;
	LARGE_INTEGER counterPiror_;
	LARGE_INTEGER counterNext_;
};

static long long OS_GetTickCount()
{
#if defined(WIN32)
	return timeGetTime();
#else
	int64_t t0;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	t0 = ((int64_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return t0;
#endif
}

class CArrayBuffer{
public:
	CArrayBuffer(const int &nWidth,const int &nHeight);
	~CArrayBuffer();

	bool push(int nWidth, int nHeight, uint8_t* pBufferIn,const int &nBufferLen);
	bool pop(uint8_t* pBufferOut,int &nBufferLen,int &nWidth,int &nHeight);

	inline bool isFull();
	inline bool isEmpty();

private:

	const static int nKbufferCount = 2;

	uint8_t* m_pBuffer;
	int m_nWidth;
	int m_nHeight;
	int m_nLen;
	int m_nCapacity;
	int m_nByteWrited;
	
	int m_nWritePoint;
	int m_nReadPoint;

	CLock m_lock;
};

class CBufferMgr{
public:
	static CBufferMgr* getInstance();
	static void releaseInstance();
	static void threadProc(LPVOID lpParam);

	~CBufferMgr();

	bool pushYUVBuffer(UINT uid, uint8_t* pBufferIn,int nBuffeLenIn,int nWidth,int nHeight);
	bool popYUVBuffer(UINT uid,uint8_t* pBufferOut,int &nBufferLenOut,int &nWidth,int &nHeight);

protected:
	class CGraba{
	public:
		CGraba(){

		}
		~CGraba(){
			if (m_pBufferMgr){
				CAutoLock al(&m_Lock);
				delete m_pBufferMgr;
				m_pBufferMgr = nullptr;
			}
		}
	};

private:
	CBufferMgr();
	inline BOOL checkBufferMgr();

private:
	static CBufferMgr* m_pBufferMgr;
	static CLock m_Lock;
	static CGraba m_graba;

	//FrameCtrl
	BOOL m_bIsTeminated;

	//BufferMgr
	CLock* m_pMemLock;
	std::map<UINT, CArrayBuffer*> m_mapUserBufferMgr;
	std::map<UINT, LONGLONG > m_mapUserTimeStamp;
};

class CAudioBuffer {
public: 
	CAudioBuffer(int nCount,int nBytePerUnit);
	~CAudioBuffer();
	
private:
	CLock m_lock;

	int		m_nCircleCount;
	int		m_nBytePerUnit;
};

class CAudioBufferManager {
public:

private:

};

#endif