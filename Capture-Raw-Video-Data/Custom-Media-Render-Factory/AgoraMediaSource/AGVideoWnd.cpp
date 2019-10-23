// AGVideoWnd.cpp :
//

#include "stdafx.h"
#include "AgoraMediaSource.h"
#include "AGVideoWnd.h"
#include "YUVTrans.h"
#include "CBufferMgr.h"
#include <memory>

IMPLEMENT_DYNAMIC(CAGInfoWnd, CWnd)

CAGInfoWnd::CAGInfoWnd()
: m_bShowTip(TRUE)
, m_nWidth(0)
, m_nHeight(0)
, m_nFps(0)
, m_nBitrate(0)
{
	m_brBack.CreateSolidBrush(RGB(0x00, 0xA0, 0xE9));
}

CAGInfoWnd::~CAGInfoWnd()
{
	m_brBack.DeleteObject();
}


BEGIN_MESSAGE_MAP(CAGInfoWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CAGInfoWnd::ShowTips(BOOL bShow)
{
	m_bShowTip = bShow;

	if (bShow)
		ShowWindow(SW_SHOW);
	else 
		ShowWindow(SW_HIDE);

	Invalidate(TRUE);
}

void CAGInfoWnd::SetVideoResolution(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	if (m_bShowTip) {
		Invalidate(TRUE);
		UpdateWindow();
	}
}

void CAGInfoWnd::SetFrameRateInfo(int nFPS)
{
	m_nFps = nFPS;

	if (m_bShowTip) {
		Invalidate(TRUE);
		UpdateWindow();
	}
}

void CAGInfoWnd::SetBitrateInfo(int nBitrate)
{
	m_nBitrate = nBitrate;

	if (m_bShowTip) {
		Invalidate(TRUE);
		UpdateWindow();
	}
}

void CAGInfoWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect	rcClient;
	CString strTip;

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
	
	if (m_bShowTip) {
		// 640x480,15fps,400k
		GetClientRect(&rcClient);
		rcClient.top += 4;
		strTip.Format(_T("%dx%d, %dfps, %dK"), m_nWidth, m_nHeight, m_nFps, m_nBitrate);
		dc.DrawText(strTip, &rcClient, DT_VCENTER | DT_CENTER);
	}
}

BOOL CAGInfoWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;

	GetClientRect(&rcClient);
	pDC->FillRect(&rcClient, &m_brBack);
	
	return TRUE;
}

// CAGVideoWnd

IMPLEMENT_DYNAMIC(CAGVideoWnd, CWnd)

CAGVideoWnd::CAGVideoWnd()
: m_nUID(0)
, m_crBackColor(RGB(0x58, 0x58, 0x58))
, m_bShowVideoInfo(FALSE)
, m_bBigShow(FALSE)
, m_bBackground(FALSE)
{
	m_bufferFrame.reset(new BYTE[1920 * 1080 * 3 / 2]);
	ZeroMemory(m_bufferFrame.get(), 1920*1080* 3/2);
}

CAGVideoWnd::~CAGVideoWnd()
{
	m_imgBackGround.DeleteImageList();
	EnableRender(FALSE);
}

BEGIN_MESSAGE_MAP(CAGVideoWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
    ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CAGVideoWnd::OnEraseBkgnd(CDC* pDC)
{
	CRect		rcClient;
	CPoint		ptDraw;
	IMAGEINFO	imgInfo;

	GetClientRect(&rcClient);

	pDC->FillSolidRect(&rcClient, m_crBackColor);
	if (!m_imgBackGround.GetImageInfo(0, &imgInfo))
		return FALSE;

	ptDraw.SetPoint((rcClient.Width() - imgInfo.rcImage.right) / 2, (rcClient.Height() - imgInfo.rcImage.bottom) / 2);
	if (ptDraw.x < 0)
		ptDraw.x = 0;
	if (ptDraw.y <= 0)
		ptDraw.y = 0;

	m_imgBackGround.Draw(pDC, 0, ptDraw, ILD_NORMAL);
	return FALSE;
}

void CAGVideoWnd::SetUID(UINT nUID)
{
	m_nUID = nUID;
}

UINT CAGVideoWnd::GetUID()
{
	return m_nUID;
}

BOOL CAGVideoWnd::IsWndFree()
{
	return m_nUID == 0 ? TRUE : FALSE;
}

BOOL CAGVideoWnd::SetBackImage(UINT nID, UINT nWidth, UINT nHeight, COLORREF crMask)
{
	CBitmap bmBackImage;

	if (!bmBackImage.LoadBitmap(nID))
		return FALSE;

	m_imgBackGround.DeleteImageList();

	m_imgBackGround.Create(nWidth, nHeight, ILC_COLOR24 | ILC_MASK, 1, 1);
	m_imgBackGround.Add(&bmBackImage, crMask);
	bmBackImage.DeleteObject();

	Invalidate(FALSE);

	return TRUE;
}

void CAGVideoWnd::ShowBackground(BOOL bBackground)
{
    m_bBackground = bBackground;

    Invalidate(FALSE);
}

void CAGVideoWnd::SetFaceColor(COLORREF crBackColor)
{
	m_crBackColor = crBackColor;

	Invalidate(FALSE);
}

void CAGVideoWnd::SetVideoResolution(UINT nWidth, UINT nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

void CAGVideoWnd::GetVideoResolution(UINT *nWidth, UINT *nHeight)
{
	*nWidth = m_nWidth;
	*nHeight = m_nHeight;
}

void CAGVideoWnd::SetBitrateInfo(int nReceivedBitrate)
{
	m_nBitRate = nReceivedBitrate;
}

void CAGVideoWnd::SetFrameRateInfo(int nReceiveFrameRate)
{
	m_nFrameRate = nReceiveFrameRate;
}

void CAGVideoWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWBIG, (WPARAM)this, (LPARAM)m_nUID);

	CWnd::OnLButtonDown(nFlags, point);
}

void CAGVideoWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWMODECHANGED, (WPARAM)this, (LPARAM)m_nUID);

	CWnd::OnRButtonDown(nFlags, point);
}

int CAGVideoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CAGVideoWnd::ShowVideoInfo(BOOL bShow)
{
	m_bShowVideoInfo = bShow;

	Invalidate(FALSE);
}


void CAGVideoWnd::SetBigShowFlag(BOOL bBigShow)
{
	CRect	rcClient;

	m_bBigShow = bBigShow;
	GetClientRect(&rcClient);

	int x = (rcClient.Width()- 192) / 2;
	int y = rcClient.Height() - 24;
};

void CAGVideoWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	int x = (cx - 192) / 2;
	int y = cy - 24;
}

void CAGVideoWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWMODECHANGED, (WPARAM)this, (LPARAM)m_nUID);

	CWnd::OnLButtonDblClk(nFlags, point);
}

void CAGVideoWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting

    if (m_bBackground) {
        CRect		rcClient;
        CPoint		ptDraw;
        IMAGEINFO	imgInfo;

        GetClientRect(&rcClient);

        dc.FillSolidRect(&rcClient, m_crBackColor);
        if (!m_imgBackGround.GetImageInfo(0, &imgInfo))
            return;

        ptDraw.SetPoint((rcClient.Width() - imgInfo.rcImage.right) / 2, (rcClient.Height() - imgInfo.rcImage.bottom) / 2);
        if (ptDraw.x < 0)
            ptDraw.x = 0;
        if (ptDraw.y <= 0)
            ptDraw.y = 0;

        m_imgBackGround.Draw(&dc, 0, ptDraw, ILD_NORMAL);
    }
    else
        return CWnd::OnPaint();
}

void CAGVideoWnd::threadRender()
{	
	int nBuffllenYuv = 0, nWidthYuv = 0, nHeightYuv = 0;
	std::shared_ptr<BYTE> pBufferRGB24;
	std::shared_ptr<BYTE> pBufferRGB24_1;
	int nBufferLen = 0;

	m_gdirender.enableTipInfo(true);

	while (m_bRunning) {
		if (nullptr == m_hWnd)
			continue;

		WCHAR struid[30] = { 0 };
		swprintf_s(struid, L"%ld", m_nUID);
		m_gdirender.addtext(struid, 0, 0, 100, 30);

		std::this_thread::sleep_for(std::chrono::milliseconds(40));
		long lViewId = (long)m_hWnd;
		char szbuf1[256] = { 0 };
		sprintf_s(szbuf1, "pop buffer view: %p,viewid: %ld\n", m_hWnd,lViewId);
		OutputDebugStringA(szbuf1);

		if (CBufferMgr::getInstance()->popYUVBuffer(lViewId,m_bufferFrame.get(),nBuffllenYuv,nWidthYuv,nHeightYuv) ) {
			if (m_nWidth != nWidthYuv || m_nHeight != nHeightYuv) {
				m_nWidth = nWidthYuv; m_nHeight = nHeightYuv;
			}

			nBufferLen = nBuffllenYuv;
			if (nullptr == pBufferRGB24.get()  || nullptr == pBufferRGB24_1.get()) {
				pBufferRGB24.reset(new BYTE[m_nWidth * m_nHeight * 3]);
				ZeroMemory(pBufferRGB24.get(), m_nWidth * m_nHeight * 3);
				pBufferRGB24_1.reset(new BYTE[m_nWidth * m_nHeight * 3]);
				ZeroMemory(pBufferRGB24_1.get(), m_nWidth * m_nHeight * 3);
			}
			
			CYUVTrans::I420ToRGB24(m_bufferFrame.get(), pBufferRGB24_1.get(),nBufferLen * 2,m_nWidth,m_nHeight);
			CYUVTrans::VerMirror_RGB24(pBufferRGB24_1.get(), pBufferRGB24.get(), m_nWidth, m_nHeight);
			m_gdirender.showvideo(m_hWnd, pBufferRGB24.get(), nBufferLen * 2, m_nWidth, m_nHeight);
		}
	}
}

BOOL CAGVideoWnd::EnableRender(BOOL bEnable)
{
	if (bEnable) {
		if (!m_bRunning ) {
			m_bRunning = true;
			m_threadRender = std::thread(&CAGVideoWnd::threadRender, this);
			m_threadRender.detach();
		}
	}
	else
		m_bRunning = false;

	return TRUE;
}