#pragma once
#include "AGButton.h"
#include "AGEdit.h"
#include "AGComboBox.h"
#include "DeviceDlg.h"
#include "ExtCaptureDlg.h"
#include "afxwin.h"

// CEnterChannelDlg

class CEnterChannelDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEnterChannelDlg)

public:
	CEnterChannelDlg(CWnd* pParent = NULL);
	virtual ~CEnterChannelDlg();

	CString GetChannelName();

	void SetVideoString(LPCTSTR lpVideoString);
    void CleanEncryptionSecret();

	void ExtCaptureControl(BOOL bEnable);

	enum { IDD = IDD_ENTERCHANNEL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);  
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnPaint();
	afx_msg void OnBnClickedBtntestChannel();
	afx_msg void OnBnClickedBtnjoinChannel();
	afx_msg void OnBnClickedBtnsetChannel();
	afx_msg void OnBnClickedBtnextcapChannel();

	DECLARE_MESSAGE_MAP()

protected:
	void InitCtrls();
	void DrawClient(CDC *lpDC);

private:
	CAGEdit			m_ctrChannel;
	CAGEdit			m_ctrEncKey;
    CAGComboBox     m_cmbEncType;
    CAGButton		m_btnTest;
	CAGButton		m_btnJoin;
	CAGButton		m_btnSetup;
	CAGButton		m_btnExtCapture;

	CFont			m_ftEncy;
	CFont			m_ftHead;
	CFont			m_ftDesc;
	CFont			m_ftBtn;

    CPen            m_penFrame;

	CDeviceDlg		m_dlgDevice;
	CExtCaptureDlg	m_dlgExtCapture;
	
};
