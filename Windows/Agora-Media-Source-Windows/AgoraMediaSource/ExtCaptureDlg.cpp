// ExtCaptureDlg.cpp
//

#include "stdafx.h"
#include "AgoraMediaSource.h"
#include "ExtCaptureDlg.h"
#include "afxdialogex.h"


// CExtCaptureDlg

IMPLEMENT_DYNAMIC(CExtCaptureDlg, CDialogEx)

CExtCaptureDlg::CExtCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExtCaptureDlg::IDD, pParent)
{
	
}

CExtCaptureDlg::~CExtCaptureDlg()
{
	
}

void CExtCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_BTNCANCEL_EXTCAP, m_btnCancel);
    DDX_Control(pDX, IDC_BTNCONFIRM_EXTCAP, m_btnConfirm);
    DDX_Control(pDX, IDC_BTNAPPLY_EXTCAP, m_btnApply);
    DDX_Control(pDX, IDC_CKVIDEOCAP_EXTCAP, m_ckExtVideoCapture);
    DDX_Control(pDX, IDC_CKAUDIOCAP_EXTCAP, m_ckExtAudioCapture);
    DDX_Control(pDX, IDC_CKVIDEOCAP_PUSH, m_chkPushVideoFrame);
}


BEGIN_MESSAGE_MAP(CExtCaptureDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTNCANCEL_EXTCAP, &CExtCaptureDlg::OnBnClickedBtncancelExtcap)
	ON_BN_CLICKED(IDC_BTNCONFIRM_EXTCAP, &CExtCaptureDlg::OnBnClickedBtnconfirmExtcap)
	ON_BN_CLICKED(IDC_BTNAPPLY_EXTCAP, &CExtCaptureDlg::OnBnClickedBtnapplyExtcap)

	ON_CBN_SELCHANGE(IDC_CMBCAM_EXTCAP, &CExtCaptureDlg::OnCmbselCameraDevice)
	ON_CBN_SELCHANGE(IDC_CMBCAMCAP_EXTCAP, &CExtCaptureDlg::OnCmbselCameraCapability)
	
	ON_CBN_SELCHANGE(IDC_CMBMICR_EXTCAP, &CExtCaptureDlg::OnCmbselMicroDevice)
	ON_CBN_SELCHANGE(IDC_CMBMICRCAP_EXTCAP, &CExtCaptureDlg::OnCmbselMicroCapability)
	
	ON_CBN_SELCHANGE(IDC_CMBPLAYOUT_EXTCAP, &CExtCaptureDlg::OnCmbselPlayoutDevice)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CExtCaptureDlg 
void CExtCaptureDlg::OnPaint()
{
	CPaintDC dc(this);

	DrawClient(&dc);
}

BOOL CExtCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ftDes.CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	m_ftHead.CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	
	m_cmbCamera.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 40), this, IDC_CMBCAM_EXTCAP);
	m_cmbCamCap.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 40), this, IDC_CMBCAMCAP_EXTCAP);
	
	m_cmbMicrophone.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 40), this, IDC_CMBMICR_EXTCAP);
	m_cmbMicCap.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 40), this, IDC_CMBMICRCAP_EXTCAP);

	m_cmbPlayout.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 40), this, IDC_CMBPLAYOUT_EXTCAP);

	m_penFrame.CreatePen(PS_SOLID, 1, RGB(0xD8, 0xD8, 0xD8));

	
	m_agVideoCaptureDevice.Create();
	SetBackgroundColor(RGB(0xFE, 0xFE, 0xFE));

	InitCtrls();
   
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CExtCaptureDlg::InitCtrls()
{
	CRect ClientRect;

	GetClientRect(&ClientRect);

	m_cmbCamera.MoveWindow(140, 75, 180, 22, TRUE);
	m_cmbCamera.SetFont(&m_ftHead);
	m_cmbCamera.SetButtonImage(IDB_CMBBTN, 12, 12, RGB(0xFF, 0x00, 0xFF));
	m_cmbCamera.SetFaceColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF));

	m_cmbCamCap.MoveWindow(330, 75, 180, 22, TRUE);
	m_cmbCamCap.SetFont(&m_ftHead);
	m_cmbCamCap.SetButtonImage(IDB_CMBBTN, 12, 12, RGB(0xFF, 0x00, 0xFF));
	m_cmbCamCap.SetFaceColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF));

	m_ckExtVideoCapture.MoveWindow(140, 122, 200, 24);
    m_chkPushVideoFrame.MoveWindow(140, 156, 200, 24);
    m_chkPushVideoFrame.ShowWindow(SW_HIDE);
/*	m_cmbMicrophone.MoveWindow(140, 200, 180, 22, TRUE);
	m_cmbMicrophone.SetFont(&m_ftHead);
	m_cmbMicrophone.SetButtonImage(IDB_CMBBTN, 12, 12, RGB(0xFF, 0x00, 0xFF));
	m_cmbMicrophone.SetFaceColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF));

	m_cmbMicCap.MoveWindow(330, 200, 180, 22, TRUE);
	m_cmbMicCap.SetFont(&m_ftHead);
	m_cmbMicCap.SetButtonImage(IDB_CMBBTN, 12, 12, RGB(0xFF, 0x00, 0xFF));
	m_cmbMicCap.SetFaceColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF));

	m_cmbPlayout.MoveWindow(140, 270, 370, 22, TRUE);
	m_cmbPlayout.SetFont(&m_ftHead);
	m_cmbPlayout.SetButtonImage(IDB_CMBBTN, 12, 12, RGB(0xFF, 0x00, 0xFF));
	m_cmbPlayout.SetFaceColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF));

	m_ckExtAudioCapture.MoveWindow(140, 312, 200, 24);*/
    
	m_btnCancel.MoveWindow(120, 420, 120, 36, TRUE);
	m_btnConfirm.MoveWindow(270, 420, 120, 36, TRUE);
	m_btnApply.MoveWindow(420, 420, 120, 36, TRUE);

    m_cmbMicCap.ShowWindow(SW_HIDE);
    m_cmbMicrophone.ShowWindow(SW_HIDE);
    m_cmbPlayout.ShowWindow(SW_HIDE);
    m_ckExtAudioCapture.ShowWindow(SW_HIDE);
}

void CExtCaptureDlg::DrawClient(CDC *lpDC)
{
	CRect	rcText;
	CRect	rcClient;

	GetClientRect(&rcClient);

	lpDC->SelectObject(&m_penFrame);
	rcText.SetRect(rcClient.Width() / 2 - 200, 70, rcClient.Width() / 2 + 200, 102);
	lpDC->RoundRect(&rcText, CPoint(32, 32));

/*	rcText.OffsetRect(0, 125);
	lpDC->RoundRect(&rcText, CPoint(32, 32));

	rcText.OffsetRect(0, 70);
	lpDC->RoundRect(&rcText, CPoint(32, 32));*/
}

void CExtCaptureDlg::OnBnClickedBtncancelExtcap()  
{
	CDialogEx::OnCancel();
}


void CExtCaptureDlg::OnBnClickedBtnconfirmExtcap()
{
    OnBnClickedBtnapplyExtcap();
}

//videCapture
void CExtCaptureDlg::OnBnClickedBtnapplyExtcap()
{
    if (m_cmbCamera.GetCount() >0 &&  m_ckExtVideoCapture.GetCheck()) {
        m_agVideoCaptureDevice.SelectMediaCap(m_cmbCamCap.GetCurSel());
        m_agVideoCaptureDevice.CreateCaptureFilter();
        VIDEOINFOHEADER videoInfo;
        m_agVideoCaptureDevice.GetCurrentVideoCap(&videoInfo);
        VideoEncoderConfiguration config;
        config.dimensions.width = videoInfo.bmiHeader.biWidth;
        config.dimensions.height = videoInfo.bmiHeader.biHeight;
        CAgVideoBuffer::GetInstance()->resetVideoBuffer(videoInfo.bmiHeader.biWidth, videoInfo.bmiHeader.biHeight);
        CAgoraObject::GetEngine()->setVideoEncoderConfiguration(config);
    }
    else {
       
    }
    CDialogEx::OnOK();
}

void CExtCaptureDlg::OnCmbselCameraDevice()
{
	TCHAR	szDevicePath[MAX_PATH] = { 0 };
	SIZE_T	nPathLen = MAX_PATH;
	int		nSel = m_cmbCamera.GetCurSel();

	VIDEOINFOHEADER		vidInfoHeader;
	CString				strInfo;
	CString				strCompress;

	BOOL bSuccess = m_agVideoCaptureDevice.GetCurrentDevice(szDevicePath, &nPathLen);
	if (bSuccess)
		m_agVideoCaptureDevice.CloseDevice();   
	
	if (nSel != -1) {
		m_agVideoCaptureDevice.OpenDevice(nSel);
	}
    
	m_cmbCamCap.ResetContent();
    int count = m_agVideoCaptureDevice.GetMediaCapCount();
	for (int nIndex = 0; nIndex < count; nIndex++) {
		m_agVideoCaptureDevice.GetVideoCap(nIndex, &vidInfoHeader);

		switch (vidInfoHeader.bmiHeader.biCompression)
		{
		case 0x00000000:
			strInfo.Format(_T("%d*%d %dfps(RGB24)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('I', '4', '2', '0'):
			strInfo.Format(_T("%d*%d %dfps(YUV420)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('Y', 'U', 'Y', '2'):
			strInfo.Format(_T("%d*%d %dfps(YUY2)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('M', 'J', 'P', 'G'):
			strInfo.Format(_T("%d*%d %dfps(MJPEG)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		case MAKEFOURCC('U', 'Y', 'V', 'Y'):
			strInfo.Format(_T("%d*%d %dfps(UYVY)"), vidInfoHeader.bmiHeader.biWidth, vidInfoHeader.bmiHeader.biHeight, 10000000 / vidInfoHeader.AvgTimePerFrame);
			break;
		}

		m_cmbCamCap.InsertString(nIndex, strInfo);
	}
    m_cmbCamCap.SetCurSel(0);
}

void CExtCaptureDlg::OnCmbselCameraCapability()
{
	int nCurSel = m_cmbCamCap.GetCurSel();
	if (nCurSel == -1)
		return;

	m_agVideoCaptureDevice.SelectMediaCap(nCurSel);
}

void CExtCaptureDlg::OnCmbselMicroDevice()
{
	TCHAR	szDevicePath[MAX_PATH];
	SIZE_T	nPathLen = MAX_PATH;
	int		nSel = m_cmbMicrophone.GetCurSel();

	WAVEFORMATEX		wavFormatEx;
	CString				strInfo;
}

void CExtCaptureDlg::OnCmbselMicroCapability()
{
	int nCurSel = m_cmbMicCap.GetCurSel();
	if (nCurSel == -1)
		return;
}

void CExtCaptureDlg::OnCmbselPlayoutDevice()
{
	SIZE_T	nPathLen = MAX_PATH;
	int		nSel = m_cmbPlayout.GetCurSel();

	CString				strInfo;
	CString				strCompress;
}

void CExtCaptureDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);
    if (!bShow)
        return;

    TCHAR				szDevicePath[MAX_PATH] = { 0 };
    SIZE_T				nPathLen = MAX_PATH;
    CString				strInfo;
    AGORA_DEVICE_INFO	agDeviceInfo;

    nPathLen = MAX_PATH;
    m_cmbCamera.ResetContent();
    m_agVideoCaptureDevice.EnumDeviceList();
    m_agVideoCaptureDevice.GetCurrentDevice(szDevicePath, &nPathLen);

    for (int nIndex = 0; nIndex < m_agVideoCaptureDevice.GetDeviceCount(); nIndex++) {
        m_agVideoCaptureDevice.GetDeviceInfo(nIndex, &agDeviceInfo);
        m_cmbCamera.InsertString(nIndex, agDeviceInfo.szDeviceName);

        if (_tcscmp(szDevicePath, agDeviceInfo.szDevicePath) == 0)
            m_cmbCamera.SetCurSel(nIndex);
    }
}

BOOL CExtCaptureDlg::VideoCaptureControl(BOOL bStart)
{
    if (!m_ckExtVideoCapture.GetCheck())
        return TRUE;
    if (bStart) {
        CAgoraObject::GetAgoraObject()->EnableExtendVideoCapture(TRUE, &m_exCapVideoFrameObserver);
        return m_agVideoCaptureDevice.Start();
    }
    else {
        CAgoraObject::GetAgoraObject()->EnableExtendVideoCapture(FALSE, NULL);
         m_agVideoCaptureDevice.Stop();

    }
    return TRUE;
}

