#pragma once

/*
attention��do not use this class in the formal production, it has not been tested!

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

	static BOOL CONVERT_ENDIAN_RGB24(LPBYTE const lpRGB24Buffer,int nWidth,int nHeight);

	static BOOL CONVERT_BIG_RGB24(LPBYTE lpRGB24Buffer, int nWidth, int nHeight);

	static void VerMirror_RGB24(BYTE* pSrc, BYTE* pDest, int nWidth, int nHeight);

	static void GetRGB24FromPic(const char* pPicPath, LPBYTE lpRGB24Buffer, int &nRGBBufSize, int &nWidth, int &nHeight);

private:
	static const int m_nfv1Table[256];
	static const int m_nfv2Table[256];
	static const int m_nfu1Table[256];
	static const int m_nfu2Table[256];
};

