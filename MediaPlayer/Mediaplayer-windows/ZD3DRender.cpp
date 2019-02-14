#include "ZD3DRender.h"
#include <math.h>
#define FFMAX(a,b) ((a) > (b) ? (a) : (b))


ZD3DRender::ZD3DRender()
{
	m_pDirect3D9 = NULL;
	m_pDirect3DDevice = NULL;
	m_pDirect3DSurfaceRender = NULL;
}


ZD3DRender::~ZD3DRender()
{
	Cleanup();
}
int ZD3DRender::Init(HWND hwnd, unsigned int nWidth, unsigned int nHeight, bool isYuv,int src_width,int src_height) {
	HRESULT lRet;
	InitializeCriticalSection(&m_ctitical);
	Cleanup();
	m_pDirect3D9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pDirect3D9 == NULL)
	{
		return -1;
	}
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = true;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	
	GetClientRect(hwnd,&m_rtViewPort);
	//计算视频缩放比率
	calculate_display_rect(m_rtViewPort,0,0,src_width,src_height,nWidth,nHeight);

	lRet = m_pDirect3D9->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hwnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&m_pDirect3DDevice);
	if (FAILED(lRet)) {
		return -1;
	}
	if (isYuv)
	{
		lRet = m_pDirect3DDevice->CreateOffscreenPlainSurface(nWidth,nHeight, (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'),D3DPOOL_DEFAULT,&m_pDirect3DSurfaceRender,NULL);
		if (FAILED(lRet))
			return -1;
	}
	else {
		lRet = m_pDirect3DDevice->CreateOffscreenPlainSurface(nWidth,nHeight,D3DFMT_X8B8G8R8,D3DPOOL_DEFAULT,&m_pDirect3DSurfaceRender,NULL);
	}
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_bIsYuv = isYuv;
	return 0;
}
void ZD3DRender::calculate_display_rect(RECT &rect,
	int scr_xleft, int scr_ytop, int scr_width, int scr_height,
	int pic_width, int pic_height)
{
	float aspect_ratio = 0;
	int width, height, x, y;

	if (aspect_ratio <= 0.0)
		aspect_ratio = 1.0;

	//保持屏幕纵横比的算法
	//获取源视频的屏占比 再将屏占比 设置到需要渲染的view上去
	aspect_ratio *= (float)pic_width / (float)pic_height;

	/* XXX: we suppose the screen has a 1.0 pixel ratio */
	height = scr_height;
	width = lrint(height * aspect_ratio) & ~1;
	if (width > scr_width) {
		width = scr_width;
		height = lrint(width / aspect_ratio) & ~1;
	}
	x = (scr_width - width) / 2;
	y = (scr_height - height) / 2;
	rect.left = scr_xleft + x;
	rect.top = scr_ytop + y;
	//surface 没有长和宽的属性 所以需要加上对应的偏移量计算 得出最后显示宽度和高度
	rect.right = FFMAX(width, 1) + x ;
	rect.bottom = FFMAX(height, 1) + y;//
}
void ZD3DRender::Cleanup() {
	EnterCriticalSection(&m_ctitical);
	if (m_pDirect3DSurfaceRender) {
		m_pDirect3DSurfaceRender->Release();
	}
	if (m_pDirect3DDevice)
	{
		m_pDirect3DDevice->Release();
	}
	if (m_pDirect3D9) {
		m_pDirect3D9->Release();
	}
	LeaveCriticalSection(&m_ctitical);

}
bool ZD3DRender::Render(char *buffer){
	if (!m_pDirect3DSurfaceRender || !buffer)
		return false;
	HRESULT lRet;
	D3DLOCKED_RECT d3d_rect;
	lRet = m_pDirect3DSurfaceRender->LockRect(&d3d_rect,NULL,D3DLOCK_DONOTWAIT);
	if (FAILED(lRet))
	{
		return false;
	}
	byte *pSrc = (byte *)buffer;
	byte *pDest = (byte *)d3d_rect.pBits;
	int stride = d3d_rect.Pitch;
	unsigned long i = 0;
	if (m_bIsYuv)
	{
		for ( i = 0; i < m_nHeight; i++)
		{
			memcpy(pDest+i * stride,pSrc+ i * m_nWidth,m_nWidth);
		}
		for ( i = 0; i < m_nHeight/2; i++)
		{
			memcpy(pDest + stride * m_nHeight + i * stride / 2, pSrc + m_nWidth * m_nHeight * 5 / 4 + i * m_nWidth / 2,m_nWidth/2);
		}
		for ( i = 0; i < m_nHeight/2; i++)
		{
			memcpy(pDest + stride * m_nHeight + stride * m_nHeight / 4 + i* stride / 2, pSrc + m_nWidth * m_nHeight + i *m_nWidth / 2, m_nWidth / 2);

		}
	}
	else {
		int pixel_size = m_nWidth * 4;
		for ( i = 0; i < m_nHeight; i++)
		{
			memcpy(pDest,pSrc,pixel_size);
			pDest += stride;
			pSrc += pixel_size;
		}
	}
	lRet = m_pDirect3DSurfaceRender->UnlockRect();
	if (FAILED(lRet)) {
		return false;
	}
	if (m_pDirect3DDevice == NULL) {
		return false;
	}
	m_pDirect3DDevice->Clear(0,NULL,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0),1.0f,0);
	m_pDirect3DDevice->BeginScene();
	IDirect3DSurface9 *pBackBuffer = NULL;
	m_pDirect3DDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
	m_pDirect3DDevice->StretchRect(m_pDirect3DSurfaceRender,NULL,pBackBuffer,&m_rtViewPort,D3DTEXF_LINEAR);
	m_pDirect3DDevice->EndScene();
	m_pDirect3DDevice->Present(NULL, NULL, NULL, NULL);
	pBackBuffer->Release();
	return true;
}




