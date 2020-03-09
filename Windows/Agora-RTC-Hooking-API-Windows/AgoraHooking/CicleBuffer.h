#pragma once

#define CIC_WAITTIMEOUT		0

class CicleBuffer
{
public:
	CicleBuffer(const unsigned int iBufferSize,int waittimeout);
	~CicleBuffer(void);
	BOOL IsComplete();
	void SetComplete();
	unsigned int getFreeSize();
	unsigned int getUsedSize();
	void writeBuffer(const void* pSourceBuffer, const unsigned int iNumBytes);
	BOOL readBuffer(void* pDestBuffer, const unsigned int iBytesToRead, unsigned int* pbBytesRead);
	void flushBuffer();

private:
	BYTE* m_pBuffer;
	unsigned int m_iBufferSize;
	unsigned int m_iInternalReadCursor;
	unsigned int m_iInternalWriteCursor;
	HANDLE m_evtDataAvailable;
	CRITICAL_SECTION m_csCircleBuffer;
	BOOL m_bComplete;
	int wait_timeout;
};

