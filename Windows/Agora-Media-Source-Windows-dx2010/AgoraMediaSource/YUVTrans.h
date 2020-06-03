#pragma once

/*
attention£ºdo not use this class in the formal production, it has not been tested!

walker05 @agora
*/

class CYUVTrans
{
public:
	CYUVTrans();
	~CYUVTrans();

	static BOOL I420ToRGB24(const LPBYTE lpYUVBuffer, LPBYTE lpBGR24Buffer, SIZE_T nRGBBufSize, int nWidth, int nHeight);

	static BOOL RGB24ToI420(const LPBYTE lpBGR24Buffer, LPBYTE lpYUVBuffer, SIZE_T nYUVBufSize, int nWidth, int nHeight);

	static BOOL YUY2ToI420(const LPBYTE lpYUY2Buffer, LPBYTE lpYUVBuffer, SIZE_T nYUVBufSize, int nWidth, int nHeight);

	static BOOL UYVYToI420(const LPBYTE lpUYVYBuffer, LPBYTE lpYUVBuffer, SIZE_T nYUVBufSize, int nWidth, int nHeight);
private:
	static const int m_nfv1Table[256];
	static const int m_nfv2Table[256];
	static const int m_nfu1Table[256];
	static const int m_nfu2Table[256];
};

