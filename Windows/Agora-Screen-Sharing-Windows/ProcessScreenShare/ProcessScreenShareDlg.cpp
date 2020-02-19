
// ProcessScreenShareDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessScreenShare.h"
#include "ProcessScreenShareDlg.h"
#include "afxdialogex.h"

#include <iostream>
#include "commonFun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CProcessScreenShareDlg dialog



CProcessScreenShareDlg::CProcessScreenShareDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProcessScreenShareDlg::IDD, pParent)
	, m_lpAgoraObject(nullptr)
	, m_lpRtcEngine(nullptr)
	, m_hScreenShareWnd(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProcessScreenShareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProcessScreenShareDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_WM_COPYDATA()
	ON_MESSAGE(WM_MSGID(EID_SCREENSHARE_BASEINFO), OnScreenShareBaseInfo)
	ON_MESSAGE(WM_MSGID(EID_SCREENSHARE_START), OnScreenShareStart)
	ON_MESSAGE(WM_MSGID(EID_SCREENSHARE_STOP), OnScreenShareStop)
	ON_MESSAGE(WM_MSGID(EID_SCREENSHARE_CLOSE),OnScreenShareClose)
END_MESSAGE_MAP()


// CProcessScreenShareDlg message handlers

BOOL CProcessScreenShareDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CProcessScreenShareDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProcessScreenShareDlg::OnPaint()
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
HCURSOR CProcessScreenShareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CProcessScreenShareDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
// 	ShowWindow(SW_MINIMIZE);
// 	ShowWindow(SW_HIDE);
}

void CProcessScreenShareDlg::OnClose()
{
	if (m_lpAgoraObject) {

		m_lpAgoraObject->EnableScreenCapture(NULL, 0, NULL, FALSE);
		m_lpAgoraObject->LeaveCahnnel();

		uninitAgoraMedia();
	}

	CDialogEx::OnCancel();
}

LRESULT CProcessScreenShareDlg::OnScreenShareBaseInfo(WPARAM wParam, LPARAM lParam)
{
	//InitRtcEngine
	LPAGE_SCREENSHARE_BASEINFO lpData = (LPAGE_SCREENSHARE_BASEINFO)wParam;
	if (lpData) {

		m_strAppID = APP_ID;
		m_strChannelName = s2cs(lpData->channelname);
		m_uId = lpData->uSubuID;

		initAgoraMedia();
	}

	return TRUE;
}

LRESULT CProcessScreenShareDlg::OnScreenShareStart(WPARAM wParam, LPARAM lParam)
{
	//joinChannel  startScreenShare
	LPAGE_SCREENSHARE_START lpData = (LPAGE_SCREENSHARE_START)wParam;
	if (lpData) {

		m_hScreenShareWnd = lpData->hWnd;
		m_lpAgoraObject->JoinChannel(m_strChannelName, m_uId);
		m_lpAgoraObject->EnableScreenCapture(m_hScreenShareWnd);
	}

	return TRUE;
}

LRESULT CProcessScreenShareDlg::OnScreenShareStop(WPARAM wParam, LPARAM lParam)
{
	//stopScreenShare
	m_hScreenShareWnd = nullptr;
	m_lpAgoraObject->EnableScreenCapture(NULL, 0, NULL, FALSE);
	m_lpAgoraObject->LeaveCahnnel();

	return TRUE;
}

LRESULT CProcessScreenShareDlg::OnScreenShareClose(WPARAM wParam, LPARAM lParam)
{
	PostMessage(WM_COMMAND, IDCANCEL);

	return TRUE;
}

BOOL CProcessScreenShareDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (pCopyDataStruct && pCopyDataStruct->lpData){
		SHARETYPE type = (SHARETYPE)pCopyDataStruct->dwData;
		switch (type){
		case SHARETYPE::ShareType_BaseInfo:
			SendMessage(WM_MSGID(EID_SCREENSHARE_BASEINFO),(WPARAM)(pCopyDataStruct->lpData));
			break;
		case SHARETYPE::ShareType_Start:
			SendMessage(WM_MSGID(EID_SCREENSHARE_START), (WPARAM)(pCopyDataStruct->lpData));
			break;
		case SHARETYPE::ShareType_Stop:
			SendMessage(WM_MSGID(EID_SCREENSHARE_STOP), (WPARAM)(pCopyDataStruct->lpData));
			break;
		case SHARETYPE::ShareType_Close:
			PostMessage(WM_MSGID(EID_SCREENSHARE_CLOSE));
			break;
		default: break;
		}
	}

	return TRUE;
}

inline void CProcessScreenShareDlg::initAgoraMedia()
{
	m_lpAgoraObject = CAgoraObject::GetAgoraObject(m_strAppID);
	ASSERT(m_lpAgoraObject);
	m_lpAgoraObject->SetMsgHandlerWnd(m_hWnd);
	
	m_lpRtcEngine = CAgoraObject::GetEngine();
	ASSERT(m_lpRtcEngine);

	m_lpAgoraObject->SetLogFilePath();
	m_lpAgoraObject->EnableWebSdkInteroperability(TRUE);

	m_lpAgoraObject->EnableVideo(TRUE);
	m_lpAgoraObject->EnableAudio(FALSE);

	m_lpAgoraObject->EnableLastmileTest(FALSE);
	m_lpAgoraObject->SetClientRole(CLIENT_ROLE_TYPE::CLIENT_ROLE_BROADCASTER);
	m_lpAgoraObject->SetChannelProfile(TRUE);

	m_lpAgoraObject->MuteAllRemoteAudio(TRUE);
	m_lpAgoraObject->MuteAllRemoteVideo(TRUE);
}

inline void CProcessScreenShareDlg::uninitAgoraMedia()
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