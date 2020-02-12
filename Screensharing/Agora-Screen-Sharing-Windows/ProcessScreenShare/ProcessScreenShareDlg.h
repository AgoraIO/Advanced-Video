
// ProcessScreenShareDlg.h : header file
//

#pragma once
#include <iostream>

// CProcessScreenShareDlg dialog
class CProcessScreenShareDlg : public CDialogEx
{
// Construction
public:
	CProcessScreenShareDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROCESSSCREENSHARE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnScreenShareBaseInfo(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnScreenShareStart(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnScreenShareStop(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnScreenShareClose(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

private:
	
	inline void initAgoraMedia();
	inline void uninitAgoraMedia();

private:

	CString  m_strAppID;
	CString m_strChannelName;
	UINT m_uId;
	HWND m_hScreenShareWnd;

	CAgoraObject* m_lpAgoraObject;
	IRtcEngine* m_lpRtcEngine;
};
