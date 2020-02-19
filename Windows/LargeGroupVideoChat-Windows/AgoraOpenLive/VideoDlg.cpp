// VideoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraOpenLive.h"
#include "VideoDlg.h"
#include "afxdialogex.h"
#include "AGEventDef.h"
//#include "video_preprocessing_plugin.h"

// CVideoDlg 对话框

IMPLEMENT_DYNAMIC(CVideoDlg, CDialogEx)

CVideoDlg::CVideoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVideoDlg::IDD, pParent)
{
	m_brHead.CreateSolidBrush(RGB(0, 160, 239));
	SetBackgroundColor(RGB(0x26, 0x26, 0x26));
	m_bLastStat = FALSE;
	m_nScreenMode = SCREEN_VIDEO1;
	m_bFullScreen = FALSE;
	m_lpBigShowed = NULL;
	//m_lpWhiteBoard = NULL;
	//m_nWhiteboardMode = WHITEBOARD_HOST;
    m_bFilter = FALSE;
    m_bShowInfo = FALSE;
}

CVideoDlg::~CVideoDlg()
{
	m_brHead.DeleteObject();
}

void CVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

//	DDX_Control(pDX, IDC_CBXROLE_VIDEO, m_cbxRole);
	DDX_Control(pDX, IDC_BTNMIN_VIDEO, m_btnMin);
	DDX_Control(pDX, IDC_BTNRSTO_VIDEO, m_btnRst);
	DDX_Control(pDX, IDC_BTNCLOSE_VIDEO, m_btnClose);
}


BEGIN_MESSAGE_MAP(CVideoDlg, CDialogEx)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_NCHITTEST()

	ON_MESSAGE(WM_SHOWMODECHANGED, &CVideoDlg::OnShowModeChanged)
	ON_MESSAGE(WM_SHOWBIG, &CVideoDlg::OnShowBig)
    ON_MESSAGE(WM_WINDOWSHARE, &CVideoDlg::OnWindowShareStart)
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CVideoDlg::OnEIDJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_REJOINCHANNEL_SUCCESS), &CVideoDlg::OnEIDReJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_FIRST_LOCAL_VIDEO_FRAME), &CVideoDlg::OnEIDFirstLocalFrame)

	ON_MESSAGE(WM_MSGID(EID_FIRST_REMOTE_VIDEO_DECODED), &CVideoDlg::OnEIDFirstRemoteFrameDecoded)
	ON_MESSAGE(WM_MSGID(EID_AUDIO_VOLUME_INDICATION), &CVideoDlg::OnEIDAudioVolumeIndication)
//	ON_MESSAGE(WM_MSGID(EID_ACTIVE_SPEAKER), &CVideoDlg::OnEIDActiveSpeaker)

	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CVideoDlg::OnEIDUserOffline)
	
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STAT), &CVideoDlg::OnEIDRemoteVideoStat)

	ON_MESSAGE(WM_MSGID(EID_VIDEO_DEVICE_STATE_CHANGED), &CVideoDlg::OnEIDVideoDeviceChanged)

	ON_MESSAGE(WM_MSGID(EID_START_RCDSRV), &CVideoDlg::OnEIDStartRecordingService)
	ON_MESSAGE(WM_MSGID(EID_STOP_RCDSRV), &CVideoDlg::OnEIDStopRecordingService)
	ON_MESSAGE(WM_MSGID(EID_APICALL_EXECUTED), &CVideoDlg::OnEIDApiCallExecuted)

	ON_MESSAGE(WM_MSGID(EID_ERROR), &CVideoDlg::OnEIDError)

	ON_BN_CLICKED(IDC_BTNMIN_VIDEO, &CVideoDlg::OnBnClickedBtnmin)
	ON_BN_CLICKED(IDC_BTNCLOSE_VIDEO, &CVideoDlg::OnBnClickedBtnclose)
	ON_BN_CLICKED(IDC_BTNRSTO_VIDEO, &CVideoDlg::OnBnClickedBtnrest)
	ON_BN_CLICKED(IDC_BTNENDCALL_VIDEO, &CVideoDlg::OnBnClickedBtnclose)

	ON_BN_CLICKED(IDC_BTNTIP_VIDEO, &CVideoDlg::OnBnClickedBtntip)
	ON_BN_CLICKED(IDC_BTNMODE_VIDEO, &CVideoDlg::OnBnClickedBtnmode)
	ON_BN_CLICKED(IDC_BTNAUDIO_VIDEO, &CVideoDlg::OnBnClickedBtnaudio)

	ON_BN_CLICKED(IDC_BTNMESSAGE_VIDEO, &CVideoDlg::OnBnClickedBtnmessage)
	ON_BN_CLICKED(IDC_BTNSCRCAP_VIDEO, &CVideoDlg::OnBnClickedBtnScreenCapture)
    ON_BN_CLICKED(IDC_BTNMORE_VIDEO, &CVideoDlg::OnBnClickedBtnMore)
	ON_BN_CLICKED(IDC_BTNMODE_VIDEO, &CVideoDlg::OnBnClickedBtnmode)

	ON_BN_CLICKED(ID_IDR_MUTELVIDEO, &CVideoDlg::OnBnClickedBtnMuteLocalVideo)
	ON_BN_CLICKED(ID_IDR_MUTEPVIDEO, &CVideoDlg::OnBnClickedBtnMuteAllRemoteVideo)
	ON_BN_CLICKED(ID_IDR_DISVIDEO, &CVideoDlg::OnBnClickedBtnDisableVideo)

    ON_BN_CLICKED(ID_IDR_VIDEOINFO, &CVideoDlg::OnBnClickedBtntip)
    ON_BN_CLICKED(ID_IDR_FILTER, &CVideoDlg::OnBnClickedBtnfilter)
	ON_BN_CLICKED(ID_IDR_MUSICMIX, &CVideoDlg::OnBnClickedBtnmix)
	
	ON_BN_CLICKED(ID_IDR_SEIPUSH, &CVideoDlg::OnBnClickedBtSEIPush)

	ON_BN_CLICKED(IDC_BTNSCR_VIDEO, &CVideoDlg::OnBnClickedBtnfullscr)

	ON_BN_CLICKED(ID_SCRSHARE_DESKTOPSHARE, &CVideoDlg::OnBnClickedScreenshare)
	ON_BN_CLICKED(ID_SCRSHARE_WINDOWSHARE, &CVideoDlg::OnBnClickedWindowshare)
	ON_BN_CLICKED(ID_SCRSHARE_ENDSHARE, &CVideoDlg::OnBnClickedEndshare)

	
	ON_BN_CLICKED(ID_IDR_DEVICE, &CVideoDlg::OnBnClickedBtnsetup)
	ON_BN_CLICKED(ID_IDR_SEIPUSH, &CVideoDlg::OnBnClickedBtSEIPush)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CVideoDlg::OnEIDLeaveChannel)
	ON_WM_SHOWWINDOW()
    ON_WM_MOVE()
	ON_WM_TIMER()
	
END_MESSAGE_MAP()


// CVideoDlg 消息处理程序


void CVideoDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	if (m_btnMin.GetSafeHwnd() != NULL)
		m_btnMin.MoveWindow(cx - 72, 0, 24, 24, TRUE);
	if (m_btnRst.GetSafeHwnd() != NULL)
		m_btnRst.MoveWindow(cx - 48, 0, 24, 24, TRUE);
	if (m_btnClose.GetSafeHwnd() != NULL)
		m_btnClose.MoveWindow(cx - 24, 0, 24, 24, TRUE);

	GetClientRect(&m_rcVideoArea);
	m_rcVideoArea.top += 24;
	m_rcVideoArea.bottom -= 72;

	// 2人， 右上角子画面区域
	m_rcChildVideoArea.top = m_rcVideoArea.top + 10;
	m_rcChildVideoArea.bottom = m_rcChildVideoArea.top + 144;
	m_rcChildVideoArea.right = m_rcVideoArea.right - 14;
	m_rcChildVideoArea.left = m_rcChildVideoArea.right - 192;

	if (::IsWindow(m_dlgChat.GetSafeHwnd()) && m_dlgChat.IsWindowVisible()) {
		CRect   rcChatArea;

		rcChatArea.CopyRect(&m_rcVideoArea);
		rcChatArea.top = rcChatArea.bottom - 150;
		ClientToScreen(&rcChatArea);
		m_dlgChat.MoveWindow(&rcChatArea);
	}

	if (m_wndLocal.GetSafeHwnd() == NULL || m_wndVideo[0].GetSafeHwnd() == NULL)
		return;

	switch (m_nScreenMode)
	{
	case SCREEN_VIDEO1:
		AdjustSizeVideo1(cx, cy);
		AdjustButtonsNormal(cx, cy);
		break;
	case SCREEN_VIDEO4:
		AdjustSizeVideo4(cx, cy);
		AdjustButtonsNormal(cx, cy);
		break;
	case SCREEN_VIDEOMULTI:
		AdjustSizeVideoMulti(cx, cy);
		AdjustButtonsNormal(cx, cy);
		break;
	default:
		break;
	}

	Invalidate(TRUE);
}

void CVideoDlg::ShowButtonsNormal()
{
	CRect	rcClient;
	int nShowMode = SW_HIDE;

	if (!m_bFullScreen)
		nShowMode = SW_SHOW;

	m_btnMode.ShowWindow(nShowMode);
	m_btnMore.ShowWindow(nShowMode);

	m_btnAudio.SetBackImage(IDB_BTNMAUDIO_VIDEO, RGB(0x26, 0x26, 0x26));
	m_btnAudio.ShowWindow(nShowMode);
	m_btnEndCall.SetBackImage(IDB_BTNENDCALL_VIDEO, RGB(0x26, 0x26, 0x26));
	m_btnEndCall.ShowWindow(nShowMode);
	m_btnScrCap.ShowWindow(nShowMode);
	m_btnShow.ShowWindow(nShowMode);
    //m_btnTip.ShowWindow(nShowMode);
	m_btnScrCap.ShowWindow(nShowMode);

	GetClientRect(&rcClient);
	AdjustButtonsNormal(rcClient.Width(), rcClient.Height());
}



void CVideoDlg::AdjustButtonsNormal(int cx, int cy)
{
    /*if(m_btnMessage.GetSafeHwnd() != NULL)
        m_btnMessage.MoveWindow(cx / 2 - 312, cy - 60, 48, 48, TRUE);*/
	if (m_btnMode.GetSafeHwnd() != NULL)
		m_btnMode.MoveWindow(cx / 2 - 216, cy - 60, 48, 48, TRUE);
	if (m_btnAudio.GetSafeHwnd() != NULL)
		m_btnAudio.MoveWindow(cx / 2 - 120, cy - 60, 48, 48, TRUE);
	if (m_btnEndCall.GetSafeHwnd() != NULL)
		m_btnEndCall.MoveWindow(cx / 2 - 24, cy - 60, 48, 48, TRUE);

	if (m_btnScrCap.GetSafeHwnd() != NULL)
		m_btnScrCap.MoveWindow(cx / 2 + 72, cy - 60, 48, 48, TRUE);
    if (m_btnMore.GetSafeHwnd() != NULL)
        m_btnMore.MoveWindow(cx / 2 + 168, cy - 60, 48, 48, TRUE);

	if (m_btnShow.GetSafeHwnd() != NULL)
		m_btnShow.MoveWindow(cx - 72, cy - 48, 48, 48, TRUE);
	/*if (m_btnTip.GetSafeHwnd() != NULL)
		m_btnTip.MoveWindow(cx - 54, cy - 48, 24, 24, TRUE);
        */
}

void CVideoDlg::AdjustSizeVideo1(int cx, int cy)
{
	if (m_listWndInfo.GetCount() == 0)
		m_wndLocal.MoveWindow(&m_rcVideoArea, FALSE);
	else {
		m_wndVideo[0].MoveWindow(&m_rcVideoArea, FALSE);
		m_wndLocal.MoveWindow(&m_rcChildVideoArea, FALSE);
	}
}

void CVideoDlg::AdjustSizeVideo4(int cx, int cy)
{
	m_wndVideo[0].MoveWindow(0, m_rcVideoArea.top, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);
	m_wndVideo[1].MoveWindow(m_rcVideoArea.Width() / 2, m_rcVideoArea.top, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);
	m_wndVideo[2].MoveWindow(0, m_rcVideoArea.top + m_rcVideoArea.Height() / 2, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);
	m_wndVideo[3].MoveWindow(m_rcVideoArea.Width() / 2, m_rcVideoArea.top + m_rcVideoArea.Height() / 2, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);

	if (m_listWndInfo.GetCount() == 2)
		m_wndLocal.MoveWindow(0, m_rcVideoArea.top + m_rcVideoArea.Height() / 2, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);
	else
		m_wndLocal.MoveWindow(m_rcVideoArea.Width() / 2, m_rcVideoArea.top + m_rcVideoArea.Height() / 2, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);
}

void CVideoDlg::AdjustSizeVideoMulti(int cx, int cy)
{
	int nLocalIndex = 0;
	CRect	rcLeftHalf;
	CRect	rcRightHalf;
	int		nVideoWidth = m_rcVideoArea.Width() / 2;
	int		nSmallVideoWidth = nVideoWidth / 4;
	int		nSmallVideoHeight = m_rcVideoArea.Height() / 4;

	rcLeftHalf.SetRect(m_rcVideoArea.left, m_rcVideoArea.top, m_rcVideoArea.left + nVideoWidth, m_rcVideoArea.bottom);
	rcRightHalf.SetRect(m_rcVideoArea.left + nVideoWidth, m_rcVideoArea.top, m_rcVideoArea.right, m_rcVideoArea.bottom);

	m_lpBigShowed->MoveWindow(&rcLeftHalf, TRUE);

	for (int nIndex = 0; nIndex < 16; nIndex++) {
		int nXPos = rcRightHalf.left + nSmallVideoWidth * (nLocalIndex % 4);
		int nYPos = rcRightHalf.top + nSmallVideoHeight *(nLocalIndex / 4);

		if (!m_wndVideo[nIndex].IsBigShow()) {
//			if (m_wndVideo[nIndex].GetUID() != 0) {
				m_wndVideo[nIndex].MoveWindow(nXPos, nYPos, nSmallVideoWidth, nSmallVideoHeight, TRUE);
				nLocalIndex++;
//			}
		}
		else{
			m_wndLocal.MoveWindow(nXPos, nYPos, nSmallVideoWidth, nSmallVideoHeight, TRUE);
			nLocalIndex++;
		}
	}
}

void CVideoDlg::AdjustSizeVideoMultiNew(int cx, int cy)
{
	int nLocalIndex = 0;
	CRect	rcBottomHalf;
	CRect	rcTopHalf;
	int		nVideoWidth = m_rcVideoArea.Width();
	int		nVideoHight = m_rcVideoArea.Height() / 2;
	int		nSmallVideoWidth = nVideoWidth / 8;
	int		nSmallVideoHeight = nVideoHight / 2;

	//rcLeftHalf.SetRect(m_rcVideoArea.left, m_rcVideoArea.top, m_rcVideoArea.left + nVideoWidth, m_rcVideoArea.bottom);
	//rcRightHalf.SetRect(m_rcVideoArea.left + nVideoWidth, m_rcVideoArea.top, m_rcVideoArea.right, m_rcVideoArea.bottom);


	rcTopHalf.SetRect(m_rcVideoArea.left, m_rcVideoArea.top, m_rcVideoArea.right, m_rcVideoArea.top + nVideoHight);
	rcBottomHalf.SetRect(m_rcVideoArea.left, m_rcVideoArea.top + nVideoHight, m_rcVideoArea.right, m_rcVideoArea.bottom);



	m_lpBigShowed->MoveWindow(&rcBottomHalf, TRUE);

	for (int nIndex = 0; nIndex < 16; nIndex++) {
		int nXPos = rcTopHalf.left + nSmallVideoWidth * (nLocalIndex % 8);
		int nYPos = rcTopHalf.top + nSmallVideoHeight *(nLocalIndex / 8);

		if (!m_wndVideo[nIndex].IsBigShow()) {
			//			if (m_wndVideo[nIndex].GetUID() != 0) {
			m_wndVideo[nIndex].MoveWindow(nXPos, nYPos, nSmallVideoWidth, nSmallVideoHeight, TRUE);
			nLocalIndex++;
			//			}
		}
		else{
			m_wndLocal.MoveWindow(nXPos, nYPos, nSmallVideoWidth, nSmallVideoHeight, TRUE);
			nLocalIndex++;
		}
	}
}

void CVideoDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
		if (m_bFullScreen) {
		ShowControlButton(TRUE);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

BOOL CVideoDlg::NeedShowSizeBox(CPoint point)
{
	CRect rcClient;

	GetClientRect(&rcClient);

	rcClient.left += 5;
	rcClient.right -= 5;
	rcClient.top += 5;
	rcClient.bottom -= 5;
	
	if (rcClient.PtInRect(point) || IsZoomed())
		return FALSE;

	return TRUE;
}

void CVideoDlg::EnableSize(BOOL bEnable)
{
	LONG lStyle = ::GetWindowLong(GetSafeHwnd(), GWL_STYLE);

	if (bEnable)
		::SetWindowLong(GetSafeHwnd(), GWL_STYLE, lStyle | WS_SIZEBOX);
	else
		::SetWindowLong(GetSafeHwnd(), GWL_STYLE, lStyle&(~WS_SIZEBOX));

	CRect rcClient;

	if (bEnable)
		GetWindowRect(&rcClient);
	else
		GetClientRect(&rcClient);

	int cx = rcClient.Width();
	int cy = rcClient.Height();

	if (m_bLastStat != bEnable) {
		m_bLastStat = bEnable;
		if (m_btnMin.GetSafeHwnd() != NULL)
			m_btnMin.MoveWindow(cx - 72, 0, 24, 24, TRUE);
		if (m_btnRst.GetSafeHwnd() != NULL)
			m_btnRst.MoveWindow(cx - 48, 0, 24, 24, TRUE);
		if (m_btnClose.GetSafeHwnd() != NULL)
			m_btnClose.MoveWindow(cx - 24, 0, 24, 24, TRUE);

		if (m_btnScrCap.GetSafeHwnd() != NULL)
			m_btnScrCap.MoveWindow(48, cy - 84, 48, 48, TRUE);

		if (m_btnMode.GetSafeHwnd() != NULL)
			m_btnMode.MoveWindow(cx / 2 - 96, cy - 84, 48, 48, TRUE);
		if (m_btnAudio.GetSafeHwnd() != NULL)
			m_btnAudio.MoveWindow(cx / 2 + 24, cy - 84, 48, 48, TRUE);
		if (m_btnShow.GetSafeHwnd() != NULL)
            m_btnShow.MoveWindow(cx - 72, cy - 84, 48, 48, TRUE);
		if (m_btnEndCall.GetSafeHwnd() != NULL)
			m_btnEndCall.MoveWindow(cx - 120, cy - 84, 48, 48, TRUE);

		Invalidate(FALSE);
	}
}

void CVideoDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CDialogEx::OnPaint()
	DrawHead(&dc);
}


LRESULT CVideoDlg::OnNcHitTest(CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	LRESULT lResult = CDialogEx::OnNcHitTest(point);
	if (lResult == HTCLIENT && ::GetAsyncKeyState(MK_LBUTTON) < 0)
		lResult = HTCAPTION;

	return lResult;
}

void CVideoDlg::OnBnClickedBtnmin()
{
	ShowWindow(SW_MINIMIZE);
}

void CVideoDlg::OnBnClickedBtnclose()
{
	GetParent()->SendMessage(WM_LEAVECHANNEL, 0, 0);

	m_listWndInfo.RemoveAll();
	m_dlgChat.ShowWindow(SW_HIDE);
	m_dlgDevice.ShowWindow(SW_HIDE);

	CAgoraObject::GetAgoraObject()->StopAudioMixing();
	//m_dlgMusic.SetPlayingFlag(FALSE);

    // unmute local video
	CAgoraObject::GetAgoraObject()->EnableVideo(TRUE);
    CAgoraObject::GetAgoraObject()->MuteLocalVideo(FALSE);
    m_btnMode.SetBackImage(IDB_BTNAUDIO_VIDEO);

    // unmute local audio
    CAgoraObject::GetAgoraObject()->MuteLocalAudio(FALSE);
    m_btnAudio.SwitchButtonStatus(CAGButton::AGBTN_NORMAL);

	CAgoraObject::GetAgoraObject()->EnableScreenCapture(NULL, 0, NULL, FALSE);
    m_btnScrCap.SwitchButtonStatus(CAGButton::AGBTN_NORMAL);

    m_dlgChat.ShowWindow(SW_HIDE);
    m_dlgChat.ClearHistory();
    //m_btnMessage.SwitchButtonStatus(CAGButton::AGBTN_NORMAL);

	CAgoraObject::GetAgoraObject()->EnableLocalRender(TRUE);
	//unload_preprocessing_plugin(CAgoraObject::GetEngine());

	//CAgoraObject::GetAgoraObject()->ClearSEIInfo();

	CDialogEx::OnOK();
}

void CVideoDlg::OnBnClickedBtnrest()
{
	if (IsZoomed()) {
		m_btnRst.SetBackImage(IDB_BTNMAX, RGB(0xFF, 0x00, 0xFF));
		ShowWindow(SW_RESTORE);
	}
	else{
		m_btnRst.SetBackImage(IDB_BTNRESTORE, RGB(0xFF, 0x00, 0xFF));
		ShowWindow(SW_MAXIMIZE);
	}
	//return;
	CRect	rcClient;

	GetClientRect(&rcClient);
	m_rcVideoArea.CopyRect(&rcClient);
	m_rcVideoArea.top += 24;
	m_rcVideoArea.bottom -= 72;
	  
	m_rcChildVideoArea.top = m_rcVideoArea.top + 16;
	m_rcChildVideoArea.bottom = m_rcChildVideoArea.top + 144;
	m_rcChildVideoArea.left = m_rcVideoArea.right - 212;
	m_rcChildVideoArea.right = m_rcVideoArea.right - 20;

	int cx = rcClient.Width();
	int cy = rcClient.Height();

	switch (m_nScreenMode)
	{
	case SCREEN_VIDEO1:
		AdjustSizeVideo1(cx, cy);
		AdjustButtonsNormal(cx, cy);
		break;
	case SCREEN_VIDEO4:
		AdjustSizeVideo4(cx, cy);
		AdjustButtonsNormal(cx, cy);
		break;
	case SCREEN_VIDEOMULTI:
		AdjustSizeVideoMultiNew(cx, cy);
		AdjustButtonsNormal(cx, cy);
		break;
	default:
		break;
	}

	Invalidate(TRUE);
}


void CVideoDlg::OnBnClickedBtnfullscr()
{
	int nShowMode = 0;

	if (m_bFullScreen) {
		m_bFullScreen = FALSE;
		nShowMode = SW_SHOW;		
		ShowWindow(SW_RESTORE);

		GetClientRect(&m_rcVideoArea);
		m_rcVideoArea.top += 24;
		m_rcVideoArea.bottom -= 72;	
	}
	else{
		m_bFullScreen = TRUE;
		nShowMode = SW_HIDE;
		ShowWindow(SW_MAXIMIZE);

		GetClientRect(&m_rcVideoArea);
	}

	m_btnMin.ShowWindow(nShowMode);
	m_btnRst.ShowWindow(nShowMode);
	m_btnClose.ShowWindow(nShowMode);
	
	m_btnMode.ShowWindow(nShowMode);
	m_btnAudio.ShowWindow(nShowMode);
	m_btnEndCall.ShowWindow(nShowMode);
	m_btnScrCap.ShowWindow(nShowMode);
    m_btnMore.ShowWindow(nShowMode);
	

	switch (m_nScreenMode)
	{
	case SCREEN_VIDEO1:
		ShowVideo1();
		break;
	case SCREEN_VIDEO4:
		ShowVideo4();
		break;
	case SCREEN_VIDEOMULTI:
		ShowMultiNew();
		break;
	default:
		break;
	}

	Invalidate(TRUE);
}


void CVideoDlg::OnBnClickedScreenshare()
{
	IRtcEngine *lpRtcEngine = CAgoraObject::GetEngine();

	CAgoraObject::GetAgoraObject()->EnableScreenCapture(::GetDesktopWindow(), 15, NULL, TRUE);
	m_btnScrCap.SwitchButtonStatus(CAGButton::AGBTN_PUSH);

	m_wndLocal.Invalidate(TRUE);
	m_wndLocal.UpdateWindow();

	Invalidate(TRUE);
	UpdateWindow();
}

void CVideoDlg::OnBnClickedWindowshare()
{
	m_dlgScreenCapture.RefreashWndInfo();
	m_dlgScreenCapture.SaveScreen(NULL);

	m_dlgScreenCapture.ShowWindow(SW_MAXIMIZE);
}

void CVideoDlg::OnBnClickedEndshare()
{
	CAgoraObject *lpAgora = CAgoraObject::GetAgoraObject();

	lpAgora->EnableScreenCapture(NULL, 15, NULL, FALSE);
	m_btnScrCap.SwitchButtonStatus(CAGButton::AGBTN_NORMAL);
	CAgoraObject::GetAgoraObject()->EnableLocalRender(TRUE);
}

void CVideoDlg::ShowControlButton(BOOL bShow)
{
	int nShowMode = 0;
	if (bShow)
		nShowMode = SW_SHOW;
	else
		nShowMode = SW_HIDE;

	m_btnScrCap.ShowWindow(nShowMode);
    //m_btnMessage.ShowWindow(nShowMode);
	m_btnMode.ShowWindow(nShowMode);
    m_btnMore.ShowWindow(nShowMode);
	m_btnAudio.ShowWindow(nShowMode);
	m_btnShow.ShowWindow(nShowMode);
	m_btnEndCall.ShowWindow(nShowMode);
}

void CVideoDlg::OnBnClickedBtntip()
{
	for (int nIndex = 0; nIndex < 4; nIndex++)
        m_wndVideo[nIndex].ShowVideoInfo(!m_bShowInfo);

    m_bShowInfo = !m_bShowInfo;
}

void CVideoDlg::OnBnClickedBtnScreenCapture()
{
	CreateScreenShareMenu();

	Invalidate();
}

void CVideoDlg::OnBnClickedBtnMuteLocalVideo()
{
	CAgoraObject *lpAgoraObject = CAgoraObject::GetAgoraObject();

	BOOL bMute = lpAgoraObject->IsLocalVideoMuted();

	lpAgoraObject->MuteLocalVideo(!bMute);

	m_wndLocal.Invalidate(TRUE);
	m_wndLocal.UpdateWindow();
}

void CVideoDlg::OnBnClickedBtnMuteAllRemoteVideo()
{
	CAgoraObject *lpAgoraObject = CAgoraObject::GetAgoraObject();

	BOOL bMute = lpAgoraObject->IsAllRemoteVideoMuted();

	lpAgoraObject->MuteAllRemoteVideo(!bMute);

	m_wndLocal.Invalidate(TRUE);
	m_wndLocal.UpdateWindow();

	Invalidate(TRUE);
	UpdateWindow();
}

void CVideoDlg::OnBnClickedBtnDisableVideo()
{
	CAgoraObject *lpAgoraObject = CAgoraObject::GetAgoraObject();

	BOOL bEnable = lpAgoraObject->IsVideoEnabled();

	lpAgoraObject->EnableVideo(!bEnable);
	lpAgoraObject->EnableLocalRender(!bEnable);
	if (bEnable)
		m_btnScrCap.SwitchButtonStatus(CAGButton::AGBTN_NORMAL);
		
	m_wndLocal.Invalidate(TRUE);
	m_wndLocal.UpdateWindow();

	Invalidate(TRUE);
	UpdateWindow();
}

void CVideoDlg::OnBnClickedBtnMore()
{
    CreateMoreMenu();
}

void CVideoDlg::CreateMuteAudioMenu()
{
	CRect				rcButton;
	CMenu				Menu;
	MENUITEMINFO		mii;
	UINT				nFlag = 0;

	CAgoraObject	*lpAgoraObject = CAgoraObject::GetAgoraObject();

	Menu.LoadMenu(IDR_MENU_MUTEAUDIO);

	CMenu *pMenuPopup = Menu.GetSubMenu(0);

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING;

	mii.dwTypeData = const_cast<LPTSTR>LANG_STR("IDS_VIDEO_MUTELOCALAUDIO");
	mii.cch = _tcslen(LANG_STR("IDS_VIDEO_MUTELOCALAUDIO")) + 1;
	if (lpAgoraObject->IsLocalAudioMuted())
		nFlag = MF_BYCOMMAND | MF_CHECKED;
	else
		nFlag = MF_BYCOMMAND | MF_UNCHECKED;
	pMenuPopup->CheckMenuItem(ID_IDR_MUTELAUDIO, nFlag);
	pMenuPopup->SetMenuItemInfo(0, &mii, FALSE);

	mii.dwTypeData = const_cast<LPTSTR>LANG_STR("IDS_VIDEO_MUTEALLREMOTEAUDIO");
	mii.cch = _tcslen(LANG_STR("IDS_VIDEO_MUTEALLREMOTEAUDIO")) + 1;
	if (lpAgoraObject->IsAllRemoteAudioMuted())
		nFlag = MF_BYCOMMAND | MF_CHECKED;
	else
		nFlag = MF_BYCOMMAND | MF_UNCHECKED;
	pMenuPopup->CheckMenuItem(ID_IDR_MUTEPAUDIO, nFlag);
	pMenuPopup->SetMenuItemInfo(1, &mii, FALSE);

	mii.dwTypeData = const_cast<LPTSTR>LANG_STR("IDS_VIDEO_DISABLEAUDIO");
	mii.cch = _tcslen(LANG_STR("IDS_VIDEO_DISABLEAUDIO")) + 1;
	if (!lpAgoraObject->IsAudioEnabled())
		nFlag = MF_BYCOMMAND | MF_CHECKED;
	else
		nFlag = MF_BYCOMMAND | MF_UNCHECKED;
	pMenuPopup->CheckMenuItem(ID_IDR_DISAUDIO, nFlag);
	pMenuPopup->SetMenuItemInfo(2, &mii, FALSE);
	m_btnAudio.GetWindowRect(&rcButton);

	int x = rcButton.left - 60;
	int y = rcButton.top - 50;

	pMenuPopup->TrackPopupMenu(TPM_RIGHTBUTTON, x, y, this);
	pMenuPopup->DestroyMenu();
}

void CVideoDlg::CreateMuteVideoMenu()
{
	CRect				rcButton;
	CMenu				Menu;
	MENUITEMINFO		mii;
	UINT				nFlag = 0;

	CAgoraObject	*lpAgoraObject = CAgoraObject::GetAgoraObject();

	Menu.LoadMenu(IDR_MENU_MUTEVIDEO);

	CMenu *pMenuPopup = Menu.GetSubMenu(0);

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING;

	mii.dwTypeData = const_cast<LPTSTR>LANG_STR("IDS_SET_MUTELOCALVIDEO");
	mii.cch = _tcslen(LANG_STR("IDS_SET_MUTELOCALVIDEO")) + 1;
	if (lpAgoraObject->IsLocalVideoMuted())
		nFlag = MF_BYCOMMAND | MF_CHECKED;
	else
		nFlag = MF_BYCOMMAND | MF_UNCHECKED;
	pMenuPopup->CheckMenuItem(ID_IDR_MUTELVIDEO, nFlag);
	pMenuPopup->SetMenuItemInfo(0, &mii, FALSE);

	mii.dwTypeData = const_cast<LPTSTR>LANG_STR("IDS_VIDEO_MUTEALLREMOTEVIDEO");
	mii.cch = _tcslen(LANG_STR("IDS_VIDEO_MUTEALLREMOTEVIDEO")) + 1;
	if (lpAgoraObject->IsAllRemoteVideoMuted())
		nFlag = MF_BYCOMMAND | MF_CHECKED;
	else
		nFlag = MF_BYCOMMAND | MF_UNCHECKED;
	pMenuPopup->CheckMenuItem(ID_IDR_MUTEPVIDEO, nFlag);
	pMenuPopup->SetMenuItemInfo(1, &mii, FALSE);

	mii.dwTypeData = const_cast<LPTSTR>LANG_STR("IDS_VIDEO_DISABLEVIDEO");
	mii.cch = _tcslen(LANG_STR("IDS_VIDEO_DISABLEVIDEO")) + 1;
	if (!lpAgoraObject->IsVideoEnabled())
		nFlag = MF_BYCOMMAND | MF_CHECKED;
	else
		nFlag = MF_BYCOMMAND | MF_UNCHECKED;
	pMenuPopup->CheckMenuItem(ID_IDR_DISVIDEO, nFlag);
	pMenuPopup->SetMenuItemInfo(2, &mii, FALSE);
	m_btnMode.GetWindowRect(&rcButton);

	int x = rcButton.left - 60;
	int y = rcButton.top - 50;

	pMenuPopup->TrackPopupMenu(TPM_RIGHTBUTTON, x, y, this);
	pMenuPopup->DestroyMenu();
}

void CVideoDlg::CreateScreenShareMenu()
{
	CRect				rcButton;
	CMenu				Menu;
	MENUITEMINFO		mii;

	Menu.LoadMenu(IDR_MENU_SCRSHARE);

	CMenu *pMenuPopup = Menu.GetSubMenu(0);

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING;

	mii.dwTypeData = const_cast<LPTSTR>LANG_STR("IDS_VIDEO_SCRSHARE");
	mii.cch = _tcslen(LANG_STR("IDS_VIDEO_SCRSHARE")) + 1;
	pMenuPopup->SetMenuItemInfo(0, &mii, FALSE);

	mii.dwTypeData = const_cast<LPTSTR>LANG_STR("IDS_VIDEO_WNDSHARE");
	mii.cch = _tcslen(LANG_STR("IDS_VIDEO_WNDSHARE")) + 1;
	pMenuPopup->SetMenuItemInfo(1, &mii, FALSE);
	m_btnScrCap.GetWindowRect(&rcButton);

	int x = rcButton.left - 60;
	int y = rcButton.top - 50;

	pMenuPopup->TrackPopupMenu(TPM_RIGHTBUTTON, x, y, this);
	pMenuPopup->DestroyMenu();
}

void CVideoDlg::CreateMoreMenu()
{
	CRect			rcButton;
	CMenu			Menu;
	MENUITEMINFO	mii;
	UINT            nFlag = 0;
	int				nClientRole = CAgoraObject::GetAgoraObject()->GetClientRole();

	Menu.LoadMenu(IDR_MENU_MORE);

	CMenu *pMenuPopup = Menu.GetSubMenu(0);

	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING;

	mii.dwTypeData = const_cast<LPTSTR>LANG_STR("IDS_VIDEO_MOREDEVICE");
	mii.cch = _tcslen(LANG_STR("IDS_VIDEO_MOREDEVICE")) + 1;
	pMenuPopup->SetMenuItemInfo(0, &mii, FALSE);
	pMenuPopup->SetMenuItemBitmaps(ID_IDR_DEVICE, MF_BYCOMMAND, &m_bitMenuDevice, &m_bitMenuDevice);

	mii.dwTypeData = const_cast<LPTSTR>LANG_STR("IDS_VIDEO_MORESEI");
	mii.cch = _tcslen(LANG_STR("IDS_VIDEO_MORESEI")) + 1;
	pMenuPopup->SetMenuItemInfo(4, &mii, FALSE);
	pMenuPopup->SetMenuItemBitmaps(ID_IDR_SEIPUSH, MF_BYCOMMAND, &m_bitMenuSEI, &m_bitMenuSEI);

	m_btnMore.GetWindowRect(&rcButton);

	int x = rcButton.left - 60;
	int y = rcButton.top - 50;

	pMenuPopup->TrackPopupMenu(TPM_RIGHTBUTTON, x, y, this);
	pMenuPopup->DestroyMenu();
}

void CVideoDlg::OnBnClickedBtnsetup()
{
	m_dlgDevice.ShowWindow(SW_SHOW);
	m_dlgDevice.CenterWindow();
    CAgoraObject::GetAgoraObject()->SetClickTestFlag(TRUE);
}

void CVideoDlg::OnBnClickedBtnfilter()
{
    return;
    IRtcEngine *lpEngine = CAgoraObject::GetEngine();

    /*if (!m_bFilter){
        load_preprocessing_plugin(lpEngine);
    }
    else{
        unload_preprocessing_plugin(lpEngine);
    }*/

    m_bFilter = !m_bFilter;
}

void CVideoDlg::OnBnClickedBtnmix()
{
	//m_dlgMusic.ShowWindow(SW_SHOW);
	//m_dlgMusic.CenterWindow();
}

void CVideoDlg::OnBnClickedBtSEIPush()
{
	m_dlgSEI.ShowWindow(SW_SHOW);
	m_dlgSEI.CenterWindow();
}


void CVideoDlg::OnBnClickedBtnmessage()
{
    BOOL bIsShowing = m_dlgChat.IsWindowVisible();
    CRect   rcChatArea;

    rcChatArea.CopyRect(&m_rcVideoArea);
    rcChatArea.top = rcChatArea.bottom - 150;
    ClientToScreen(&rcChatArea);

    /*if (bIsShowing) {
        m_dlgChat.ShowWindow(SW_HIDE);
        m_btnMessage.SwitchButtonStatus(CAGButton::AGBTN_NORMAL);
    }
    else {
        m_dlgChat.ShowWindow(SW_SHOW);
        m_dlgChat.MoveWindow(&rcChatArea, TRUE);
        m_btnMessage.SwitchButtonStatus(CAGButton::AGBTN_PUSH);
    }*/
}

void CVideoDlg::OnBnClickedBtnmode()
{
	CAgoraObject *lpAgora = CAgoraObject::GetAgoraObject();

	if (!lpAgora->IsLocalVideoMuted()) {
		lpAgora->MuteLocalVideo(TRUE);
		m_btnMode.SwitchButtonStatus(CAGButton::AGBTN_PUSH);
	}
	else {
		lpAgora->MuteLocalVideo(FALSE);
		m_btnMode.SwitchButtonStatus(CAGButton::AGBTN_NORMAL);
	}

	Invalidate();
}

void CVideoDlg::OnBnClickedBtnaudio()
{
	CAgoraObject *lpAgora = CAgoraObject::GetAgoraObject();

	if (lpAgora->IsLocalAudioMuted()) {
		lpAgora->MuteLocalAudio(FALSE);
		m_btnAudio.SwitchButtonStatus(CAGButton::AGBTN_NORMAL);
	}
	else {
		lpAgora->MuteLocalAudio(TRUE);
		m_btnAudio.SwitchButtonStatus(CAGButton::AGBTN_PUSH);
	}
}

LRESULT CVideoDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	LPAGE_JOINCHANNEL_SUCCESS lpData = (LPAGE_JOINCHANNEL_SUCCESS)wParam;

	m_listWndInfo.RemoveAll();
	CAgoraObject *lpAgoraObject = CAgoraObject::GetAgoraObject();
	
	lpAgoraObject->SetSelfUID(lpData->uid);

	SEI_INFO seiInfo;
	if (lpAgoraObject->GetSEIInfo(0, &seiInfo))
		seiInfo.nUID = lpAgoraObject->GetSelfUID();

	lpAgoraObject->RemoveSEIInfo(0);
	lpAgoraObject->SetSEIInfo(lpAgoraObject->GetSelfUID(), &seiInfo);
	
	RENDER_MODE_TYPE localModeType;
	RENDER_MODE_TYPE remoteModeType;

	lpAgoraObject->GetRenderMode(&localModeType, &remoteModeType);

	AVSTATUS_INFO avsInfo;

	avsInfo.bMuteAudio = false;
	avsInfo.bMuteVideo = false;
	avsInfo.nRenderMode = localModeType;
	avsInfo.nUID = 0;

	lpAgoraObject->SetAVSInfo(0, &avsInfo);
	delete[] lpData->channel;
	lpData->channel = NULL;
	delete lpData;
	
	lpData = NULL;
	return 0;
}

LRESULT CVideoDlg::OnEIDReJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	LPAGE_REJOINCHANNEL_SUCCESS lpData = (LPAGE_REJOINCHANNEL_SUCCESS)wParam;

	m_listWndInfo.RemoveAll();
	delete lpData;

	return 0;
}

LRESULT CVideoDlg::OnEIDFirstLocalFrame(WPARAM wParam, LPARAM lParam)
{
	LPAGE_FIRST_LOCAL_VIDEO_FRAME lpData = (LPAGE_FIRST_LOCAL_VIDEO_FRAME)wParam;

	if (m_listWndInfo.GetCount() == 0)
		ShowVideo1();

	SEI_INFO seiInfo;

	CAgoraObject *lpAgoraObject = CAgoraObject::GetAgoraObject();

	memset(&seiInfo, 0, sizeof(SEI_INFO));

	seiInfo.nUID = lpAgoraObject->GetSelfUID();
	seiInfo.nWidth = lpData->width;
	seiInfo.nHeight = lpData->height;
	lpAgoraObject->SetSEIInfo(seiInfo.nUID, &seiInfo);

	delete lpData;

	return 0;
}

LRESULT CVideoDlg::OnEIDFirstRemoteFrameDecoded(WPARAM wParam, LPARAM lParam)
{
	LPAGE_FIRST_REMOTE_VIDEO_DECODED lpData = (LPAGE_FIRST_REMOTE_VIDEO_DECODED)wParam;
	CAgoraObject *lpAgoraObject = CAgoraObject::GetAgoraObject();
	BOOL bFound = FALSE;
	SEI_INFO seiInfo;
	AVSTATUS_INFO avsInfo;

	int nLowStreamType = lpAgoraObject->GetLowStreamType();

	switch (nLowStreamType)
	{
	case 0:
		lpAgoraObject->SetRemoteVideoStreamType(lpData->uid, REMOTE_VIDEO_STREAM_HIGH);
		break;
	case 1:
		lpAgoraObject->SetRemoteVideoStreamType(lpData->uid, REMOTE_VIDEO_STREAM_LOW);
		break;
	case 2:
		break;
	default:
		break;
	}

	POSITION pos = m_listWndInfo.GetHeadPosition();
	while (pos != NULL) {
		AGVIDEO_WNDINFO &agvWndInfo = m_listWndInfo.GetNext(pos);
		if (agvWndInfo.nUID == lpData->uid) {
			bFound = TRUE;
			break;
		}
	}

	if (!bFound) {
		AGVIDEO_WNDINFO agvWndInfo;
		memset(&agvWndInfo, 0, sizeof(AGVIDEO_WNDINFO));
		agvWndInfo.nUID = lpData->uid;
		agvWndInfo.nWidth = lpData->width;
		agvWndInfo.nHeight = lpData->height;

		m_listWndInfo.AddTail(agvWndInfo);
	}

	RebindVideoWnd();

	memset(&seiInfo, 0, sizeof(SEI_INFO));

	seiInfo.nUID = lpData->uid;
	seiInfo.nWidth = lpData->width;
	seiInfo.nHeight = lpData->height;
	CAgoraObject::GetAgoraObject()->SetSEIInfo(seiInfo.nUID, &seiInfo);

	CAgoraObject::GetAgoraObject()->GetAVSInfo(lpData->uid, &avsInfo);
	avsInfo.nUID = lpData->uid;
	avsInfo.nRenderMode = 0;
	avsInfo.bMuteAudio = false;
	avsInfo.bMuteVideo = false;

	CAgoraObject::GetAgoraObject()->SetAVSInfo(seiInfo.nUID, &avsInfo);

	delete lpData;

	return 0;
}

LRESULT CVideoDlg::OnEIDAudioVolumeIndication(WPARAM wParam, LPARAM lParam)
{
	LPAGE_AUDIO_VOLUME_INDICATION lpData = (LPAGE_AUDIO_VOLUME_INDICATION)wParam;

	for (int nIndex = 0; nIndex < 16; nIndex++)
		m_wndVideo[nIndex].ShowSpeakingTip(FALSE, 0);

	for (int i = 0; i < static_cast<unsigned int>(lpData->speakerNumber); i++) {
		for (int j = 0; j < 16; j++) {
			if (m_wndVideo[j].GetUID() == lpData->speakers[i].uid)
				m_wndVideo[j].ShowSpeakingTip(TRUE, lpData->speakers[i].volume);

			m_wndVideo[j].Invalidate(FALSE);
		}
	}
	
	delete lpData;

	return 0;
}

LRESULT CVideoDlg::OnEIDActiveSpeaker(WPARAM wParam, LPARAM lParam)
{
	UINT nUID = (UINT)wParam;

	for (int nIndex = 0; nIndex < 16; nIndex++) {
		if (m_wndVideo[nIndex].GetUID() == nUID)
			m_wndVideo[nIndex].ShowSpeakingTip(TRUE, 0);
		else
			m_wndVideo[nIndex].ShowSpeakingTip(FALSE, 0);
	}

	return 0;
}

LRESULT CVideoDlg::OnEIDError(WPARAM wParam, LPARAM lParam)
{
	CString str;
	LPAGE_ERROR lpData = (LPAGE_ERROR)wParam;

	str.Format(_T("Error:[%d] %s"), lpData->err, lpData->msg);
	m_dlgChat.AddChatMessage(0, str);
	delete[] lpData->msg;
	delete lpData;

	return 0;
}

LRESULT CVideoDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
    CString str;
    LPAGE_USER_JOINED lpData = (LPAGE_USER_JOINED)wParam;

	delete lpData;

	return 0;
}

LRESULT CVideoDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_OFFLINE lpData = (LPAGE_USER_OFFLINE)wParam;

	POSITION pos = m_listWndInfo.GetHeadPosition();
	while (pos != NULL){
		if (m_listWndInfo.GetAt(pos).nUID == lpData->uid) {
			m_listWndInfo.RemoveAt(pos);
			RebindVideoWnd();
			break;
		}

		m_listWndInfo.GetNext(pos);
	}

	CAgoraObject::GetAgoraObject()->RemoveAVSInfo(lpData->uid);

	delete lpData;

	return 0;
}

LRESULT CVideoDlg::OnEIDConnectionLost(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CVideoDlg::OnEIDVideoDeviceChanged(WPARAM wParam, LPARAM lParam)
{
	LPAGE_VIDEO_DEVICE_STATE_CHANGED lpData = (LPAGE_VIDEO_DEVICE_STATE_CHANGED)wParam;

 	m_dlgDevice.ShowWindow(SW_SHOW);
 	m_dlgDevice.CenterWindow();

	delete [](lpData->deviceId);
	delete lpData;

	return 0;
}

LRESULT CVideoDlg::OnEIDRemoteVideoStat(WPARAM wParam, LPARAM lParam)
{
	LPAGE_REMOTE_VIDEO_STAT lpData = (LPAGE_REMOTE_VIDEO_STAT)wParam;

	for (int nIndex = 0; nIndex < 16; nIndex++) {
		if (m_wndVideo[nIndex].GetUID() == lpData->uid) {
			m_wndVideo[nIndex].SetFrameRateInfo(lpData->receivedFrameRate);
			m_wndVideo[nIndex].SetBitrateInfo(lpData->receivedBitrate);
			m_wndVideo[nIndex].SetVideoResolution(lpData->width, lpData->height);
			break;
		}
	}

/*	while (posNext != NULL) {
		AGVIDEO_WNDINFO &rWndInfo = m_listWndInfo.GetNext(posNext);

		if (rWndInfo.nUID == lpData->uid) {
			rWndInfo.nFramerate = lpData->receivedFrameRate;
			rWndInfo.nBitrate = lpData->receivedBitrate;
			rWndInfo.nWidth = lpData->width;
			rWndInfo.nHeight = lpData->height;
			m_wndVideo[rWndInfo.nIndex].SetFrameRateInfo(rWndInfo.nFramerate);
			m_wndVideo[rWndInfo.nIndex].SetBitrateInfo(rWndInfo.nBitrate);
			m_wndVideo[rWndInfo.nIndex].SetVideoResolution(rWndInfo.nWidth, rWndInfo.nHeight);
			break;
		}
	}
*/
	delete lpData;

	return 0;
}

LRESULT CVideoDlg::OnEIDStartRecordingService(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

LRESULT CVideoDlg::OnEIDStopRecordingService(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

LRESULT CVideoDlg::OnEIDApiCallExecuted(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


LRESULT CVideoDlg::OnEIDRequestChannelKey(WPARAM wParam, LPARAM lParam)
{
	CAgoraObject *lpAgoraObject = CAgoraObject::GetAgoraObject();

	//CStringA strChannelKey = lpAgoraObject->GetDynChannelKey(lpAgoraObject->GetChanelName(), lpAgoraObject->GetAppID(), lpAgoraObject->GetAppCert());
	
	//CAgoraObject::GetEngine()->renewChannelKey(strChannelKey);

	return 0;
}

LRESULT CVideoDlg::OnEIDClientRoleChanged(WPARAM wParam, LPARAM lParam)
{
	if (m_dlgChat.GetSafeHwnd() != NULL)
		m_dlgChat.AddChatMessage(0, _T("Role Changed Success"));

	return 0;
}

void CVideoDlg::DrawHead(CDC *pDC)
{
	CRect rcTitle;
	CString strTitle;

	if (m_bFullScreen)
		return;

	GetWindowText(strTitle);
	GetClientRect(&rcTitle);
	CBrush *lpDefaultBrush = pDC->SelectObject(&m_brHead);

	rcTitle.bottom = 24;
	pDC->FillRect(&rcTitle, &m_brHead);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(RGB(0xFF, 0xFF, 0xFF));
	pDC->DrawText(strTitle, &rcTitle, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	pDC->SelectObject(lpDefaultBrush);
}

void CVideoDlg::InitCtrls()
{
	CRect	rcClient;
	int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);
	
	GetClientRect(&rcClient);
	
	m_btnMode.Create(NULL, WS_VISIBLE | WS_CHILD, CRect(0, 0, 1, 1), this, IDC_BTNMODE_VIDEO);
	m_btnAudio.Create(NULL, WS_VISIBLE | WS_CHILD, CRect(0, 0, 1, 1), this, IDC_BTNAUDIO_VIDEO);
	m_btnEndCall.Create(NULL, WS_VISIBLE | WS_CHILD, CRect(0, 0, 1, 1), this, IDC_BTNENDCALL_VIDEO);
	m_btnScrCap.Create(NULL, WS_VISIBLE | WS_CHILD, CRect(0, 0, 1, 1), this, IDC_BTNSCRCAP_VIDEO);
    m_btnMore.Create(NULL, WS_VISIBLE | WS_CHILD, CRect(0, 0, 1, 1), this, IDC_BTNMORE_VIDEO);

	m_btnShow.Create(NULL, WS_VISIBLE | WS_CHILD, CRect(0, 0, 1, 1), this, IDC_BTNSCR_VIDEO);
	
	for (int nIndex = 0; nIndex < 16; nIndex++){
		m_wndVideo[nIndex].Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, IDC_BASEWND_VIDEO + nIndex);
		m_wndVideo[nIndex].SetBackImage(IDB_BACKGROUND_VIDEO, 96, 96, RGB(0x44, 0x44, 0x44));
		m_wndVideo[nIndex].SetFaceColor(RGB(0x58, 0x58, 0x58));
	}

	m_wndLocal.Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 1, 1), this, IDC_BASEWND_VIDEO + 4);
	m_wndLocal.SetBackImage(IDB_BACKGROUND_VIDEO, 96, 96, RGB(0x44, 0x44, 0x44));
	m_wndLocal.SetFaceColor(RGB(0x58, 0x58, 0x58));
	m_wndLocal.SetUID(0);

	m_btnMin.MoveWindow(rcClient.Width() - 72, 1, 22, 22, TRUE);
	m_btnRst.MoveWindow(rcClient.Width() - 48, 1, 22, 22, TRUE);
	m_btnClose.MoveWindow(rcClient.Width() - 24, 1, 22, 22, TRUE);

    //m_btnMessage.MoveWindow(rcClient.Width() / 2 - 144, rcClient.Height() - 84, 48, 48, TRUE);
    m_btnMode.MoveWindow(rcClient.Width() / 2 - 24, rcClient.Height() - 84, 48, 48, TRUE);
    m_btnAudio.MoveWindow(rcClient.Width() / 2 + 24, rcClient.Height() - 84, 48, 48, TRUE);
    m_btnShow.MoveWindow(rcClient.Width() - 72, rcClient.Height() - 84, 48, 48, TRUE);
    m_btnMore.MoveWindow(rcClient.Width() / 2 + 264, rcClient.Height() - 84, 48, 48, TRUE);
    m_btnEndCall.MoveWindow(rcClient.Width() - 120, rcClient.Height() - 84, 48, 48, TRUE);
    
	m_wndVideo[0].MoveWindow(0, 24, rcClient.Width(), rcClient.Height() - 96, TRUE);

	m_btnMin.SetBackImage(IDB_BTNMIN, RGB(0xFF, 0x00, 0xFF));
	m_btnRst.SetBackImage(IDB_BTNMAX, RGB(0xFF, 0x00, 0xFF));
	m_btnClose.SetBackImage(IDB_BTNCLOSE, RGB(0xFF, 0x00, 0xFF));

	m_btnScrCap.SetBackColor(RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26));
	m_btnScrCap.EnableFrameEffect(FALSE);
	m_btnScrCap.SetBackImage(IDB_BTNSCRCAP_VIDEO, RGB(0x26, 0x26, 0x26));

    /*m_btnMessage.SetBackColor(RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26));
    m_btnMessage.EnableFrameEffect(FALSE);
    m_btnMessage.SetBackImage(IDB_BTNMSG_VIDEO, RGB(0x26, 0x26, 0x26));*/

	m_btnMode.SetBackColor(RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26));
	m_btnMode.EnableFrameEffect(FALSE);
	m_btnMode.SetBackImage(IDB_BTNAUDIO_VIDEO, RGB(0x26, 0x26, 0x26));

	m_btnAudio.SetBackColor(RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26));
	m_btnAudio.EnableFrameEffect(FALSE);
	m_btnAudio.SetBackImage(IDB_BTNMAUDIO_VIDEO, RGB(0x26, 0x26, 0x26));
	
	m_btnShow.SetBackColor(RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26));
	m_btnShow.EnableFrameEffect(FALSE);
	m_btnShow.SetBackImage(IDB_BTNFULLSCR_VIDEO, RGB(0x26, 0x26, 0x26));

	m_btnEndCall.SetBackColor(RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26));
	m_btnEndCall.EnableFrameEffect(FALSE);
	m_btnEndCall.SetBackImage(IDB_BTNENDCALL_VIDEO, RGB(0x26, 0x26, 0x26));
   
    m_btnMore.SetBackColor(RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26), RGB(0x26, 0x26, 0x26));
    m_btnMore.EnableFrameEffect(FALSE);
    m_btnMore.SetBackImage(IDB_BTNMORE_VIDEO, RGB(0x26, 0x26, 0x26));
}

void CVideoDlg::ShowVideo1()
{
	m_wndLocal.ShowWindow(SW_HIDE);
	m_wndLocal.SetBigShowFlag(FALSE);
	for (int nIndex = 0; nIndex < 4; nIndex++) {
		m_wndVideo[nIndex].ShowWindow(SW_HIDE);
		m_wndVideo[nIndex].SetBigShowFlag(FALSE);
	}

	if (m_listWndInfo.GetCount() == 0) {
		m_wndLocal.MoveWindow(&m_rcVideoArea, FALSE);
		m_wndLocal.SetParent(this);
		m_wndLocal.SetBigShowFlag(TRUE);
		m_wndLocal.ShowWindow(SW_SHOW);
	}
	else {
		m_wndVideo[0].MoveWindow(&m_rcVideoArea, FALSE);
		m_wndVideo[0].ShowWindow(SW_SHOW);
		m_wndVideo[0].SetBigShowFlag(TRUE);
		m_wndVideo[0].SetParent(this);

		m_wndLocal.MoveWindow(&m_rcChildVideoArea, FALSE);
		m_wndLocal.SetParent(&m_wndVideo[0]);
		m_wndLocal.ShowWindow(SW_SHOW);
		m_wndLocal.SetParent(&m_wndVideo[0]);
	}

	m_nScreenMode = SCREEN_VIDEO1;

	ShowButtonsNormal();
}

void CVideoDlg::ShowVideo4()
{
	m_wndLocal.ShowWindow(SW_HIDE);
	m_wndLocal.SetBigShowFlag(FALSE);
	for (int nIndex = 0; nIndex < 4; nIndex++) {
		m_wndVideo[nIndex].ShowWindow(SW_SHOW);
		m_wndVideo[nIndex].SetBigShowFlag(FALSE);
		m_wndVideo[nIndex].SetParent(this);
	}

	m_wndVideo[0].MoveWindow(0, m_rcVideoArea.top, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);
	m_wndVideo[1].MoveWindow(m_rcVideoArea.Width() / 2, m_rcVideoArea.top, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);
	m_wndVideo[2].MoveWindow(0, m_rcVideoArea.top + m_rcVideoArea.Height() / 2, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);
	m_wndVideo[3].MoveWindow(m_rcVideoArea.Width() / 2, m_rcVideoArea.top + m_rcVideoArea.Height() / 2, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);

	CRect	rcLocal;
	int		nRemoteCount = m_listWndInfo.GetCount();
	
	if (nRemoteCount == 2)
		m_wndLocal.MoveWindow(0, m_rcVideoArea.top + m_rcVideoArea.Height() / 2, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);
	else
		m_wndLocal.MoveWindow(m_rcVideoArea.Width() / 2, m_rcVideoArea.top + m_rcVideoArea.Height() / 2, m_rcVideoArea.Width() / 2, m_rcVideoArea.Height() / 2, FALSE);

	m_wndVideo[nRemoteCount].ShowWindow(SW_HIDE);
	m_wndLocal.SetParent(this);
	m_wndLocal.ShowWindow(SW_SHOW);

	m_nScreenMode = SCREEN_VIDEO4;

	ShowButtonsNormal();
	
}

void CVideoDlg::ShowMulti()
{
	int nLocalIndex = 0;
	CRect	rcLeftHalf;
	CRect	rcRightHalf;
	CAgoraObject *lpAgoraObject = CAgoraObject::GetAgoraObject();
	int		nVideoWidth = m_rcVideoArea.Width()/2;
	int		nSmallVideoWidth = nVideoWidth / 4;
	int		nSmallVideoHeight = m_rcVideoArea.Height() / 4;
	int		nLowStreamType = lpAgoraObject->GetLowStreamType();

	m_wndLocal.ShowWindow(SW_SHOW);
	m_wndLocal.SetBigShowFlag(FALSE);
	for (int nIndex = 0; nIndex < 16; nIndex++)
		m_wndVideo[nIndex].SetBigShowFlag(FALSE);
	
	if (m_lpBigShowed == NULL)
		m_lpBigShowed = &m_wndVideo[0];

	rcLeftHalf.SetRect(m_rcVideoArea.left, m_rcVideoArea.top, m_rcVideoArea.left + nVideoWidth, m_rcVideoArea.bottom);
	rcRightHalf.SetRect(m_rcVideoArea.left + nVideoWidth, m_rcVideoArea.top, m_rcVideoArea.right, m_rcVideoArea.bottom);
	
	m_lpBigShowed->ShowWindow(SW_SHOW);
	m_lpBigShowed->MoveWindow(&rcLeftHalf, TRUE);
	m_lpBigShowed->SetParent(this);
	m_lpBigShowed->SetBigShowFlag(TRUE);
	
	switch (nLowStreamType)
	{
	case 0:
		lpAgoraObject->SetRemoteVideoStreamType(m_lpBigShowed->GetUID(), REMOTE_VIDEO_STREAM_HIGH);
		break;
	case 1:
		lpAgoraObject->SetRemoteVideoStreamType(m_lpBigShowed->GetUID(), REMOTE_VIDEO_STREAM_LOW);
		break;
	case 2:
		break;
	default:
		lpAgoraObject->SetRemoteVideoStreamType(m_lpBigShowed->GetUID(), REMOTE_VIDEO_STREAM_HIGH);
		break;
	}
	
	for (int nIndex = 0; nIndex < 16; nIndex++) {
		int nXPos = rcRightHalf.left + nSmallVideoWidth * (nLocalIndex % 4);
		int nYPos = rcRightHalf.top + nSmallVideoHeight *(nLocalIndex / 4);

		if (!m_wndVideo[nIndex].IsBigShow()) {
			m_wndVideo[nIndex].MoveWindow(nXPos, nYPos, nSmallVideoWidth, nSmallVideoHeight, TRUE);
			m_wndVideo[nIndex].ShowWindow(SW_SHOW);
			m_wndVideo[nIndex].SetParent(this);
			
			if (m_wndVideo[nIndex].GetUID() != 0 && m_wndVideo[nIndex].GetUID() != m_lpBigShowed->GetUID()) {
				switch (nLowStreamType)
				{
				case 0:
					lpAgoraObject->SetRemoteVideoStreamType(m_wndVideo[nIndex].GetUID(), REMOTE_VIDEO_STREAM_LOW);
					break;
				case 1:
					lpAgoraObject->SetRemoteVideoStreamType(m_wndVideo[nIndex].GetUID(), REMOTE_VIDEO_STREAM_LOW);
					break;
				case 2:
					break;
				default:
					lpAgoraObject->SetRemoteVideoStreamType(m_wndVideo[nIndex].GetUID(), REMOTE_VIDEO_STREAM_LOW);
					break;
				}
			}
		}
		else{
			m_wndLocal.MoveWindow(nXPos, nYPos, nSmallVideoWidth, nSmallVideoHeight, TRUE);
			m_wndLocal.ShowWindow(SW_SHOW);
			m_wndLocal.SetParent(this);
		}

		nLocalIndex++;
	}

	m_nScreenMode = SCREEN_VIDEOMULTI;

	ShowButtonsNormal();
}
void CVideoDlg::ShowMultiNew()
{
	int nLocalIndex = 0;
	CRect	rcTopHalf;
	CRect	rcBottomHalf;
	CAgoraObject *lpAgoraObject = CAgoraObject::GetAgoraObject();
	int		nVideoWidth = m_rcVideoArea.Width();
	int		nVideoHight = m_rcVideoArea.Height() / 2;
	int		nSmallVideoWidth = nVideoWidth /8 ;
	int		nSmallVideoHeight = nVideoHight / 2;
	int		nLowStreamType = lpAgoraObject->GetLowStreamType();

	m_wndLocal.ShowWindow(SW_SHOW);
	m_wndLocal.SetBigShowFlag(FALSE);
	for (int nIndex = 0; nIndex < 16; nIndex++)
		m_wndVideo[nIndex].SetBigShowFlag(FALSE);

	if (m_lpBigShowed == NULL)
		m_lpBigShowed = &m_wndVideo[0];

	rcTopHalf.SetRect(m_rcVideoArea.left, m_rcVideoArea.top, m_rcVideoArea.right, m_rcVideoArea.top + nVideoHight);
	rcBottomHalf.SetRect(m_rcVideoArea.left, m_rcVideoArea.top + nVideoHight, m_rcVideoArea.right, m_rcVideoArea.bottom);

	m_lpBigShowed->ShowWindow(SW_SHOW);
	m_lpBigShowed->MoveWindow(&rcBottomHalf, TRUE);
	m_lpBigShowed->SetParent(this);
	m_lpBigShowed->SetBigShowFlag(TRUE);

	switch (nLowStreamType)
	{
	case 0:
		lpAgoraObject->SetRemoteVideoStreamType(m_lpBigShowed->GetUID(), REMOTE_VIDEO_STREAM_HIGH);
		break;
	case 1:
		lpAgoraObject->SetRemoteVideoStreamType(m_lpBigShowed->GetUID(), REMOTE_VIDEO_STREAM_LOW);
		break;
	case 2:
		break;
	default:
		lpAgoraObject->SetRemoteVideoStreamType(m_lpBigShowed->GetUID(), REMOTE_VIDEO_STREAM_HIGH);
		break;
	}

	for (int nIndex = 0; nIndex < 16; nIndex++) {
		int nXPos = rcTopHalf.left + nSmallVideoWidth * (nLocalIndex % 8);
		int nYPos = rcTopHalf.top + nSmallVideoHeight *(nLocalIndex / 8);

		if (!m_wndVideo[nIndex].IsBigShow()) {
			m_wndVideo[nIndex].MoveWindow(nXPos, nYPos, nSmallVideoWidth, nSmallVideoHeight, TRUE);
			m_wndVideo[nIndex].ShowWindow(SW_SHOW);
			m_wndVideo[nIndex].SetParent(this);

			if (m_wndVideo[nIndex].GetUID() != 0 && m_wndVideo[nIndex].GetUID() != m_lpBigShowed->GetUID()) {
				switch (nLowStreamType)
				{
				case 0:
					lpAgoraObject->SetRemoteVideoStreamType(m_wndVideo[nIndex].GetUID(), REMOTE_VIDEO_STREAM_LOW);
					break;
				case 1:
					lpAgoraObject->SetRemoteVideoStreamType(m_wndVideo[nIndex].GetUID(), REMOTE_VIDEO_STREAM_LOW);
					break;
				case 2:
					break;
				default:
					lpAgoraObject->SetRemoteVideoStreamType(m_wndVideo[nIndex].GetUID(), REMOTE_VIDEO_STREAM_LOW);
					break;
				}
			}
		}
		else{
			m_wndLocal.MoveWindow(nXPos, nYPos, nSmallVideoWidth, nSmallVideoHeight, TRUE);
			m_wndLocal.ShowWindow(SW_SHOW);
			m_wndLocal.SetParent(this);
		}

		nLocalIndex++;
	}

	m_nScreenMode = SCREEN_VIDEOMULTI;

	ShowButtonsNormal();
}

BOOL CVideoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_dlgDevice.Create(CDeviceDlg::IDD, this);
	m_dlgDevice.EnableDeviceTest(FALSE);

	m_dlgScreenCapture.Create(CAGScreenCaptureDlg::IDD, this);
	m_dlgChat.Create(CChatDlg::IDD, this);
	m_dlgSEI.Create(CSEIDlg::IDD, this);
	InitCtrls();
    
    m_bitMenuInfo.LoadBitmap(IDB_MENU_INFO);
    m_bitMenuDevice.LoadBitmap(IDB_MENU_DEVICE);
    m_bitMenuFilter.LoadBitmap(IDB_MENU_FILTER);
	m_bitMenuMix.LoadBitmap(IDB_MENU_MIX);
	m_bitMenuSEI.LoadBitmap(IDB_MENU_SEI);
	m_bitMenuBCaster.LoadBitmap(IDB_MENU_BROADER);
	m_bitMenuAudRec.LoadBitmap(IDB_MENU_AUDREC);
	m_bitMenuVidRec.LoadBitmap(IDB_MENU_VIDREC);
	m_bitMenuVExtSet.LoadBitmap(IDB_MENU_VEXTSET);


	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


HWND CVideoDlg::GetRemoteVideoWnd(int nIndex)
{
	if (nIndex < 0 || nIndex > 3)
		return NULL;

	return m_wndVideo[nIndex].GetSafeHwnd();
}


void CVideoDlg::RebindVideoWnd()
{
	if (m_wndVideo[0].GetSafeHwnd() == NULL || m_wndLocal.GetSafeHwnd() == NULL)
		return;

	VideoCanvas canvas;
	RENDER_MODE_TYPE localRenderType;
	RENDER_MODE_TYPE remoteRenderType;

	CAgoraObject::GetAgoraObject()->GetRenderMode(&localRenderType, &remoteRenderType);
	canvas.renderMode = remoteRenderType;

	POSITION pos = m_listWndInfo.GetHeadPosition();
	for (int nIndex = 0; nIndex < 16; nIndex++) {
		if (pos != NULL) {
			AGVIDEO_WNDINFO &agvWndInfo = m_listWndInfo.GetNext(pos);
			canvas.uid = agvWndInfo.nUID;
			canvas.view = m_wndVideo[nIndex].GetSafeHwnd();
			agvWndInfo.nIndex = nIndex;

			CAgoraObject::GetEngine()->setupRemoteVideo(canvas);

			m_wndVideo[nIndex].SetUID(canvas.uid);
			m_wndVideo[nIndex].SetVideoResolution(agvWndInfo.nWidth, agvWndInfo.nHeight);
			m_wndVideo[nIndex].SetFrameRateInfo(agvWndInfo.nFramerate);
			m_wndVideo[nIndex].SetBitrateInfo(agvWndInfo.nBitrate);

			AVSTATUS_INFO avsInfo;

			avsInfo.nUID = canvas.uid;
			avsInfo.nRenderMode = remoteRenderType;
			avsInfo.bMuteVideo = FALSE;
			avsInfo.bMuteAudio = FALSE;
			CAgoraObject::GetAgoraObject()->SetAVSInfo(avsInfo.nUID, &avsInfo);
		}
		else
			m_wndVideo[nIndex].SetUID(0);
	}


	if (m_nScreenMode != SCREEN_VIDEOMULTI) {
		if (m_listWndInfo.GetCount() <= 1)
			ShowVideo1();
		else if (m_listWndInfo.GetCount() > 1 && m_listWndInfo.GetCount() < 4)
			ShowVideo4();
		else
			ShowMultiNew();
	}
	else
		ShowMultiNew();
}


BOOL CVideoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN){
		switch (pMsg->wParam){
		case VK_RETURN:
			return FALSE;
		case VK_ESCAPE:
			if (m_bFullScreen)
				OnBnClickedBtnfullscr();
			
			return FALSE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT CVideoDlg::OnShowModeChanged(WPARAM wParam, LPARAM lParam)
{
	if (m_nScreenMode == SCREEN_VIDEOMULTI) {
		if (m_listWndInfo.GetCount() <= 1)
			ShowVideo1();
		else if (m_listWndInfo.GetCount() > 1 && m_listWndInfo.GetCount() < 4)
			ShowVideo4();
	}
	else {
		m_lpBigShowed = (CAGVideoWnd *)wParam;
		ShowMultiNew();
	}
	
	return 0;
}

LRESULT CVideoDlg::OnShowBig(WPARAM wParam, LPARAM lParam)
{
	CRect rcNextBig;
	CRect rcCurBig;

	if (m_nScreenMode != SCREEN_VIDEOMULTI)
		return 0;

	CAGVideoWnd *lpNextBigWnd = (CAGVideoWnd *)wParam;
	if (lpNextBigWnd == m_lpBigShowed)
		return 0;

	m_lpBigShowed = lpNextBigWnd;
	ShowMultiNew();

	return 0;
}

void CVideoDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO:  在此处添加消息处理程序代码

	if (bShow && GetSafeHwnd() != NULL)
		RebindVideoWnd();

}


void CVideoDlg::OnMove(int x, int y)
{
    CDialogEx::OnMove(x, y);

    // TODO:  在此处添加消息处理程序代码
    CRect rcChatBox;

    rcChatBox.SetRect(x, y + m_rcVideoArea.Height() - 126, x + m_rcVideoArea.Width(), y + m_rcVideoArea.Height() + 24);

    if (::IsWindow(m_dlgChat.GetSafeHwnd()) && m_dlgChat.IsWindowVisible()){
        m_dlgChat.MoveWindow(&rcChatBox);
    }
}

void CVideoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnTimer(nIDEvent);
}
LRESULT CVideoDlg::OnWindowShareStart(WPARAM wParam, LPARAM lParam)
{
    HWND hShareWnd = (HWND)wParam;
    if (hShareWnd == GetSafeHwnd()) {
        CAgoraObject::GetAgoraObject()->EnableLocalRender(FALSE);
        m_wndLocal.Invalidate(TRUE);
    }

    CAgoraObject::GetAgoraObject()->EnableVideo();
    CAgoraObject::GetAgoraObject()->EnableScreenCapture(hShareWnd, 15, NULL, TRUE);

    m_btnScrCap.SwitchButtonStatus(CAGButton::AGBTN_PUSH);

    return 0;
}

LRESULT CVideoDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	LPAGE_LEAVE_CHANNEL lpData = (LPAGE_LEAVE_CHANNEL)wParam;
	delete lpData;
	lpData = NULL;
	return 0;
}