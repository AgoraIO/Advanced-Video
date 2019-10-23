#ifndef __RENDERGDI_H__
#define __RENDERGDI_H__

#include <memory>
#include <thread>
#include <atlimage.h>

class CRenderGDI {
public:
	CRenderGDI();
	CRenderGDI(void* pHandle, int nWidth, int nHeight);
	~CRenderGDI();

	void enableTipInfo(bool bEnable) { m_bEnableTipInfo = bEnable; }
	bool addtext(const TCHAR* pStr, int nXpos, int nYpos, int nX, int nY);
	bool addlogo(unsigned char* pLogoBuffer,int nXpos,int nYpos,int nWidthSrc,int nHeightSrc);
	bool showvideo(void* pHandle,unsigned char* pVideoBuffer, int nBufferLen, int nWidth, int nHeight);

	bool loadBKImage(const TCHAR* pStrPath);
	bool unloadBKImage();
	
protected:
	void ThreadRender();

private:
	
	int		m_nWidth;
	int		m_nHeight;
	std::unique_ptr<unsigned char> m_videoBuffer;
	std::thread		m_threadRender;
	bool				m_bThreadRuning;
	void*				m_wnd;
	int					m_nFps;
	int					m_nFrameCount;
	DWORD		m_dLasttps;

	bool				m_bEnableTipInfo;
	TCHAR			m_addTxt[256];
	RECT			m_addTxtRt;
	std::unique_ptr<BYTE>	m_logoBuffer;
	RECT			m_addLogoRt;

	ATL::CImage* m_pImageBk;
};

#endif