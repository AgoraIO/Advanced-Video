#pragma once
#include<stdio.h>
#include<tchar.h>
#include<d3d9.h>
class ZD3DRender
{
public:
	ZD3DRender();
	~ZD3DRender();
public:
	//根据窗口句柄初始化
	int Init(HWND hwnd,unsigned int nWidth,unsigned int nHeight,bool isYuv,int src_width,int src_height);
	void Cleanup();
	bool Render(char *buffer);
private:
	bool m_bIsYuv;
	int m_nWidth;
	int m_nHeight;

	RECT m_rtViewPort;
	CRITICAL_SECTION   m_ctitical;
	IDirect3D9    *m_pDirect3D9;
	IDirect3DDevice9 *m_pDirect3DDevice;
	IDirect3DSurface9 *m_pDirect3DSurfaceRender;
	void calculate_display_rect(RECT &rect,
		int scr_xleft, int scr_ytop, int scr_width, int scr_height,
		int pic_width, int pic_height);
};

