#pragma once
#include "AGButton.h"

// CLogoDlg

class CLogoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLogoDlg)

public:
	CLogoDlg(CWnd* pParent = NULL);
	virtual ~CLogoDlg();

	enum { IDD = IDD_LOGO_DIALOG };

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnNcHitTest(CPoint point);

	afx_msg void OnBnClickedBtnmin();
	afx_msg void OnBnClickedBtnclose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

protected:
	void InitCtrls();
	void DrawClient(CDC *lpDC);

private:
	CAGButton	m_btnMin;
	CAGButton	m_btnClose;

	CFont		m_ftTitle;
	CFont		m_ftDescp;

	UINT_PTR	m_nTimerID;
	
private:
    CBitmap     m_bmpLogo;
};
