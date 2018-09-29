// AssistantBox.cpp : implementation file
//

#include "stdafx.h"
#include "AgoraHooking.h"
#include "AssistantBox.h"
#include "afxdialogex.h"

// CAssistantBox dialog

IMPLEMENT_DYNAMIC(CAssistantBox, CDialogEx)

CAssistantBox::CAssistantBox(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAssistantBox::IDD, pParent),
	m_hWndVideoTest(nullptr),
	m_hWndVideoTitle(nullptr),
	m_bIsShow(TRUE),
	m_lpHookPlayerInstance(nullptr)
{
	EnableAutomation();
}

CAssistantBox::~CAssistantBox()
{
}

void CAssistantBox::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CDialogEx::OnFinalRelease();
}

void CAssistantBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CAMERA, m_ComCameraList);
	DDX_Control(pDX, IDC_COMBO_MIC, m_ComAudioInputList);
	DDX_Control(pDX, IDC_COMBO_PLAYOUT, m_ComPlayOutList);
	DDX_Control(pDX, IDC_EDIT_UID, m_EditLoginUid);
	DDX_Control(pDX, IDC_EDIT_CHANNEL, m_EditChannel);
	DDX_Control(pDX, IDC_EDIT_INVITER, m_EditInviter);
	DDX_Control(pDX, IDC_CHECK_HOOK, m_CheckBoxHook);
	DDX_Control(pDX, IDC_COMBO_PLAYER, m_ComHookPlayerList);
}


BEGIN_MESSAGE_MAP(CAssistantBox, CDialogEx)
	ON_WM_LBUTTONDOWN()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_NCPAINT()
	ON_BN_CLICKED(IDC_BOX_BUTTON_CLOSE,onButtonCloseClicked)
	ON_BN_CLICKED(IDC_BOX_BUTTON_MIN,onButtonMinClicked)
	ON_BN_CLICKED(IDC_BUTTON_INVITE, &CAssistantBox::OnBnClickedButtonInvite)
	ON_CBN_SELCHANGE(IDC_COMBO_CAMERA, &CAssistantBox::OnCbnSelchangeComboCamera)
	ON_CBN_SELCHANGE(IDC_COMBO_MIC, &CAssistantBox::OnCbnSelchangeComboMic)
	ON_CBN_SELCHANGE(IDC_COMBO_PLAYOUT, &CAssistantBox::OnCbnSelchangeComboPlayout)
	ON_CBN_SELCHANGE(IDC_COMBO_PLAYER, &CAssistantBox::OnCbnSelchangeComboPlayer)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CAssistantBox, CDialogEx)
END_DISPATCH_MAP()

// Note: we add support for IID_IAssistantBox to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .IDL file.

// {D46F9EC8-FA7D-4412-822B-22B32853E4B5}
static const IID IID_IAssistantBox =
{ 0xD46F9EC8, 0xFA7D, 0x4412, { 0x82, 0x2B, 0x22, 0xB3, 0x28, 0x53, 0xE4, 0xB5 } };

BEGIN_INTERFACE_MAP(CAssistantBox, CDialogEx)
	INTERFACE_PART(CAssistantBox, IID_IAssistantBox, Dispatch)
END_INTERFACE_MAP()


// CAssistantBox message handlers
BOOL CAssistantBox::OnInitDialog()
{
	//TO DO
	initCtrl();
	initResource();

	return CDialogEx::OnInitDialog();
}

void CAssistantBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\n");
	PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CAssistantBox::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	OutputDebugStringA(__FUNCTION__);
	OutputDebugStringA("\n");
	CDialogEx::OnNcLButtonDown(nHitTest, point);
}

void CAssistantBox::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void CAssistantBox::OnShowWindow(BOOL bShow, UINT nStatus)
{
	m_bIsShow = bShow;
	
	m_lpAgoraObject = CAgoraObject::GetAgoraObject();
	m_lpRtcEngine = CAgoraObject::GetEngine();
	if (m_lpAgoraObject && m_lpRtcEngine) {

		m_AgoraCameraManager.Close();
		m_AgoraAudioInputManager.Close();
		m_AgoraPlayoutManager.Close();

		m_AgoraCameraManager.Create(m_lpRtcEngine);
		m_AgoraAudioInputManager.Create(m_lpRtcEngine);
		m_AgoraPlayoutManager.Create(m_lpRtcEngine);
		
		m_ComCameraList.ResetContent();
		int nDeviceCount = m_AgoraCameraManager.GetDeviceCount();
		CString strDeviceName; CString strDeviceID;
		for (int nIndex = 0; nDeviceCount > nIndex; nIndex++) {

			m_AgoraCameraManager.GetDevice(nIndex, strDeviceName, strDeviceID);
			if (0 == nIndex)
				m_AgoraCameraManager.SetCurDevice(strDeviceID);
			m_ComCameraList.AddString(strDeviceName);
		}
		m_ComCameraList.SetCurSel(0);

		m_ComAudioInputList.ResetContent();
		nDeviceCount = m_AgoraAudioInputManager.GetDeviceCount();
		for (int nIndex = 0; nDeviceCount > nIndex; nIndex++) {

			m_AgoraAudioInputManager.GetDevice(nIndex, strDeviceName, strDeviceID);
			m_ComAudioInputList.AddString(strDeviceName);
		}
		m_ComAudioInputList.SetCurSel(0);

		m_ComPlayOutList.ResetContent();
		nDeviceCount = m_AgoraPlayoutManager.GetDeviceCount();
		for (int nIndex = 0; nDeviceCount > nIndex; nIndex++) {
			
			m_AgoraPlayoutManager.GetDevice(nIndex, strDeviceName, strDeviceID);
			m_ComPlayOutList.AddString(strDeviceName);
		}
		m_ComPlayOutList.SetCurSel(0);
	}
	
	std::string uid = gConfigHook.getLoginUid(gInstance);
	m_EditLoginUid.SetWindowTextW(s2cs(uid));
	m_CheckBoxHook.SetCheck(TRUE);
	m_ComHookPlayerList.ResetContent();
	m_ComHookPlayerList.AddString(L"¿á¹·ÒôÀÖ");
	m_ComHookPlayerList.AddString(L"ÍøÒ×ÒôÀÖ");
	m_ComHookPlayerList.AddString(L"QQÒôÀÖ");
	//m_ComHookPlayerList.AddString(L"¿áÎÒÒôÀÖ");
	m_ComHookPlayerList.SetCurSel(0);

	std::string strChannel = gConfigHook.getChannelName();
	m_EditChannel.SetWindowTextW(s2cs(strChannel));

	//m_AgoraCameraManager.TestCameraDevice(m_hWndVideoTest, TRUE);

	CString strVersion = m_lpAgoraObject->GetSDKVersionEx();
	GetDlgItem(IDC_STATIC_VERSION)->SetWindowTextW(strVersion);
}

void CAssistantBox::OnNcPaint()
{
	if (!m_bIsShow)
		m_bIsShow = !m_bIsShow;

	if (m_bIsShow){
	
	}
}

inline void CAssistantBox::initCtrl()
{
	m_bIsShow = TRUE;
	SetBackgroundImage(IDB_DLGMAIN);

	RECT rt;
	GetClientRect(&rt);
	m_hWndVideoTitle = CreateWindow(L"Static", L"", WS_CHILD | WS_VISIBLE | SS_LEFT, rt.left, rt.top, 300, 22, m_hWnd, NULL, NULL, NULL);

	rt.left = rt.right - 22;
	rt.bottom = rt.top + 22;

	m_AgBtnClose.Create(L"", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, rt, this, IDC_BOX_BUTTON_CLOSE);
	m_AgBtnClose.SetBackImage(IDB_BTNCLOSE);

	rt.left = rt.left - 22;
	rt.right = rt.left + 22;

	m_AgBtnMin.Create(L"", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, rt, this, IDC_BOX_BUTTON_MIN);
	m_AgBtnMin.SetBackImage(IDB_BTNMIN);
}

inline void CAssistantBox::uninitCtrl()
{
	m_lpRtcEngine = CAgoraObject::GetEngine();
	if (m_lpRtcEngine) {

		m_AgoraCameraManager.Close();
		m_AgoraAudioInputManager.Close();
		m_AgoraPlayoutManager.Close();
	}
}

inline void CAssistantBox::initResource()
{
	m_lpHookPlayerInstance = CHookPlayerInstance::getHookInstance();
}

inline void CAssistantBox::uninitResource()
{

}

void CAssistantBox::setBoxTitle(CString strTitle)
{
	::SetWindowText(m_hWndVideoTitle, strTitle);
}

void CAssistantBox::onButtonCloseClicked()
{
	::PostMessage(theApp.GetMainWnd()->m_hWnd, WM_COMMAND,IDCANCEL, NULL);

	uninitResource();
	uninitCtrl();

	CDialogEx::OnCancel();
}

void CAssistantBox::onButtonMinClicked()
{
	ShowWindow(SW_MINIMIZE);
	m_bIsShow = FALSE;
}

void CAssistantBox::OnBnClickedButtonInvite()
{
	// TODO: Add your control notification handler code here
	m_AgoraCameraManager.TestCameraDevice(nullptr, FALSE);

	LPAGE_INVITER_JOINCHANNEL lpData = new AGE_INVITER_JOINCHANNEL;
	CString strParam;
	m_EditChannel.GetWindowTextW(strParam);
	lpData->channelName = cs2s(strParam);
	m_EditInviter.GetWindowTextW(strParam);
	lpData->uInviterId = str2long(cs2s(strParam));
	lpData->nHookType = m_ComHookPlayerList.GetCurSel();

	theApp.GetMainWnd()->PostMessageW(WM_MSGID(EID_INVITER_JOINCHANNEL),WPARAM(lpData));
}


void CAssistantBox::OnCbnSelchangeComboCamera()
{
	// TODO: Add your control notification handler code here
	if (m_lpAgoraObject) {

		int nCurSel = m_ComCameraList.GetCurSel();
		CString strDeviceName; CString strDeviceID;
		m_AgoraCameraManager.GetDevice(nCurSel, strDeviceName, strDeviceID);
		m_AgoraCameraManager.SetCurDevice(strDeviceID);
	}	
}


void CAssistantBox::OnCbnSelchangeComboMic()
{
	// TODO: Add your control notification handler code here
	if (m_lpAgoraObject) {

		int nCurSel = m_ComAudioInputList.GetCurSel();
		CString strDeviceName; CString strDeviceID;
		m_AgoraAudioInputManager.GetDevice(nCurSel, strDeviceName, strDeviceID);
		m_AgoraAudioInputManager.SetCurDevice(strDeviceID);
	}
}


void CAssistantBox::OnCbnSelchangeComboPlayout()
{
	// TODO: Add your control notification handler code here
	if (m_lpAgoraObject) {

		int nCurSel = m_ComPlayOutList.GetCurSel();
		CString strDeviceName; CString strDeviceID;
		m_AgoraPlayoutManager.GetDevice(nCurSel, strDeviceName, strDeviceID);
		m_AgoraPlayoutManager.SetCurDevice(strDeviceID);
	}
}


void CAssistantBox::OnCbnSelchangeComboPlayer()
{
	// TODO: Add your control notification handler code here
}
