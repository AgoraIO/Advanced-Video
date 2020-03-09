#pragma once
#include "AGButton.h"
#include "AGVideoWnd.h"
#include "DeviceDlg.h"
#include "ChatDlg.h"
#include "SEIDlg.h"
#include "AGScreenCaptureDlg.h"

// CVideoDlg 对话框

class CVideoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVideoDlg)

public:
	CVideoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVideoDlg();

// 对话框数据
	enum { IDD = IDD_VIDEO_DIALOG };

	enum { 
		SCREEN_VIDEO1 = 0,	// 单屏
		SCREEN_VIDEO4,		// 4分屏
		SCREEN_VIDEOMULTI,	// 1大N小(16人)
	};

	HWND GetRemoteVideoWnd(int nIndex);
	HWND GetLocalVideoWnd() { return m_wndLocal.GetSafeHwnd(); };

	void RebindVideoWnd();

	void ShowControlButton(BOOL bShow = TRUE);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnMove(int x, int y);

    afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg LRESULT OnShowModeChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowBig(WPARAM wParam, LPARAM lParam);
	
	afx_msg void OnBnClickedBtnmin();
	afx_msg void OnBnClickedBtnclose();
	afx_msg void OnBnClickedBtnrest();

    afx_msg void OnBnClickedBtnmessage();
    afx_msg void OnBnClickedBtnmode();
	afx_msg void OnBnClickedBtnaudio();
	afx_msg void OnBnClickedBtnScreenCapture();
	afx_msg void OnBnClickedBtnMore();

	afx_msg void OnBnClickedBtnMuteLocalVideo();
	afx_msg void OnBnClickedBtnMuteAllRemoteVideo();
	afx_msg void OnBnClickedBtnDisableVideo();

    afx_msg void OnBnClickedBtntip();
    afx_msg void OnBnClickedBtnsetup();
    afx_msg void OnBnClickedBtnfilter();
	afx_msg void OnBnClickedBtnmix();
	afx_msg void OnBnClickedBtSEIPush();

	afx_msg void OnBnClickedBtnfullscr();

	afx_msg void OnBnClickedScreenshare();
	afx_msg void OnBnClickedWindowshare();
	afx_msg void OnBnClickedEndshare();

	// 用于处理引擎的回调消息
	afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDReJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDAudioVolumeIndication(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDActiveSpeaker(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnEIDError(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnEIDFirstLocalFrame(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDFirstRemoteFrameDecoded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDConnectionLost(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDVideoDeviceChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDRemoteVideoStat(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnEIDStartRecordingService(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDStopRecordingService(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDApiCallExecuted(WPARAM wParam, LPARAM lParam);
	
	afx_msg LRESULT OnEIDStreamMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDRequestChannelKey(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDClientRoleChanged(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnWindowShareStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()

protected:
	BOOL NeedShowSizeBox(CPoint point);
	void EnableSize(BOOL bEnable);
	void CreateMuteAudioMenu();
	void CreateMuteVideoMenu();
	void CreateScreenShareMenu();
    void CreateMoreMenu();

	void DrawHead(CDC *pDC);

	void InitCtrls();

	void ShowVideo1();
	void ShowVideo4();
	void ShowMulti();
	void ShowMultiNew();

	void ShowButtonsNormal();

	void AdjustButtonsNormal(int cx, int cy);

	void AdjustSizeVideo1(int cx, int cy);
	void AdjustSizeVideo4(int cx, int cy);
	void AdjustSizeVideoMulti(int cx, int cy);
	void AdjustSizeVideoMultiNew(int cx, int cy);

private:
	CBrush			m_brHead;

	CAGButton		m_btnMin;
	CAGButton		m_btnRst;
	CAGButton		m_btnClose;

    //CAGButton       m_btnMessage;
	CAGButton		m_btnMode;
	CAGButton		m_btnAudio;
	CAGButton		m_btnEndCall;
	CAGButton		m_btnScrCap;
    CAGButton       m_btnMore;



	CAGButton		m_btnShow;

	BOOL			m_bLastStat;
	UINT			m_nScreenMode;
	UINT			m_nBigShowedUID;
	
	CAGVideoWnd		m_wndLocal;
	CAGVideoWnd		m_wndVideo[16];
	CAGVideoWnd		*m_lpBigShowed;

	CDeviceDlg		m_dlgDevice;
    CChatDlg        m_dlgChat;
	CSEIDlg			m_dlgSEI;
	
	
	

	CRect			m_rcVideoArea;
	CRect			m_rcChildVideoArea;

    CBitmap         m_bitMenuInfo;
    CBitmap         m_bitMenuDevice;
    CBitmap         m_bitMenuFilter;
	CBitmap			m_bitMenuMix;
	CBitmap			m_bitMenuSEI;
	CBitmap			m_bitMenuBCaster;
	CBitmap			m_bitMenuAudRec;
	CBitmap			m_bitMenuVidRec;
	CBitmap			m_bitMenuVExtSet;
	CAGScreenCaptureDlg		m_dlgScreenCapture;
	INT_PTR			m_nVidRcdTimer;

private:	// data	

	typedef struct _AGVIDEO_WNDINFO
	{
		UINT	nUID;
		int		nIndex;

		UINT	nWidth;
		UINT	nHeight;
		int		nBitrate;
		int		nFramerate;
		int		nCodec;

	} AGVIDEO_WNDINFO, *PAGVIDEO_WNDINFO, *LPAGVIDEO_WNDINFO;

	CList<AGVIDEO_WNDINFO>	m_listWndInfo;

	BOOL			m_bRecording;
	BOOL			m_bFullScreen;
    BOOL            m_bFilter;
    BOOL            m_bShowInfo;


};
