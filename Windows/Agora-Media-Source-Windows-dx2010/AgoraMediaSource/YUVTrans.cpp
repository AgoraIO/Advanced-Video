#include "stdafx.h"
#include "YUVTrans.h"

const int CYUVTrans::m_nfv1Table[256] = { -180, -179, -177, -176, -174, -173, -172, -170, -169, -167, -166, -165, -163, -162, -160, -159, -158, -156, -155, -153, -152, -151, -149, -148, -146, -145, -144, -142, -141, -139, -138, -137, -135, -134, -132, -131, -130, -128, -127, -125, -124, -123, -121, -120, -118, -117, -115, -114, -113, -111, -110, -108, -107, -106, -104, -103, -101, -100, -99, -97, -96, -94, -93, -92, -90, -89, -87, -86, -85, -83, -82, -80, -79, -78, -76, -75, -73, -72, -71, -69, -68, -66, -65, -64, -62, -61, -59, -58, -57, -55, -54, -52, -51, -50, -48, -47, -45, -44, -43, -41, -40, -38, -37, -36, -34, -33, -31, -30, -29, -27, -26, -24, -23, -22, -20, -19, -17, -16, -15, -13, -12, -10, -9, -8, -6, -5, -3, -2, 0, 1, 2, 4, 5, 7, 8, 9, 11, 12, 14, 15, 16, 18, 19, 21, 22, 23, 25, 26, 28, 29, 30, 32, 33, 35, 36, 37, 39, 40, 42, 43, 44, 46, 47, 49, 50, 51, 53, 54, 56, 57, 58, 60, 61, 63, 64, 65, 67, 68, 70, 71, 72, 74, 75, 77, 78, 79, 81, 82, 84, 85, 86, 88, 89, 91, 92, 93, 95, 96, 98, 99, 100, 102, 103, 105, 106, 107, 109, 110, 112, 113, 114, 116, 117, 119, 120, 122, 123, 124, 126, 127, 129, 130, 131, 133, 134, 136, 137, 138, 140, 141, 143, 144, 145, 147, 148, 150, 151, 152, 154, 155, 157, 158, 159, 161, 162, 164, 165, 166, 168, 169, 171, 172, 173, 175, 176, 178 };
const int CYUVTrans::m_nfv2Table[256] = { -92, -91, -91, -90, -89, -88, -88, -87, -86, -86, -85, -84, -83, -83, -82, -81, -81, -80, -79, -78, -78, -77, -76, -76, -75, -74, -73, -73, -72, -71, -71, -70, -69, -68, -68, -67, -66, -66, -65, -64, -63, -63, -62, -61, -61, -60, -59, -58, -58, -57, -56, -56, -55, -54, -53, -53, -52, -51, -51, -50, -49, -48, -48, -47, -46, -46, -45, -44, -43, -43, -42, -41, -41, -40, -39, -38, -38, -37, -36, -36, -35, -34, -33, -33, -32, -31, -31, -30, -29, -28, -28, -27, -26, -26, -25, -24, -23, -23, -22, -21, -21, -20, -19, -18, -18, -17, -16, -16, -15, -14, -13, -13, -12, -11, -11, -10, -9, -8, -8, -7, -6, -6, -5, -4, -3, -3, -2, -1, 0, 0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 22, 23, 24, 25, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 32, 33, 34, 35, 35, 36, 37, 37, 38, 39, 40, 40, 41, 42, 42, 43, 44, 45, 45, 46, 47, 47, 48, 49, 50, 50, 51, 52, 52, 53, 54, 55, 55, 56, 57, 57, 58, 59, 60, 60, 61, 62, 62, 63, 64, 65, 65, 66, 67, 67, 68, 69, 70, 70, 71, 72, 72, 73, 74, 75, 75, 76, 77, 77, 78, 79, 80, 80, 81, 82, 82, 83, 84, 85, 85, 86, 87, 87, 88, 89, 90, 90 };
const int CYUVTrans::m_nfu1Table[256] = { -44, -44, -44, -43, -43, -43, -42, -42, -42, -41, -41, -41, -40, -40, -40, -39, -39, -39, -38, -38, -38, -37, -37, -37, -36, -36, -36, -35, -35, -35, -34, -34, -33, -33, -33, -32, -32, -32, -31, -31, -31, -30, -30, -30, -29, -29, -29, -28, -28, -28, -27, -27, -27, -26, -26, -26, -25, -25, -25, -24, -24, -24, -23, -23, -22, -22, -22, -21, -21, -21, -20, -20, -20, -19, -19, -19, -18, -18, -18, -17, -17, -17, -16, -16, -16, -15, -15, -15, -14, -14, -14, -13, -13, -13, -12, -12, -11, -11, -11, -10, -10, -10, -9, -9, -9, -8, -8, -8, -7, -7, -7, -6, -6, -6, -5, -5, -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -1, -1, 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 13, 14, 14, 14, 15, 15, 15, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 25, 26, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 31, 32, 32, 33, 33, 33, 34, 34, 34, 35, 35, 35, 36, 36, 36, 37, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 42, 43, 43 };
const int CYUVTrans::m_nfu2Table[256] = { -227, -226, -224, -222, -220, -219, -217, -215, -213, -212, -210, -208, -206, -204, -203, -201, -199, -197, -196, -194, -192, -190, -188, -187, -185, -183, -181, -180, -178, -176, -174, -173, -171, -169, -167, -165, -164, -162, -160, -158, -157, -155, -153, -151, -149, -148, -146, -144, -142, -141, -139, -137, -135, -134, -132, -130, -128, -126, -125, -123, -121, -119, -118, -116, -114, -112, -110, -109, -107, -105, -103, -102, -100, -98, -96, -94, -93, -91, -89, -87, -86, -84, -82, -80, -79, -77, -75, -73, -71, -70, -68, -66, -64, -63, -61, -59, -57, -55, -54, -52, -50, -48, -47, -45, -43, -41, -40, -38, -36, -34, -32, -31, -29, -27, -25, -24, -22, -20, -18, -16, -15, -13, -11, -9, -8, -6, -4, -2, 0, 1, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 21, 23, 24, 26, 28, 30, 31, 33, 35, 37, 39, 40, 42, 44, 46, 47, 49, 51, 53, 54, 56, 58, 60, 62, 63, 65, 67, 69, 70, 72, 74, 76, 78, 79, 81, 83, 85, 86, 88, 90, 92, 93, 95, 97, 99, 101, 102, 104, 106, 108, 109, 111, 113, 115, 117, 118, 120, 122, 124, 125, 127, 129, 131, 133, 134, 136, 138, 140, 141, 143, 145, 147, 148, 150, 152, 154, 156, 157, 159, 161, 163, 164, 166, 168, 170, 172, 173, 175, 177, 179, 180, 182, 184, 186, 187, 189, 191, 193, 195, 196, 198, 200, 202, 203, 205, 207, 209, 211, 212, 214, 216, 218, 219, 221, 223, 225 };

CYUVTrans::CYUVTrans()
{
}


CYUVTrans::~CYUVTrans()
{
}


BOOL CYUVTrans::I420ToRGB24(const LPBYTE lpYUVBuffer, LPBYTE lpBGR24Buffer, SIZE_T nRGBBufSize, int nWidth, int nHeight)
{
	if (nRGBBufSize < nWidth*nHeight * 3)
		return FALSE;

	DWORD dwPixelSize = nWidth * nHeight;
	unsigned char* yData = const_cast<unsigned char*>(lpYUVBuffer);
	unsigned char* vData = &yData[dwPixelSize];
	unsigned char* uData = &vData[dwPixelSize >> 2];

	int bgr[3];
	int yIdx, uIdx, vIdx, idx;
	int rdif, invgdif, bdif;
	for (int i = 0; i < nHeight; i++){
		for (int j = 0; j < nWidth; j++){
			yIdx = i * nWidth + j;
			vIdx = (i / 2) * (nWidth / 2) + (j / 2);
			uIdx = vIdx;

			rdif = m_nfv1Table[vData[vIdx]];
			invgdif = m_nfu1Table[uData[uIdx]] + m_nfv2Table[vData[vIdx]];
			bdif = m_nfu2Table[uData[uIdx]];

			/* YUV420 to BGR24 */
			bgr[0] = yData[yIdx] + rdif;       //R
			bgr[1] = yData[yIdx] - invgdif;    //G
			bgr[2] = yData[yIdx] + bdif;       //B 

			for (int k = 0; k < 3; k++){
				idx = (i * nWidth + j) * 3 + k;
				if (bgr[k] >= 0 && bgr[k] <= 255)
					lpBGR24Buffer[idx] = bgr[k];
				else
					lpBGR24Buffer[idx] = (bgr[k] < 0) ? 0 : 255;
			}
		}
	}

	return TRUE;
}

BOOL CYUVTrans::RGB24ToI420(const LPBYTE lpBGR24Buffer, LPBYTE lpYUVBuffer, SIZE_T nYUVBufSize, int nWidth, int nHeight)
{
	if (nYUVBufSize < nWidth*nHeight * 3 / 2)
		return FALSE;

	LPBYTE lpBufRGB = NULL;
	LPBYTE lpBufYUV = NULL;

	LPBYTE lpBufY = lpYUVBuffer;
	LPBYTE lpBufV = lpYUVBuffer + nWidth * nHeight;
	LPBYTE lpBufU = lpBufV + (nWidth * nHeight * 1 / 4);
	
	unsigned int ylen = nWidth * nHeight;
	unsigned int ulen = (nWidth * nHeight) / 4;
	unsigned int vlen = (nWidth * nHeight) / 4;

	for (int j = 0; j <nHeight; j++) {
		LPBYTE lpBufRGB = lpBGR24Buffer + nWidth * (nHeight - 1 - j) * 3;
	
		for (int i = 0; i < nWidth; i++) {
			BYTE r = *(lpBufRGB++);
			BYTE g = *(lpBufRGB++);
			BYTE b = *(lpBufRGB++);
			BYTE y = (BYTE)((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
			BYTE u = (BYTE)((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
			BYTE v = (BYTE)((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
			
			*(lpBufY++) = max(0, min(y, 255));	// Y
			
			if (j % 2 == 0 && i % 2 == 0) {
				if (u>255)
					u = 255;
		
				if (u<0)
					u = 0;
				
				*(lpBufU++) = u;	// U
				
			}
			else {  
				if (i % 2 == 0) {
					if (v>255)
						v = 255;

					if (v<0)
						v = 0;
				
					*(lpBufV++) = v;	// V
				}
			}
		}
	}

	return TRUE;
}

BOOL CYUVTrans::YUY2ToI420(const LPBYTE lpYUY2Buffer, LPBYTE lpYUVBuffer, SIZE_T nYUVBufSize, int nWidth, int nHeight)
{
	if (nYUVBufSize < nWidth*nHeight * 3 / 2)
		return FALSE;

	BYTE *lpUBuf = lpYUVBuffer + (nWidth * nHeight);
	BYTE *lpVBuf = lpUBuf + (nWidth * nHeight) / 4;

	for (int i = 0; i < nHeight / 2; i++)
	{

		BYTE *lpYUY2Bufl1 = lpYUY2Buffer + nWidth * 2 * 2 * i;
		BYTE *lpYUY2Bufl2 = lpYUY2Bufl1 + nWidth * 2;
		
		BYTE *lpYUVBufl1 = lpYUVBuffer + nWidth * 2 * i;
		BYTE *lpYUVBufl2 = lpYUVBufl1 + nWidth;
		
		for (int j = 0; j < nWidth / 2; j++)
		{
			*lpYUVBufl1++ = lpYUY2Bufl1[0];
			*lpUBuf++ = lpYUY2Bufl1[1];
			*lpYUVBufl1++ = lpYUY2Bufl1[2];
			*lpVBuf++ = lpYUY2Bufl1[3];
			*lpYUVBufl2++ = lpYUY2Bufl2[0];
			*lpYUVBufl2++ = lpYUY2Bufl2[2];
			lpYUY2Bufl1 += 4;
			lpYUY2Bufl2 += 4;
		}
	}

	return TRUE;
}

BOOL CYUVTrans::UYVYToI420(const LPBYTE lpUYVYBuffer, LPBYTE lpYUVBuffer, SIZE_T nYUVBufSize, int nWidth, int nHeight)
{
	if (nYUVBufSize < nWidth*nHeight * 3 / 2)
		return FALSE;

	BYTE *lpYBuf = lpYUVBuffer;
	BYTE *lpUBuf = lpYUVBuffer + (nWidth * nHeight);
	BYTE *lpVBuf = lpUBuf + (nWidth * nHeight) / 4;

	BYTE *lpBufUYVY = lpUYVYBuffer;

	for (int i = 0; i < nHeight; i += 1) {

		for (int j = 0; j < nWidth; j += 2) {

			lpYBuf[i*nWidth + j] = lpBufUYVY[1];
			lpYBuf[i*nWidth + j + 1] = lpBufUYVY[3];

			lpUBuf[i*nWidth / 4 + j / 2] = lpBufUYVY[0];
			lpVBuf[i*nWidth / 4 + j / 2] = lpBufUYVY[2];

			lpBufUYVY += 4;
		}
	}

	return TRUE;
}
