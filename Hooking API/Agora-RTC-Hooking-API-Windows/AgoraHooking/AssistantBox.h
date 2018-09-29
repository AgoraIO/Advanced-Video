#pragma once


// CAssistantBox dialog

#include "AGButton.h"
#include "AgoraCameraManager.h"
#include "AgoraAudInputManager.h"
#include "AgoraPlayoutManager.h"
#include "CHookPlayerInstance.h"

class CAssistantBox : public CDialogEx
{
	DECLARE_DYNAMIC(CAssistantBox)

public:
	CAssistantBox(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAssistantBox();
	BOOL isShow(){ return m_bIsShow; }
	void setWnd(HWND wnd){ m_hWndVideoTest = wnd; }

	virtual void OnFinalRelease();

// Dialog Data
	enum { IDD = IDD_DIALOG_AssistantBox };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNcPaint();

	void onButtonCloseClicked();
	void onButtonMinClicked();

public:
	afx_msg void OnBnClickedButtonInvite();
	afx_msg void OnCbnSelchangeComboCamera();
	afx_msg void OnCbnSelchangeComboMic();
	afx_msg void OnCbnSelchangeComboPlayout();
	afx_msg void OnCbnSelchangeComboPlayer();

protected:
	inline void initCtrl();
	inline void uninitCtrl();

	inline void initResource();
	inline void uninitResource();

private:

	CAGButton m_AgBtnMin;
	CAGButton m_AgBtnClose;
	BOOL m_bIsShow;

	CAgoraObject* m_lpAgoraObject;
	IRtcEngine* m_lpRtcEngine;
	CAgoraCameraManager m_AgoraCameraManager;
	CAgoraPlayoutManager m_AgoraPlayoutManager;
	CAgoraAudInputManager m_AgoraAudioInputManager;
	HWND m_hWndVideoTest;

	CComboBox m_ComCameraList;
	CComboBox m_ComAudioInputList;
	CComboBox m_ComPlayOutList;
	CEdit m_EditChannel;
	CEdit m_EditLoginUid;
	CEdit m_EditInviter;
	CButton m_CheckBoxHook;
	CComboBox m_ComHookPlayerList;

	CHookPlayerInstance* m_lpHookPlayerInstance;
};
