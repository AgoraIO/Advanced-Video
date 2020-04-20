#pragma once
#include "AGComboBox.h"
#include "AGButton.h"
#include "AGDShowAudioCapture.h"
#include "AGAudioCapture.h"

#include "AGDShowVideoCapture.h"
#include "AGVideoCapture.h"

#include "AGAudioCapture.h"
#include "XAudioPlayout.h"

#include "ExtendVideoFrameObserver.h"
#include "ExtendAudioFrameObserver.h"
#include "StreamingVoiceContext.h"

#include "afxwin.h"

// CExtCaptureDlg 对话框

typedef struct _PLAYOUT_THREAD_PARAM
{
	HANDLE		hExitEvent;

	IXAudio2VoiceCallback	*lpXAudioVoiceContext;
	IXAudio2SourceVoice		*lpXAudioSourceVoice;

} PLAYOUT_THREAD_PARAM, *PPLAYOUT_THREAD_PARAM, *LPPLAYOUT_THREAD_PARAM;


class CExtCaptureDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExtCaptureDlg)

public:
	CExtCaptureDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExtCaptureDlg();

// 对话框数据
	enum { IDD = IDD_EXTCAP_DIALOG };

	BOOL VideoCaptureControl(BOOL bStart);
	BOOL AudioCaptureControl(BOOL bStart);

	BOOL IsExtVideoCapEnabled() const { return m_ckExtVideoCapture.GetCheck(); };
	BOOL IsExtAudioCapEnabled() const { return m_ckExtAudioCapture.GetCheck(); };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();

	afx_msg void OnBnClickedBtncancelExtcap();
	afx_msg void OnBnClickedBtnconfirmExtcap();
	afx_msg void OnBnClickedBtnapplyExtcap();

	afx_msg void OnCmbselCameraDevice();
	afx_msg void OnCmbselCameraCapability();

	afx_msg void OnCmbselMicroDevice();
	afx_msg void OnCmbselMicroCapability();

	afx_msg void OnCmbselPlayoutDevice();

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	DECLARE_MESSAGE_MAP()

protected:
	void InitCtrls();
	void DrawClient(CDC *lpDC);

private:
	CAGComboBox     m_cmbCamera;
	CAGComboBox     m_cmbCamCap;

	CAGComboBox		m_cmbMicrophone;
	CAGComboBox		m_cmbMicCap;

	CAGComboBox		m_cmbPlayout;

	CAGButton		m_btnCancel;
	CAGButton		m_btnConfirm;
	CAGButton		m_btnApply;

	CButton			m_ckExtVideoCapture;
	CButton			m_ckExtAudioCapture;

	CFont			m_ftDes;
	CFont			m_ftHead;
	CPen            m_penFrame;

private:
	CAGDShowVideoCapture	m_agVideoCaptureDevice;
	CAGVideoCapture			m_agVideoCapture;

	CAGDShowAudioCapture	m_agAudioCaptureDevice;
	CAGAudioCapture			m_agAudioCapture;

	CXAudioPlayout			m_agXAudioPlayoutDevice;

	CExtendVideoFrameObserver	m_exCapVideoFrameObserver;
	CExtendAudioFrameObserver	m_exCapAudioFrameObserver;
	StreamingVoiceContext		m_exCapVoiceContext;

	HANDLE						m_hExitPlayEvent;
	PLAYOUT_THREAD_PARAM		m_playThreadParam;

private:
	static UINT PlayoutThread(LPVOID lParam);
	 

};
