#pragma once

#include "../SDK/include/IAgoraRtcEngine.h"
#include "../SDK/include/IAgoraMediaEngine.h"
#include "../SDK/include/IAgoraRtcEngine2.h"
#include "AGEngineEventHandler.h"
#include "../SDK/include/AgoraBase.h"

#include "ExtendVideoFrameObserver.h"
#include "ExtendAudioFrameObserver.h"

#define AVC_VER _T("V1.13.0, Build234, 09/11/2017, SDK1.13.0")

using namespace agora;
using namespace agora::media;

// 引擎标记位
#define AG_ENGFLAG_ENNETTEST	0x00000001
#define AG_ENGFLAG_ECHOTEST		0x00000002
#define AG_ENGFLAG_SPKPHTEST	0x00000004
#define AG_ENGFLAG_MICPHTEST	0x00000008
#define AG_ENGFLAG_VIDEOTEST	0x00000010


#define AG_CODEC_E264	0x00000000
#define AG_CODEC_EVP	0x00000001
#define AG_CODEC_VP8	0x00000002

#define APP_ID _T("0c0b4b61adf94de1befd7cdd78a50444")


class CAgoraObject
{
public:
	~CAgoraObject(void);

	enum {
		VIDEO_EXCAPTURE_OFF = 0x00000000,
		VIDEO_EXCAPTURE_OBSERVER = 0x00000001,
		VIDEO_EXCAPTURE_OBSERVER_VERTICAL = 0x00000002,
		VIDEO_EXCAPTURE_PUSH = 0x00000003,
		VIDEO_EXCAPTURE_RENDERFACTORY = 0x00000004,
	};

	void SetNetworkTestFlag(BOOL bEnable);
	BOOL GetNetworkTestFlag();

	void SetEchoTestFlag(BOOL bEnable);
	BOOL GetEchoTestFlag();

	void SetSpeakerphoneTestFlag(BOOL bEnable);
	BOOL GetSpeakerphoneTestFlag();

	void SetMicrophoneTestFlag(BOOL bEnable);
	BOOL GetMicrophoneTestFlag();

	void SetVideoTestFlag(BOOL bEnable);
	BOOL GetVideoTestFlag();

	void SetMsgHandlerWnd(HWND hWnd);
	HWND GetMsgHandlerWnd();

	BOOL SetLogFilePath(LPCTSTR lpLogPath = NULL);

	BOOL JoinChannel(LPCTSTR lpChannelName, UINT nUID = 0);
	BOOL LeaveCahnnel();
	CString GetChanelName();
	CString GetCallID();
	CString GetVendorKey() { return m_strVendorKey; };

	void SetSelfUID(UINT nUID) { m_nSelfUID = nUID; };
	UINT GetSelfUID() { return m_nSelfUID; };

	BOOL EnableVideo(BOOL bEnable = TRUE);
	BOOL IsVideoEnabled();

	BOOL EnableScreenCapture(HWND hWnd, int nCapFPS = 15, const LPRECT lpRect = NULL, BOOL bEnable = TRUE, int nBitrate = 0);
	BOOL IsScreenCaptureEnabled();

	BOOL MuteLocalAudio(BOOL bMuted = TRUE);
	BOOL IsLocalAudioMuted();

	BOOL MuteLocalVideo(BOOL bMuted = TRUE);
	BOOL IsLocalVideoMuted();

	BOOL EnableAudioRecording(BOOL bEnable, LPCTSTR lpFilePath);

	BOOL EnableNetworkTest(BOOL bEnable);

	BOOL EnableEchoTest(BOOL bEnable = TRUE);
	BOOL IsEchoTesting() { return m_bEchoTest; };

	BOOL SetVideoProfileEx(int nWidth, int nHeight, int nFrameRate, int nBitRate);
	BOOL SetAudioProfileEx(int nSampleRate, int nChannels, int nSamplesPerCall);

	BOOL EnableExtendAudioCapture(BOOL bEnable, IAudioFrameObserver* lpAudioFrameObserver);
	BOOL EnableExtendVideoCapture(BOOL bEnable);

	BOOL LocalVideoPreview(HWND hVideoWnd, BOOL bPreviewOn = TRUE);

	BOOL SetLogFilter(LOG_FILTER_TYPE logFilterType, LPCTSTR lpLogPath);

    BOOL SetEncryptionSecret(LPCTSTR lpKey, int nEncryptType = 0);

    BOOL EnableLocalRender(BOOL bEnable);

    int CreateMessageStream();
    BOOL SendChatMessage(int nStreamID, LPCTSTR lpChatMessage);

	static IRtcEngine *GetEngine();
	
	static CString GetSDKVersion();

	static BOOL EnableWhiteboardVer(BOOL bEnable);
	static BOOL EnableWhiteboardFeq(BOOL bEnable);

	BOOL PushVideoFrame(LPCVOID lpYUVBuffer, int nWidth, int nHeight);
	UINT64 WINAPI GetHighAccuracyTickCount();
	void setExtendVideoCaptureMode(const UINT &nVideoCaptureMode) { m_nExtCaptureModeVideo = nVideoCaptureMode; }
	UINT GetExtendVideoCaptureMode() const { return m_nExtCaptureModeVideo; };

protected:
	CAgoraObject(void);

private:
	DWORD	m_dwEngineFlag;
	static  CAgoraObject	*m_lpAgoraObject;
	static	IRtcEngine	    *m_lpAgoraEngine;
	static	CString			m_strVendorKey;
	
	UINT		m_nSelfUID;
	CString		m_strChannelName;
	BOOL		m_bVideoEnable;

	BOOL		m_bLocalAudioMuted;
	BOOL		m_bLocalVideoMuted;
	BOOL		m_bEchoTest;

	BOOL		m_bScreenCapture;
	UINT			m_nExtCaptureModeVideo;

//	int			m_nCodecType;

public:
	static CAgoraObject *GetAgoraObject(LPCTSTR lpVendorKey = NULL);
	static void CloseAgoraObject();

	static CAGEngineEventHandler m_EngineEventHandler;
};
