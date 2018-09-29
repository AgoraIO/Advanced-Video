
// AgoraHookingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraHooking.h"
#include "AgoraHookingDlg.h"
#include "afxdialogex.h"
#include "AssistantBox.h"
#include "ExtendAudioFrameObserver.h"
#include "CHookPlayerInstance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAgoraHookingDlg dialog



CAgoraHookingDlg::CAgoraHookingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAgoraHookingDlg::IDD, pParent),
	m_pAssistantBox(nullptr),
	m_bIsCapture(FALSE),
	m_strInstance(gInstance),
	m_lpExtendAudioFrame(nullptr),
	m_lpHookPlayerInstance(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAgoraHookingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAgoraHookingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_NCLBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CLOSE,onButtonCloseClicked)
	ON_BN_CLICKED(IDC_BUTTON_MIN,onButtonMinClicked)

	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CAgoraHookingDlg::OnJoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_REJOINCHANNEL_SUCCESS), &CAgoraHookingDlg::OnRejoinChannelSuccess)
	ON_MESSAGE(WM_MSGID(EID_ERROR), &CAgoraHookingDlg::OnError)
	ON_MESSAGE(WM_MSGID(EID_AUDIO_QUALITY), &CAgoraHookingDlg::OnAudioQuality)
	ON_MESSAGE(WM_MSGID(EID_AUDIO_VOLUME_INDICATION), &CAgoraHookingDlg::OnAudioVolumeIndication)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CAgoraHookingDlg::OnLeaveChannel)
	ON_MESSAGE(WM_MSGID(EID_MEDIA_ENGINE_EVENT), &CAgoraHookingDlg::OnMediaEngineEvent)
	ON_MESSAGE(WM_MSGID(EID_AUDIO_DEVICE_STATE_CHANGED), &CAgoraHookingDlg::OnAudioDeviceStateChanged)
	ON_MESSAGE(WM_MSGID(EID_FIRST_LOCAL_VIDEO_FRAME), &CAgoraHookingDlg::OnFirstLocalVideoFrame)
	ON_MESSAGE(WM_MSGID(EID_REQUEST_CHANNELKEY), &CAgoraHookingDlg::OnRequestChannelKey)
	ON_MESSAGE(WM_MSGID(EID_LASTMILE_QUALITY), &CAgoraHookingDlg::OnLastmileQuality)
	ON_MESSAGE(WM_MSGID(EID_FIRST_REMOTE_VIDEO_DECODED), &CAgoraHookingDlg::OnFirstRemoteVideoDecoded)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraHookingDlg::OnUserJoined)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraHookingDlg::OnUserOffline)
	ON_MESSAGE(WM_MSGID(EID_USER_MUTE_AUDIO), &CAgoraHookingDlg::OnUserMuteAudio)
	ON_MESSAGE(WM_MSGID(EID_USER_MUTE_VIDEO), &CAgoraHookingDlg::OnUserMuteVideo)
	ON_MESSAGE(WM_MSGID(EID_LOCAL_VIDEO_STAT), &CAgoraHookingDlg::OnLocalVideoStats)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STAT), &CAgoraHookingDlg::OnRemoteVideoStats)
	ON_MESSAGE(WM_MSGID(EID_INVITER_JOINCHANNEL),&CAgoraHookingDlg::OnInviterJoinChannel)
END_MESSAGE_MAP()


// CAgoraHookingDlg message handlers

BOOL CAgoraHookingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_strAppId = gConfigHook.getAppId();
	m_strAppcertificateId = gConfigHook.getAppCertificateId();
	m_strChannel = gConfigHook.getChannelName();
	m_uLoginUid = str2int(gConfigHook.getLoginUid(m_strInstance));

	if ((0 == m_strAppId.length() || 0 == m_strChannel.length() || 0 == m_uLoginUid)){
		CAgoraFormatStr::AgoraMessageBox(_T("There are one empty AppId, Channel, and LoginUID in the configuration file. Please modify"));
		PostQuitMessage(0);
		return FALSE;
	}

	initCtrl();
	ShowWindow(SW_HIDE);
	SetTimer(1, 10, NULL);

	initResource();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAgoraHookingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAgoraHookingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAgoraHookingDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bIsCapture = TRUE;
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\n");
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CAgoraHookingDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bIsCapture = FALSE;
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\n");

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CAgoraHookingDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (nullptr != m_lpAgoraObject) {

		RECT rt;
		GetClientRect(&rt);
		ClientToScreen(&rt);
		RECT rtAssistangBox;
		m_pAssistantBox->GetClientRect(&rtAssistangBox);
		ShowWindow(SW_HIDE);
		//m_pAssistantBox->MoveWindow(rt.right, rt.top, rtAssistangBox.right - rtAssistangBox.left, rt.bottom - rt.top);
		m_pAssistantBox->CenterWindow(this);
		m_pAssistantBox->ShowWindow(SW_SHOW);
	}
}

void CAgoraHookingDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bIsCapture && m_pAssistantBox->isShow()) {
		OutputDebugString(_T(__FUNCTION__));
		OutputDebugString(_T("\n"));

		RECT rt;
		GetClientRect(&rt);
		ClientToScreen(&rt);
		char logMsg[256] = { '\0' };
		RECT rtAssistangBox;
		m_pAssistantBox->GetWindowRect(&rtAssistangBox);

		sprintf_s(logMsg, "WndRect(%d,%d,%d,%d) ; BoxRect(%d,%d,%d,%d)", rt.left, rt.top, rt.top, rt.bottom, rtAssistangBox.left, rtAssistangBox.top, rtAssistangBox.right, rtAssistangBox.bottom);
		OutputDebugStringA(logMsg);
		m_pAssistantBox->MoveWindow(rt.right, rt.top, rtAssistangBox.right - rtAssistangBox.left, rt.bottom - rt.top);
		m_pAssistantBox->ShowWindow(SW_SHOW);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CAgoraHookingDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {

		KillTimer(nIDEvent);
		ShowWindow(SW_MINIMIZE);
		ShowWindow(SW_HIDE);
		m_pAssistantBox->CenterWindow();
		m_pAssistantBox->ShowWindow(SW_SHOW);
	}
}

BOOL CAgoraHookingDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialogEx::PreTranslateMessage(pMsg);
}

inline void CAgoraHookingDlg::initCtrl()
{
	//To Do: initialize controls related resource

	SetBackgroundImage(IDB_DLGLOGO);

	RECT rt;
	GetClientRect(&rt);
	rt.left = rt.right - 22;
	rt.bottom = rt.top + 22;

	m_AgBtnClose.Create(L"", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,rt,theApp.GetMainWnd(),IDC_BUTTON_CLOSE);
	m_AgBtnClose.SetBackImage(IDB_BTNCLOSE);

	rt.left = rt.left - 22;
	rt.right = rt.left + 22;
	m_AgBtnMin.Create(L"", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE , rt, theApp.GetMainWnd(), IDC_BUTTON_MIN);
	m_AgBtnMin.SetBackImage(IDB_BTNMIN);
	
	GetClientRect(&rt);
	m_hWndTitle = CreateWindow(L"Static", L"", WS_CHILD | WS_VISIBLE | SS_LEFT,rt.left,rt.top,300,22,m_hWnd,NULL,NULL,NULL);
	m_hWndLeftRemote = CreateWindow(L"Static", L"Remote", WS_CHILD | WS_VISIBLE | SS_CENTER, rt.left, rt.top + 22, (rt.right - rt.left) / 2, rt.bottom - rt.top - 22, m_hWnd, NULL,NULL, NULL);
	m_hWndRightSelf = CreateWindow(L"Static", L"Self", WS_CHILD | WS_VISIBLE | SS_CENTER, (rt.right - rt.left) / 2, rt.top + 22, (rt.right - rt.left) / 2, rt.bottom - rt.top - 22, m_hWnd, NULL, NULL, NULL);
	::ShowWindow(m_hWndLeftRemote, SW_HIDE);
	::MoveWindow(m_hWndRightSelf, rt.left, rt.top + 22, rt.right - rt.left, rt.bottom - rt.top - 22,TRUE);	

	if (NULL == m_pAssistantBox) {

		m_pAssistantBox = new CAssistantBox;
		m_pAssistantBox->Create(CAssistantBox::IDD);
		m_pAssistantBox->setWnd(m_hWndRightSelf);
	}
}

inline void CAgoraHookingDlg::uninitCtrl()
{
	if (NULL != m_pAssistantBox) {

		delete m_pAssistantBox;
		m_pAssistantBox = nullptr;
	}
}

inline void CAgoraHookingDlg::initResource()
{
	m_lpAgoraObject->EnableExtendAudioCapture(FALSE, NULL);
	if (m_lpExtendAudioFrame) {
		delete m_lpExtendAudioFrame;
		m_lpExtendAudioFrame = nullptr;
	}

	m_lpHookPlayerInstance = CHookPlayerInstance::getHookInstance();

	m_lpAgoraObject = CAgoraObject::GetAgoraObject(s2cs(m_strAppId));
	ASSERT(m_lpAgoraObject);
	m_lpAgoraObject->SetMsgHandlerWnd(m_hWnd);

	m_lpRtcEngine = CAgoraObject::GetEngine();
	ASSERT(m_lpRtcEngine);

	CString strSdkLogFilePath = s2cs(getMediaSdkLogPath());
	m_lpAgoraObject->SetLogFilePath(strSdkLogFilePath);

	m_lpAgoraObject->EnableWebSdkInteroperability(TRUE);
	m_lpAgoraObject->EnableLocalMirrorImage(FALSE);

	m_lpAgoraObject->EnableVideo(FALSE);
	m_lpAgoraObject->EnableAudio(TRUE);

	m_lpAgoraObject->SetVideoRenderType(1);//DISPLAY_GDI
	m_lpAgoraObject->SetVideoProfileEx(800, 600, 15, 900);
	m_lpAgoraObject->SetAudioProfile(AUDIO_PROFILE_MUSIC_HIGH_QUALITY_STEREO, AUDIO_SCENARIO_SHOWROOM);

	m_lpAgoraObject->EnableLastmileTest(FALSE);
	m_lpAgoraObject->SetClientRole(CLIENT_ROLE_TYPE::CLIENT_ROLE_BROADCASTER);
	m_lpAgoraObject->SetChannelProfile(TRUE);

	m_lpAgoraObject->setAudioAEC(FALSE);
	m_lpAgoraObject->setAudioAgcOn(FALSE);
	m_lpAgoraObject->setAudioNS(FALSE);

	//RTMP
	BOOL bEnableRotate = str2int(gConfigHook.getLeftRotate90(m_strInstance));
	BOOL bEnableRtmp = str2int(gConfigHook.getRtmpSave(m_strInstance));
	if (bEnableRtmp){

		int nRtmpWidth = str2int(gConfigHook.getRtmpWidth(m_strInstance));
		int nRtmpHeight = str2int(gConfigHook.getRtmpHeight(m_strInstance));
		int nRtmpFps = str2int(gConfigHook.getRtmpFps(m_strInstance));
		int nRtmpBitrate = str2int(gConfigHook.getResolutionBitrate(m_strInstance));
		std::string strRtmpURL = gConfigHook.getRtmpUrl(m_strInstance);
		AGE_PUBLISH_PARAM pp;
		pp.bitrate = nRtmpBitrate;
		pp.fps = nRtmpFps;
		pp.height = bEnableRotate ? nRtmpWidth : nRtmpHeight;
		pp.width = bEnableRotate ? nRtmpHeight : nRtmpWidth;
		pp.rtmpUrl = strRtmpURL;
		m_lpAgoraObject->setPublishParam(pp);
		m_lpAgoraObject->enablePublish(true);

		CAgoraFormatStr::AgoraWriteLog(("RtmpParam Width: %d,Height: %d,Fps: %d,Bitrate: %d,URL: %s"), pp.width, pp.height, pp.fps, pp.bitrate, (pp.rtmpUrl).data());
		CAgoraFormatStr::AgoraOutDebugStr(_T("Rtmp Wdith: %d,Height: %d,Fps: %d,Bitrate: %d \r\n URL : %s"), pp.width, pp.height, pp.fps, pp.bitrate, s2cs(pp.rtmpUrl));
	}

	m_lpAgoraObject->MuteLocalVideo(TRUE);
	m_lpAgoraObject->MuteAllRemoteVideo(TRUE);
	m_lpAgoraObject->EnableLocalVideo(FALSE);
}

inline void CAgoraHookingDlg::uninitResource()
{
	if (nullptr == m_lpAgoraObject){
		return;
	}

	m_lpAgoraObject->EnableVideo(FALSE);
	m_lpAgoraObject->EnableLastmileTest(FALSE);
	if (m_lpAgoraObject){
		CAgoraObject::CloseAgoraObject();
		m_lpAgoraObject = nullptr;
		m_lpRtcEngine = nullptr;
	}
}
			   
inline void CAgoraHookingDlg::getRectClient(RECT &rt)
{

}

inline void CAgoraHookingDlg::getRectClientLeft(RECT &rt)
{

}

inline void CAgoraHookingDlg::getRectClientRight(RECT &rt)
{

}

void CAgoraHookingDlg::onButtonCloseClicked()
{
	if (m_lpHookPlayerInstance) 
		m_lpHookPlayerInstance->stopHook();

	if (m_lpAgoraObject)
		m_lpAgoraObject->LeaveCahnnel();

	if (m_lpRtcEngine)
		m_lpRtcEngine->stopPreview();

	uninitCtrl();
	uninitResource();

	CDialogEx::OnCancel();
}

void CAgoraHookingDlg::onButtonMinClicked()
{
	ShowWindow(SW_MINIMIZE);
}

LRESULT CAgoraHookingDlg::OnInviterJoinChannel(WPARAM wParam, LPARAM lParam)
{
	LPAGE_INVITER_JOINCHANNEL lpData = (LPAGE_INVITER_JOINCHANNEL)wParam;
	if (lpData) {
		
		m_lpExtendAudioFrame = new CExtendAudioFrameObserver;
		if (m_lpExtendAudioFrame)
			m_lpAgoraObject->EnableExtendAudioCapture(TRUE, m_lpExtendAudioFrame);
		m_lpAgoraObject->SetAudioRecordParam(KNSampelRate, KNChannel, KNSampelPerCall);

		m_uInviter = lpData->uInviterId;
		m_lpAgoraObject->LocalVideoPreview(/*m_hWndRightSelf*/NULL, TRUE);
		m_lpAgoraObject->JoinChannel(s2cs(m_strChannel), m_uLoginUid);

		CString strDir;
		CAgoraFormatStr::QueryDir(CAgoraFormatStr::eHookPlayerType(lpData->nHookType), strDir);
		if (m_lpHookPlayerInstance) {

			m_lpHookPlayerInstance->startHook(strDir.GetBuffer());
			strDir.ReleaseBuffer();
		}
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnJoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	LPAGE_JOINCHANNEL_SUCCESS lpData = (LPAGE_JOINCHANNEL_SUCCESS)wParam;
	if (lpData) {

		char szbuf[256] = { '\0' };
		sprintf_s(szbuf, "[%s] [%d]", lpData->channel, lpData->uid);
#if 0
		::SetWindowTextW(m_hWndTitle, s2cs(szbuf));
#else
		if (m_pAssistantBox)
			m_pAssistantBox->setBoxTitle(s2cs(szbuf));
#endif

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnRejoinChannelSuccess(WPARAM wParam, LPARAM lParam)
{
	LPAGE_REJOINCHANNEL_SUCCESS lpData = (LPAGE_REJOINCHANNEL_SUCCESS)wParam;
	if (lpData) {

		CAgoraFormatStr::AgoraWriteLog("Uid: %u Channel: %s RejoinChannelSuccess", lpData->uid, lpData->channel);
		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnWarning(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHookingDlg::OnError(WPARAM wParam, LPARAM lParam)
{
	LPAGE_ERROR lpData = (LPAGE_ERROR)wParam;
	if (lpData) {
		
		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnAudioQuality(WPARAM wParam, LPARAM lParam)
{
	LPAGE_AUDIO_QUALITY lpData = (LPAGE_AUDIO_QUALITY)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnAudioVolumeIndication(WPARAM wParam, LPARAM lParam)
{
	LPAGE_AUDIO_VOLUME_INDICATION lpData = (LPAGE_AUDIO_VOLUME_INDICATION)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	LPAGE_LEAVE_CHANNEL lpData = (LPAGE_LEAVE_CHANNEL)wParam;
	if (lpData) {

		CAgoraFormatStr::AgoraWriteLog("LeaveChannel uid: %u", lpData->rtcStat.users);
		delete lpData; lpData = nullptr;
	}
	return TRUE;
}

LRESULT CAgoraHookingDlg::OnRtcStats(WPARAM wParam, LPARAM lParam)
{

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnMediaEngineEvent(WPARAM wParam, LPARAM lParam)
{
	LPAGE_MEDIA_ENGINE_EVENT lpData = (LPAGE_MEDIA_ENGINE_EVENT)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnAudioDeviceStateChanged(WPARAM wParam, LPARAM lParam)
{
	LPAGE_AUDIO_DEVICE_STATE_CHANGED lpData = (LPAGE_AUDIO_DEVICE_STATE_CHANGED)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnRequestChannelKey(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHookingDlg::OnLastmileQuality(WPARAM wParam, LPARAM lParam)
{
	LPAGE_LASTMILE_QUALITY lpData = (LPAGE_LASTMILE_QUALITY)wParam;

	if (lpData ) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnVideoDeviceStateChanged(WPARAM wParam, LPARAM lParam)
{
	LPAGE_VIDEO_DEVICE_STATE_CHANGED lpData = (LPAGE_VIDEO_DEVICE_STATE_CHANGED)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnFirstLocalVideoFrame(WPARAM wParam, LPARAM lParam)
{
	LPAGE_FIRST_LOCAL_VIDEO_FRAME lpData = (LPAGE_FIRST_LOCAL_VIDEO_FRAME)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnFirstRemoteVideoDecoded(WPARAM wParam, LPARAM lParam)
{
	LPAGE_FIRST_REMOTE_VIDEO_DECODED lpData = (LPAGE_FIRST_REMOTE_VIDEO_DECODED)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnFirstRemoteVideoFrame(WPARAM wParam, LPARAM lParam)
{
	LPAGE_FIRST_REMOTE_VIDEO_FRAME lpData = (LPAGE_FIRST_REMOTE_VIDEO_FRAME)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnUserJoined(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_JOINED lpData = (LPAGE_USER_JOINED)wParam;
	if (lpData) {

		CAgoraFormatStr::AgoraWriteLog("%s uid: %u", __FUNCTION__, lpData->uid);

		if (m_uInviter == lpData->uid || TRUE) {

			RECT rt;
			GetClientRect(&rt);
			::MoveWindow(m_hWndRightSelf, rt.right / 2, rt.top + 22, (rt.right - rt.left) / 2, rt.bottom - rt.top - 22, TRUE);

			if (m_lpRtcEngine) {
				VideoCanvas vc;
				vc.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;
				vc.uid = lpData->uid;
				vc.view = /*m_hWndLeftRemote*/NULL;
				::ShowWindow(m_hWndLeftRemote, SW_SHOW);
				m_lpRtcEngine->setupRemoteVideo(vc);
			}
		}
		
		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnUserOffline(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_OFFLINE lpData = (LPAGE_USER_OFFLINE)wParam;
	if (lpData) {

		if (m_uInviter == lpData->uid || TRUE) {

			RECT rt;
			GetClientRect(&rt);
			::MoveWindow(m_hWndRightSelf, rt.left, rt.top + 22, (rt.right - rt.left), rt.bottom - rt.top - 22, TRUE);

			if (m_lpRtcEngine) {
				VideoCanvas vc;
				vc.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;
				vc.uid = lpData->uid;
				vc.view = NULL;
				::ShowWindow(m_hWndLeftRemote, SW_HIDE);
				m_lpRtcEngine->setupRemoteVideo(vc);
			}
		}

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnUserMuteAudio(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_MUTE_AUDIO lpData = (LPAGE_USER_MUTE_AUDIO)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnUserMuteVideo(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_MUTE_VIDEO lpData = (LPAGE_USER_MUTE_VIDEO)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnApiCallExecuted(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHookingDlg::OnLocalVideoStats(WPARAM wParam, LPARAM lParam)
{
	LPAGE_LOCAL_VIDEO_STAT lpData = (LPAGE_LOCAL_VIDEO_STAT)wParam;
	if (lpData) {


		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnRemoteVideoStats(WPARAM wParam, LPARAM lParam)
{
	LPAGE_REMOTE_VIDEO_STAT lpData = (LPAGE_REMOTE_VIDEO_STAT)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnCameraReady(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHookingDlg::OnVideoStopped(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHookingDlg::OnConnectionLost(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHookingDlg::OnConnectionInterrupted(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHookingDlg::OnUserEnableVideo(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_MUTE_VIDEO lpData = (LPAGE_USER_MUTE_VIDEO)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}

LRESULT CAgoraHookingDlg::OnStartRecordingService(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHookingDlg::OnStopRecordingService(WPARAM wParam, LPARAM lParam)
{
	return TRUE;
}

LRESULT CAgoraHookingDlg::OnRefreshRecordingServiceStatus(WPARAM wParam, LPARAM lParam)
{
	LPAGE_RCDSRV_STATUS lpData = (LPAGE_RCDSRV_STATUS)wParam;
	if (lpData) {

		delete lpData; lpData = nullptr;
	}

	return TRUE;
}
