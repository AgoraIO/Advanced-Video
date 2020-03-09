// ExtCaptureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraMediaSource.h"
#include "ExtCaptureDlg.h"
#include "afxdialogex.h"


// CExtCaptureDlg 对话框

IMPLEMENT_DYNAMIC(CExtCaptureDlg, CDialogEx)

CExtCaptureDlg::CExtCaptureDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExtCaptureDlg::IDD, pParent)
{
	m_hExitPlayEvent = ::CreateEvent(NULL, NULL, FALSE, NULL);
}

CExtCaptureDlg::~CExtCaptureDlg()
{
	::CloseHandle(m_hExitPlayEvent);
}

void CExtCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_BTNCANCEL_EXTCAP, m_btnCancel);
	DDX_Control(pDX, IDC_BTNCONFIRM_EXTCAP, m_btnConfirm);
	DDX_Control(pDX, IDC_BTNAPPLY_EXTCAP, m_btnApply);
	DDX_Control(pDX, IDC_CKVIDEOCAP_EXTCAP, m_ckExtVideoCapture);
	DDX_Control(pDX, IDC_CKAUDIOCAP_EXTCAP, m_ckExtAudioCapture);
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


// CExtCaptureDlg 消息处理程序
void CExtCaptureDlg::OnPaint()
{
	CPaintDC dc(this);

	DrawClient(&dc);
}

BOOL CExtCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ftDes.CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	m_ftHead.CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	
	m_cmbCamera.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 40), this, IDC_CMBCAM_EXTCAP);
	m_cmbCamCap.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 40), this, IDC_CMBCAMCAP_EXTCAP);
	
	m_cmbMicrophone.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 40), this, IDC_CMBMICR_EXTCAP);
	m_cmbMicCap.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 40), this, IDC_CMBMICRCAP_EXTCAP);

	m_cmbPlayout.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 40), this, IDC_CMBPLAYOUT_EXTCAP);

	m_penFrame.CreatePen(PS_SOLID, 1, RGB(0xD8, 0xD8, 0xD8));

	m_agAudioCaptureDevice.Create();
	m_agVideoCaptureDevice.Create();
	m_agXAudioPlayoutDevice.Create();

	SetBackgroundColor(RGB(0xFE, 0xFE, 0xFE));

	InitCtrls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
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

	m_cmbMicrophone.MoveWindow(140, 200, 180, 22, TRUE);
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

	m_ckExtAudioCapture.MoveWindow(140, 312, 200, 24);

	m_btnCancel.MoveWindow(120, 420, 120, 36, TRUE);
	m_btnConfirm.MoveWindow(270, 420, 120, 36, TRUE);
	m_btnApply.MoveWindow(420, 420, 120, 36, TRUE);
}

void CExtCaptureDlg::DrawClient(CDC *lpDC)
{
	CRect	rcText;
	CRect	rcClient;

	GetClientRect(&rcClient);

	lpDC->SelectObject(&m_penFrame);
	rcText.SetRect(rcClient.Width() / 2 - 200, 70, rcClient.Width() / 2 + 200, 102);
	lpDC->RoundRect(&rcText, CPoint(32, 32));

	rcText.OffsetRect(0, 125);
	lpDC->RoundRect(&rcText, CPoint(32, 32));

	rcText.OffsetRect(0, 70);
	lpDC->RoundRect(&rcText, CPoint(32, 32));
}

void CExtCaptureDlg::OnBnClickedBtncancelExtcap()  
{
	// TODO:  在此添加控件通知处理程序代码

	CDialogEx::OnCancel();
}


void CExtCaptureDlg::OnBnClickedBtnconfirmExtcap()
{
	// TODO:  在此添加控件通知处理程序代码

	CDialogEx::OnOK();
}


void CExtCaptureDlg::OnBnClickedBtnapplyExtcap()
{
	// TODO:  在此添加控件通知处理程序代码
	VIDEOINFOHEADER videoInfo;
	WAVEFORMATEX	waveFormat;
	SIZE_T			nBufferSize = 0;

	CAgoraObject	*lpAgoraObject = CAgoraObject::GetAgoraObject();

	int nCamCapSel = m_cmbCamCap.GetCurSel();
	if (nCamCapSel == -1) {
		m_cmbCamCap.SetCurSel(0);
		nCamCapSel = 0;
	}

	int nMicCapSel = m_cmbMicCap.GetCurSel();
	if (nMicCapSel == -1) {
		m_cmbMicCap.SetCurSel(0);
		nMicCapSel = 0;
	}
		
	if (m_ckExtVideoCapture.GetCheck()) {
		m_agVideoCaptureDevice.SelectMediaCap(nCamCapSel);
		m_agVideoCaptureDevice.GetCurrentVideoCap(&videoInfo);
//		nBufferSize = 3 * videoInfo.bmiHeader.biHeight*videoInfo.bmiHeader.biWidth;
		m_agVideoCaptureDevice.SetCaptureBuffer(videoInfo.bmiHeader.biSizeImage, 30, 4);
		m_agVideoCaptureDevice.SetGrabberCallback(&m_agVideoCapture, 1);
		CVideoPackageQueue::GetInstance()->SetVideoFormat(&videoInfo.bmiHeader);

		lpAgoraObject->SetVideoProfileEx(videoInfo.bmiHeader.biWidth, videoInfo.bmiHeader.biHeight, static_cast<int>(10000000 / videoInfo.AvgTimePerFrame), 1000);
		
	}

	if (m_ckExtAudioCapture.GetCheck()) {
		m_agAudioCaptureDevice.SelectMediaCap(nMicCapSel);
		m_agAudioCaptureDevice.GetCurrentAudioCap(&waveFormat);
		nBufferSize = waveFormat.nAvgBytesPerSec / 50;
		m_agAudioCaptureDevice.SetCaptureBuffer(nBufferSize, 16, waveFormat.nBlockAlign);
		m_agAudioCaptureDevice.SetGrabberCallback(&m_agAudioCapture, 1);
		CAudioCapturePackageQueue::GetInstance()->SetAudioFormat(&waveFormat);
		CAudioCapturePackageQueue::GetInstance()->SetAudioPackageSize(nBufferSize);

		lpAgoraObject->SetAudioProfileEx(waveFormat.nSamplesPerSec, waveFormat.nChannels, waveFormat.nSamplesPerSec / 20);

//		m_agXAudioPlayoutDevice.SetAudioSampleRate(waveFormat.nSamplesPerSec);
		m_agXAudioPlayoutDevice.SetAudioFormat(&waveFormat, &m_exCapVoiceContext);

	}

}

void CExtCaptureDlg::OnCmbselCameraDevice()
{
	TCHAR	szDevicePath[MAX_PATH];
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
		m_agVideoCaptureDevice.SetCaptureBuffer(0x1950000, 30, 4);
	}

	m_cmbCamCap.ResetContent();
	for (int nIndex = 0; nIndex < m_agVideoCaptureDevice.GetMediaCapCount(); nIndex++) {
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

	BOOL bSuccess = m_agAudioCaptureDevice.GetCurrentDevice(szDevicePath, &nPathLen);
	if (bSuccess)
		m_agAudioCaptureDevice.CloseDevice();

	if (nSel != -1)
		m_agAudioCaptureDevice.OpenDevice(nSel);

	m_cmbMicCap.ResetContent();
	for (int nIndex = 0; nIndex < m_agAudioCaptureDevice.GetMediaCapCount(); nIndex++) {
		m_agAudioCaptureDevice.GetAudioCap(nIndex, &wavFormatEx);
		strInfo.Format(_T("%.1fkHz %dbits %dCh"), wavFormatEx.nSamplesPerSec / 1000.0, wavFormatEx.wBitsPerSample, wavFormatEx.nChannels);
		m_cmbMicCap.InsertString(nIndex, strInfo);
	}
}

void CExtCaptureDlg::OnCmbselMicroCapability()
{
	int nCurSel = m_cmbMicCap.GetCurSel();
	if (nCurSel == -1)
		return;

	m_agAudioCaptureDevice.SelectMediaCap(nCurSel);
}

void CExtCaptureDlg::OnCmbselPlayoutDevice()
{
	SIZE_T	nPathLen = MAX_PATH;
	int		nSel = m_cmbPlayout.GetCurSel();

	CString				strInfo;
	CString				strCompress;

	m_agXAudioPlayoutDevice.CloseDevice();

	if (nSel != -1)
		m_agXAudioPlayoutDevice.OpenDevice(nSel);

}

void CExtCaptureDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO:  在此处添加消息处理程序代码
	if (!bShow)
		return;

	TCHAR				szDevicePath[MAX_PATH];
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


	nPathLen = MAX_PATH;
	m_cmbMicrophone.ResetContent();
	m_agAudioCaptureDevice.EnumDeviceList();
	m_agAudioCaptureDevice.GetCurrentDevice(szDevicePath, &nPathLen);
	for (int nIndex = 0; nIndex < m_agAudioCaptureDevice.GetDeviceCount(); nIndex++) {
		m_agAudioCaptureDevice.GetDeviceInfo(nIndex, &agDeviceInfo);
		m_cmbMicrophone.InsertString(nIndex, agDeviceInfo.szDeviceName);

		if (_tcscmp(szDevicePath, agDeviceInfo.szDevicePath) == 0)
			m_cmbMicrophone.SetCurSel(nIndex);
	}

	nPathLen = MAX_PATH;
	m_cmbPlayout.ResetContent();
	m_agXAudioPlayoutDevice.EnumDeviceList();
	m_agXAudioPlayoutDevice.GetCurrentDevice(szDevicePath, &nPathLen);
	for (int nIndex = 0; nIndex < m_agXAudioPlayoutDevice.GetDeviceCount(); nIndex++) {
		m_agXAudioPlayoutDevice.GetDeviceInfo(nIndex, &agDeviceInfo);
		m_cmbPlayout.InsertString(nIndex, agDeviceInfo.szDeviceName);

		if (_tcscmp(szDevicePath, agDeviceInfo.szDevicePath) == 0)
			m_cmbPlayout.SetCurSel(nIndex);
	}
}

BOOL CExtCaptureDlg::VideoCaptureControl(BOOL bStart)
{
	if (!m_ckExtVideoCapture.GetCheck())
		return TRUE;

	if (bStart) {
		CAgoraObject::GetAgoraObject()->EnableExtendVideoCapture(TRUE, &m_exCapVideoFrameObserver);
		return m_agVideoCaptureDevice.CaptureControl(DEVICE_START);
	}
	else {
		CAgoraObject::GetAgoraObject()->EnableExtendVideoCapture(FALSE, NULL);
		return m_agVideoCaptureDevice.CaptureControl(DEVICE_STOP);
	}
}

BOOL CExtCaptureDlg::AudioCaptureControl(BOOL bStart)
{
	if (!m_ckExtAudioCapture.GetCheck())
		return TRUE;

	if (bStart) {
		CAgoraObject::GetAgoraObject()->EnableExtendAudioCapture(TRUE, &m_exCapAudioFrameObserver);
		
		m_playThreadParam.lpXAudioSourceVoice = m_agXAudioPlayoutDevice.GetSourceVoicePtr();
		m_playThreadParam.lpXAudioVoiceContext = &m_exCapVoiceContext;

		AfxBeginThread(&CExtCaptureDlg::PlayoutThread, &m_playThreadParam);
		
		return m_agAudioCaptureDevice.CaptureControl(DEVICE_START);
	}
	else {
		CAgoraObject::GetAgoraObject()->EnableExtendAudioCapture(FALSE, NULL);
		::SetEvent(m_hExitPlayEvent);

		return m_agAudioCaptureDevice.CaptureControl(DEVICE_STOP);
	}
}

UINT CExtCaptureDlg::PlayoutThread(LPVOID lParam)
{
	LPPLAYOUT_THREAD_PARAM lpParam = reinterpret_cast<LPPLAYOUT_THREAD_PARAM>(lParam);

	StreamingVoiceContext	*lpXAudioVoiceContext = reinterpret_cast<StreamingVoiceContext *>(lpParam->lpXAudioVoiceContext);
	IXAudio2SourceVoice		*lpXAudioSoruceVoice = reinterpret_cast<IXAudio2SourceVoice *>(lpParam->lpXAudioSourceVoice);

	CAudioPlayPackageQueue	*lpBufferQueue = CAudioPlayPackageQueue::GetInstance();

	XAUDIO2_BUFFER	xAudioBuffer;
	LPBYTE			lpAudioData = new BYTE[8192];
	SIZE_T			nAudioBufferSize = 8192;

	do {
		if (::WaitForSingleObject(lpParam->hExitEvent, 0) == WAIT_OBJECT_0)
			break;

		nAudioBufferSize = 8192;

		if (!lpBufferQueue->PopAudioPackage(lpAudioData, &nAudioBufferSize))
			continue;

		memset(&xAudioBuffer, 0, sizeof(XAUDIO2_BUFFER));
		xAudioBuffer.AudioBytes = nAudioBufferSize;
		xAudioBuffer.pAudioData = lpAudioData;
		lpXAudioSoruceVoice->SubmitSourceBuffer(&xAudioBuffer);
		::WaitForSingleObject(lpXAudioVoiceContext->hBufferEndEvent, INFINITE);

	} while (TRUE);

	delete[] lpAudioData;

	return 0;
}