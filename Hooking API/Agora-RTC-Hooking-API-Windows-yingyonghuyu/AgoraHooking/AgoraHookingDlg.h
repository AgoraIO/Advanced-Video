
// AgoraHookingDlg.h : header file
//

#include "AGButton.h"
#pragma once

class CAssistantBox;
class CExtendAudioFrameObserver;

#include "CHookPlayerInstance.h"

// CAgoraHookingDlg dialog
class CAgoraHookingDlg : public CDialogEx
{
// Construction
public:
	CAgoraHookingDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_AGORAHOOKING_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void onButtonCloseClicked();
	void onButtonMinClicked();	

	//EventHandle
	LRESULT OnJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnRejoinChannelSuccess(WPARAM wParam, LPARAM lParam);
	LRESULT OnWarning(WPARAM wParam, LPARAM lParam);
	LRESULT OnError(WPARAM wParam, LPARAM lParam);
	LRESULT OnAudioQuality(WPARAM wParam, LPARAM lParam);
	LRESULT OnAudioVolumeIndication(WPARAM wParam, LPARAM lParam);

	LRESULT OnLeaveChannel(WPARAM wParam, LPARAM lParam);
	LRESULT OnRtcStats(WPARAM wParam, LPARAM lParam);
	LRESULT OnMediaEngineEvent(WPARAM wParam, LPARAM lParam);

	LRESULT OnAudioDeviceStateChanged(WPARAM wParam, LPARAM lParam);
	LRESULT OnVideoDeviceStateChanged(WPARAM wParam, LPARAM lParam);

	LRESULT OnRequestChannelKey(WPARAM wParam, LPARAM lParam);
	LRESULT OnLastmileQuality(WPARAM wParam, LPARAM lParam);
	LRESULT OnFirstLocalVideoFrame(WPARAM wParam, LPARAM lParam);
	LRESULT OnFirstRemoteVideoDecoded(WPARAM wParam, LPARAM lParam);
	LRESULT OnFirstRemoteVideoFrame(WPARAM wParam, LPARAM lParam);
	LRESULT OnUserJoined(WPARAM wParam, LPARAM lParam);
	LRESULT OnUserOffline(WPARAM wParam, LPARAM lParam);
	LRESULT OnUserMuteAudio(WPARAM wParam, LPARAM lParam);
	LRESULT OnUserMuteVideo(WPARAM wParam, LPARAM lParam);
	LRESULT OnApiCallExecuted(WPARAM wParam, LPARAM lParam);

	LRESULT OnLocalVideoStats(WPARAM wParam, LPARAM lParam);
	LRESULT OnRemoteVideoStats(WPARAM wParam, LPARAM lParam);
	LRESULT OnCameraReady(WPARAM wParam, LPARAM lParam);
	LRESULT OnVideoStopped(WPARAM wParam, LPARAM lParam);
	LRESULT OnConnectionLost(WPARAM wParam, LPARAM lParam);
	LRESULT OnConnectionInterrupted(WPARAM wParam, LPARAM lParam);

	LRESULT OnUserEnableVideo(WPARAM wParam, LPARAM lParam);

	LRESULT OnStartRecordingService(WPARAM wParam, LPARAM lParam);
	LRESULT OnStopRecordingService(WPARAM wParam, LPARAM lParam);
	LRESULT OnRefreshRecordingServiceStatus(WPARAM wParam, LPARAM lParam);

	LRESULT OnInviterJoinChannel(WPARAM wParam,LPARAM lParam);
		
protected:
	inline void initCtrl();
	inline void uninitCtrl();

	inline void initResource();
	inline void uninitResource();
	
	inline void getRectClient(RECT &rt);
	inline void getRectClientLeft(RECT &rt);
	inline void getRectClientRight(RECT &rt);
	
protected:
	HWND m_hWndLeftRemote;
	HWND m_hWndRightSelf;
	HWND m_hWndTitle;

	CAGButton m_AgBtnMin;
	CAGButton m_AgBtnClose;

	CAssistantBox* m_pAssistantBox;
	BOOL m_bIsCapture;

	std::string m_strAppId;
	std::string m_strAppcertificateId;
	std::string m_strChannel;
	UINT m_uLoginUid;
	std::string m_strCameraName;
	std::string m_strInstance;
	UINT m_uInviter;

	CAgoraObject* m_lpAgoraObject;
	IRtcEngine* m_lpRtcEngine;
	CExtendAudioFrameObserver* m_lpExtendAudioFrame;

	CHookPlayerInstance* m_lpHookPlayerInstance;

	const int  const KNSampelRate = 44100;
	const int  const KNChannel = 2;
	const int  const KNSampelPerCall = 44100 * 2 / 100;
};
