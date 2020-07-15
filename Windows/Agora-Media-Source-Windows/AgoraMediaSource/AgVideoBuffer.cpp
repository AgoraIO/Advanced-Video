#include "stdafx.h"
#include "AgVideoBuffer.h"
#include <mutex>

std::mutex buf_mutex;
BYTE CAgVideoBuffer::videoBuffer[VIDEO_BUF_SIZE] = { 0 };

CAgVideoBuffer* CAgVideoBuffer::GetInstance()
{
    static CAgVideoBuffer agVideoBuffer;
    return &agVideoBuffer;
}

CAgVideoBuffer::CAgVideoBuffer()
{

}

CAgVideoBuffer::~CAgVideoBuffer()
{

}


void CAgVideoBuffer::SetVideoFormat(const BITMAPINFOHEADER *lpInfoHeader)
{
    memcpy_s(&m_bmiHeader, sizeof(BITMAPINFOHEADER), lpInfoHeader, sizeof(BITMAPINFOHEADER));
    m_nPackageSize = m_bmiHeader.biWidth*m_bmiHeader.biWidth * 3 / 2;
    _ASSERT(m_nPackageSize <= VIDEO_BUF_SIZE);
}

void CAgVideoBuffer::GetVideoFormat(BITMAPINFOHEADER *lpInfoHeader)
{
    memcpy_s(lpInfoHeader, sizeof(BITMAPINFOHEADER), &m_bmiHeader, sizeof(BITMAPINFOHEADER));
}

bool CAgVideoBuffer::writeBuffer(BYTE* buffer, int bufsize, int ts)
{
    if (bufsize < m_nPackageSize)
        return false;
    std::lock_guard<std::mutex> buf_lock(buf_mutex);
    memcpy_s(videoBuffer, bufsize, buffer, bufsize);
    timestamp = ts;
    return true;
}
bool CAgVideoBuffer::readBuffer(BYTE* buffer, int bufsize, int& ts)
{
    if (bufsize < m_nPackageSize)
        return false;
    std::lock_guard<std::mutex> buf_lock(buf_mutex);
    memcpy_s(buffer, bufsize, videoBuffer, bufsize);
    ts = timestamp;
    return true;
}

void CAgVideoBuffer::resetVideoBuffer(int width, int height)
{
    int size = width * height;
    memset(videoBuffer, 0, size);
    memset(videoBuffer + size, 128, size / 4);
    memset(videoBuffer + size * 5 / 4, 128, size / 4);
}