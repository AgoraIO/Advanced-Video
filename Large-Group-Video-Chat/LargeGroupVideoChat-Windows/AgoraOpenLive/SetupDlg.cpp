// SetupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraOpenLive.h"
#include "SetupDlg.h"
#include "afxdialogex.h"


// CSetupDlg 对话框

IMPLEMENT_DYNAMIC(CSetupDlg, CDialogEx)

CSetupDlg::CSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetupDlg::IDD, pParent)
{

}

CSetupDlg::~CSetupDlg()
{
}

void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTNCANCEL_SETUP, m_btnCancel);
	DDX_Control(pDX, IDC_BTNCONFIRM_SETUP, m_btnConfirm);
	DDX_Control(pDX, IDC_CKSAVESETTING_SETUP, m_ckSaveSettings);
	DDX_Control(pDX, IDC_CKSWPWH_SETUP, m_ckSwapWH);
	DDX_Control(pDX, IDC_CKFULLBAND_SETUP, m_ckFullBand);
	DDX_Control(pDX, IDC_CKSTEREO_SETUP, m_ckStereo);
	DDX_Control(pDX, IDC_CKFULLBITRATE_SETUP, m_ckFullBitrate);
}


BEGIN_MESSAGE_MAP(CSetupDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTNCONFIRM_SETUP, &CSetupDlg::OnBnClickedBtnconfirmSetup)
//	ON_CBN_SELCHANGE(IDC_CMBRES_SETUP, &CSetupDlg::OnCbnSelchangeCmbresSetup)
//	ON_CBN_SELCHANGE(IDC_CMBCODEC_SETUP, &CSetupDlg::OnCbnSelchangeCmbresSetup)

	ON_WM_HSCROLL()
	ON_WM_LBUTTONDBLCLK()

	ON_BN_CLICKED(IDC_BUTTON1, &CSetupDlg::OnBnClickedBtncancelSetup)
END_MESSAGE_MAP()


// CSetupDlg 消息处理程序


BOOL CSetupDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ftHead.CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	m_ftDes.CreateFont(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
	m_ftBtn.CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

	m_penFrame.CreatePen(PS_SOLID, 1, RGB(0xD8, 0xD8, 0xD8));
	m_cbxVideoProfile.Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, CRect(0, 0, 300, 40), this, IDC_CMBVDOPRF_SETUP);
	SetBackgroundColor(RGB(0xFF, 0xFF, 0xFF), TRUE);
    InitData();
    InitCtrls();

	if (m_agConfig.IsAutoSaveEnabled())
		m_ckSaveSettings.SetCheck(TRUE);
	else
		m_ckSaveSettings.SetCheck(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CSetupDlg::InitCtrls()
{
	CRect ClientRect;

	MoveWindow(0, 0, 320, 450, 1);
	GetClientRect(&ClientRect);

	int nResolutionIndex = 0;
	CString str;
	
	m_cbxVideoProfile.MoveWindow(210, 80, 200, 22, TRUE);
	m_cbxVideoProfile.SetFont(&m_ftDes);
	m_cbxVideoProfile.SetButtonImage(IDB_CMBBTN, 12, 12, RGB(0xFF, 0x00, 0xFF));
	m_cbxVideoProfile.SetFaceColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF));
	m_cbxVideoProfile.SetListMaxHeight(600);

	m_ckSwapWH.MoveWindow(80, 120, 20, 20, TRUE);
	m_ckSaveSettings.MoveWindow(80, ClientRect.Height() - 95, 20, 20, TRUE);

	m_ckFullBand.MoveWindow(80, 220, 20, 20, TRUE);
	m_ckStereo.MoveWindow(80, 240, 20, 20, TRUE);
	m_ckFullBitrate.MoveWindow(80, 260, 20, 20, TRUE);

	m_btnCancel.MoveWindow(ClientRect.Width() / 2 - 93, ClientRect.Height() - 58, 174, 36, TRUE);

	m_btnConfirm.MoveWindow(ClientRect.Width() / 2 + 93, ClientRect.Height() - 58, 174, 36, TRUE);

	for (int nIndex = 0; nIndex < 3; nIndex++) {
		m_cbxVideoProfile.InsertString(nIndex, m_szProfileDes[nIndex]);
		m_cbxVideoProfile.SetItemData(nIndex, (DWORD_PTR)m_nProfileValue[nIndex]);
	}

	m_btnCancel.SetBorderColor(RGB(0xD8, 0xD8, 0xD8), RGB(0x00, 0xA0, 0xE9), RGB(0x00, 0xA0, 0xE9), RGB(0xCC, 0xCC, 0xCC));
	m_btnCancel.SetBackColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xFF, 0xFF));
	m_btnCancel.SetTextColor(RGB(0x55, 0x58, 0x5A), RGB(0x00, 0xA0, 0xE9), RGB(0x00, 0xA0, 0xE9), RGB(0xCC, 0xCC, 0xCC));

	m_btnConfirm.SetBackColor(RGB(0, 160, 239), RGB(0, 160, 239), RGB(0, 160, 239), RGB(192, 192, 192));
	m_btnConfirm.SetFont(&m_ftBtn);
	m_btnConfirm.SetTextColor(RGB(0xFF, 0xFF, 0xFF), RGB(0xFF, 0xC8, 0x64), RGB(0xFF, 0xC8, 0x64), RGB(0xCC, 0xCC, 0xCC));
	m_btnConfirm.SetWindowText(LANG_STR("IDS_SET_BTCONFIRM"));

    if (m_agConfig.IsAutoSaveEnabled())
        nResolutionIndex = m_agConfig.GetSolution();
    else
        nResolutionIndex = 0;
    
    m_cbxVideoProfile.SetCurSel(nResolutionIndex);
}

void CSetupDlg::InitData()
{
	m_szProfileDes[0] = _T("640x480 15fps 500kbps");
	m_nProfileValue[0] = 0;
	m_szProfileDes[1] = _T("640x360 15fps 400kbps");
	m_nProfileValue[1] = 1;

	m_szProfileDes[2] = _T("640x360 24fps 800kbps");
	m_nProfileValue[2] = 2;

}

void CSetupDlg::DrawClient(CDC *lpDC)
{
	CRect	rcText;
	CRect	rcClient;
	LPCTSTR lpString = NULL;

	GetClientRect(&rcClient);

	CFont *lpDefFont = lpDC->SelectObject(&m_ftHead);
	CPen  *lpDefPen = lpDC->SelectObject(&m_penFrame);

	rcText.SetRect(rcClient.Width() / 2 - 188, 75, rcClient.Width() / 2 + 172, 107);
	lpDC->RoundRect(&rcText, CPoint(32, 32));

	lpDC->SetBkColor(RGB(0xFF, 0xFF, 0xFF));
	lpDC->SetTextColor(RGB(0xD8, 0xD8, 0xD8));

	lpDC->SetTextColor(RGB(0x44, 0x45, 0x46));
	lpString = LANG_STR("IDS_SET_RESOLUTION");
	lpDC->TextOut(80, 83, lpString, _tcslen(lpString));
	
	lpString = LANG_STR("IDS_SET_SWAPWH");
	lpDC->TextOut(100, 120, lpString, _tcslen(lpString));

	lpString = LANG_STR("IDS_SET_FULLBAND");
	lpDC->TextOut(100, 220, lpString, _tcslen(lpString));
	lpString = LANG_STR("IDS_SET_STEREO");
	lpDC->TextOut(100, 240, lpString, _tcslen(lpString));
	lpString = LANG_STR("IDS_SET_FULLBITRATE");
	lpDC->TextOut(100, 260, lpString, _tcslen(lpString));


	lpString = LANG_STR("IDS_SET_SAVESETTING");
	lpDC->TextOut(100, rcClient.Height() - 95, lpString, _tcslen(lpString));

	lpDC->SelectObject(lpDefPen);
	lpDC->SelectObject(lpDefFont);
}

void CSetupDlg::OnBnClickedBtnconfirmSetup()
{
	// TODO:  在此添加控件通知处理程序代码
	GetParent()->SendMessage(WM_GOBACK, 0, 0);

	if (m_ckSaveSettings.GetCheck() == TRUE) {
		m_agConfig.EnableAutoSave(TRUE);
        m_agConfig.SetSolution(m_cbxVideoProfile.GetCurSel());
	}
	else
		m_agConfig.EnableAutoSave(FALSE);

	BOOL bFullBand = m_ckFullBand.GetCheck();
	BOOL bStereo = m_ckStereo.GetCheck();
	BOOL bFullBitrate = m_ckFullBitrate.GetCheck();

//	CAgoraObject::GetAgoraObject()->SetHighQualityAudio(bFullBand, bStereo, bFullBitrate);
}

void CSetupDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	DrawClient(&dc);
}

int CSetupDlg::GetVideoSolution()
{
    int nIndex = m_cbxVideoProfile.GetCurSel();
    return (int)m_cbxVideoProfile.GetItemData(nIndex);
}

void CSetupDlg::SetVideoSolution(int nIndex)
{
	m_cbxVideoProfile.SetCurSel(nIndex);
}

CString CSetupDlg::GetVideoSolutionDes()
{
    int nIndex = m_cbxVideoProfile.GetCurSel();
    if (nIndex == -1)
        nIndex = 0;

    return (CString)m_szProfileDes[nIndex];
}

void CSetupDlg::SetWHSwap(BOOL bSwap)
{
	m_ckSwapWH.SetCheck(bSwap);
}

BOOL CSetupDlg::IsWHSwap()
{
	return (BOOL)m_ckSwapWH.GetCheck();
}

void CSetupDlg::OnBnClickedBtncancelSetup()
{
	// TODO:  在此添加控件通知处理程序代码
	GetParent()->SendMessage(WM_GOBACK, 0, 0);
}
