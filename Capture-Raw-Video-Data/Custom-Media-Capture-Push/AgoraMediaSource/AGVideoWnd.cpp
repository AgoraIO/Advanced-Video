// AGVideoWnd.cpp :
//

#include "stdafx.h"
#include "AgoraMediaSource.h"
#include "AGVideoWnd.h"
#include "YUVTrans.h"
#include "VideoPackageQueue.h"
#include "AudioCapturePackageQueue.h"

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

	Invalidate(FALSE);
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

}

CAGVideoWnd::~CAGVideoWnd()
{
	m_imgBackGround.DeleteImageList();
	EnableLocalRender(FALSE);
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
		return TRUE;

	ptDraw.SetPoint((rcClient.Width() - imgInfo.rcImage.right) / 2, (rcClient.Height() - imgInfo.rcImage.bottom) / 2);
	if (ptDraw.x < 0)
		ptDraw.x = 0;
	if (ptDraw.y <= 0)
		ptDraw.y = 0;

	m_imgBackGround.Draw(pDC, 0, ptDraw, ILD_NORMAL);
	return TRUE;
}

void CAGVideoWnd::SetUID(UINT nUID)
{
	m_nUID = nUID;

	if (m_nUID == 0)
		m_wndInfo.ShowWindow(SW_HIDE);
	else
		m_wndInfo.ShowWindow(SW_SHOW);
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

	m_wndInfo.SetVideoResolution(nWidth, nHeight);
	if (nWidth && nHeight) {
		EnableLocalRender(TRUE);
		m_bufferFrame = std::shared_ptr<BYTE>(new BYTE[nWidth * nHeight * 3 / 2]);
	}
}

void CAGVideoWnd::GetVideoResolution(UINT *nWidth, UINT *nHeight)
{
	*nWidth = m_nWidth;
	*nHeight = m_nHeight;
}

void CAGVideoWnd::SetBitrateInfo(int nReceivedBitrate)
{
	m_nBitRate = nReceivedBitrate;
	m_wndInfo.SetBitrateInfo(nReceivedBitrate);
}

void CAGVideoWnd::SetFrameRateInfo(int nReceiveFrameRate)
{
	m_nFrameRate = nReceiveFrameRate;
	m_wndInfo.SetFrameRateInfo(nReceiveFrameRate);
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

	m_wndInfo.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 192, 28), this, IDC_STATIC);

	return 0;
}

void CAGVideoWnd::ShowVideoInfo(BOOL bShow)
{
	m_bShowVideoInfo = bShow;

	m_wndInfo.ShowTips(bShow);
	Invalidate(FALSE);
}


void CAGVideoWnd::SetBigShowFlag(BOOL bBigShow)
{
	CRect	rcClient;

	m_bBigShow = bBigShow;
	GetClientRect(&rcClient);

	int x = (rcClient.Width()- 192) / 2;
	int y = rcClient.Height() - 24;
	
	if (m_wndInfo.GetSafeHwnd() != NULL) {
		if (m_bBigShow)
			y -= 4;

		m_wndInfo.MoveWindow(x, y, 192, 24);
	}
};


void CAGVideoWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	int x = (cx - 192) / 2;
	int y = cy - 24;

	if (m_wndInfo.GetSafeHwnd() != NULL) {
		if (m_bBigShow)
			y -= 4;

		m_wndInfo.MoveWindow(x, y, 192, 24);
	}
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

void CAGVideoWnd::threadLocalRender()
{
	SIZE_T nBufferLen = m_nWidth * m_nHeight * 3 / 2;
	BYTE* pBufferRGB24 = new BYTE[m_nWidth * m_nHeight * 3];
	ZeroMemory(pBufferRGB24, m_nWidth * m_nHeight * 3);
	BYTE* pBufferRGB24_1 = new BYTE[m_nWidth * m_nHeight * 3];
	ZeroMemory(pBufferRGB24_1, m_nWidth * m_nHeight * 3);

	while (m_bRunning) {
		std::this_thread::sleep_for(std::chrono::milliseconds(40));

		RECT rt;
		::GetWindowRect(m_hWnd, &rt);
		HDC hdc = ::GetWindowDC(m_hWnd);
		HDC hMemDc = CreateCompatibleDC(hdc);
		HBITMAP hBmp = CreateCompatibleBitmap(hdc, rt.right - rt.left, rt.bottom - rt.top);
		HBITMAP hOld = (HBITMAP)SelectObject(hMemDc, hBmp);
		BITMAPINFO bmpHdr = { 0 };
		bmpHdr.bmiHeader.biWidth = m_nWidth;
		bmpHdr.bmiHeader.biHeight = m_nHeight;
		bmpHdr.bmiHeader.biBitCount = 24;
		bmpHdr.bmiHeader.biClrImportant = 0;
		bmpHdr.bmiHeader.biSize = sizeof(bmpHdr);
		bmpHdr.bmiHeader.biXPelsPerMeter = 0;
		bmpHdr.bmiHeader.biYPelsPerMeter = 0;
		bmpHdr.bmiHeader.biClrUsed = 0;
		bmpHdr.bmiHeader.biPlanes = 1;
		bmpHdr.bmiHeader.biCompression = BI_RGB;

		if (CVideoPackageQueue::GetInstance()->PopVideoPackage(m_bufferFrame.get(), &nBufferLen)) {
			CYUVTrans::I420ToRGB24(m_bufferFrame.get(), pBufferRGB24_1,nBufferLen * 2,m_nWidth,m_nHeight);
			CYUVTrans::VerMirror_RGB24(pBufferRGB24_1, pBufferRGB24, m_nWidth, m_nHeight);

			//landscap scale.
			int nWndWidth = rt.right - rt.left;
			int nWndHeight = rt.bottom - rt.top;
			float fWndScale = nWndWidth * 1.0 / nWndHeight;
			float fVideoScale = m_nWidth * 1.0/ m_nHeight;
			float xPos, yPos, nScapWidth, nScapHeight = 0;
			if (fWndScale >= fVideoScale) {//height piexl full 
				xPos = (nWndWidth - nWndHeight * fVideoScale) * 1.0 / 2;
				yPos = 0;
				nScapHeight = nWndHeight;
				nScapWidth = nWndHeight * fVideoScale;
			}
			else {//width piexl full
				xPos = 0;
				yPos = (nWndHeight - nWndWidth / fVideoScale) *1.0 / 2;
				nScapWidth = nWndWidth;
				nScapHeight = nWndWidth / fVideoScale;
			}

			SetStretchBltMode(hMemDc, HALFTONE);
			StretchDIBits(hMemDc, xPos, yPos, nScapWidth, nScapHeight,
				0, 0, m_nWidth, m_nHeight, pBufferRGB24, &bmpHdr, DIB_RGB_COLORS, SRCCOPY);

			BitBlt(hdc, 0, 0, nWndWidth, nWndHeight,
				hMemDc, 0,0,SRCCOPY);

			SelectObject(hMemDc, hOld);
			DeleteObject(hBmp);
			DeleteObject(hMemDc);
			::ReleaseDC(m_hWnd, hdc);
		}
	}

	delete[] pBufferRGB24; pBufferRGB24 = nullptr;
	delete[] pBufferRGB24_1; pBufferRGB24_1 = nullptr;
}

BOOL CAGVideoWnd::EnableLocalRender(BOOL bEnable)
{
	if (bEnable) {
		if (m_nUID == 0 && !m_bRunning) {
			m_threadLocalRender = std::thread(&CAGVideoWnd::threadLocalRender, this);
			m_bRunning = true;
			m_threadLocalRender.detach();
		}
	}
	else
		m_bRunning = false;

	return TRUE;
}