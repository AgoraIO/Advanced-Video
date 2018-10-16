#include "StdAfx.h"
#include "AgoraObject.h"
#include "AGResourceVisitor.h"
#include "commonFun.h"
#include <stdio.h>

CAgoraObject *CAgoraObject::m_lpAgoraObject = NULL;
IRtcEngine	*CAgoraObject::m_lpAgoraEngine = NULL;
CAGEngineEventHandler CAgoraObject::m_EngineEventHandler;
CString   CAgoraObject::m_strAppID;

CAgoraObject::CAgoraObject(void)
	: m_dwEngineFlag(0)
	, m_bVideoEnable(FALSE)
	, m_bAudioEnable(TRUE)
	, m_bLocalAudioMuted(FALSE)
	, m_bScreenCapture(FALSE)
	, m_nSelfUID(0)
	, m_nRoleType(0)
	, m_nChannelProfile(0)
	, m_nRcdVol(0)
	, m_nPlaybackVol(0)
	, m_nMixVol(0)
	, m_bFullBand(FALSE)
	, m_bStereo(FALSE)
	, m_bFullBitrate(FALSE)
	, m_bLoopBack(FALSE)
{
	m_strChannelName.Empty();
	m_bLocalVideoMuted = FALSE;

	m_bAllRemoteAudioMuted = FALSE;
	m_bAllRemoteVideoMuted = FALSE;

	m_nCanvasWidth = 640;
	m_nCanvasHeight = 480;
}

CAgoraObject::~CAgoraObject(void)
{
	
}

CString CAgoraObject::GetSDKVersion()
{
	int nBuildNumber = 0;
	const char *lpszEngineVer = getAgoraRtcEngineVersion(&nBuildNumber);

	CString strEngineVer;

#ifdef UNICODE
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszEngineVer, -1, strEngineVer.GetBuffer(256), 256);
	strEngineVer.ReleaseBuffer();
#else
	strEngineVer = lpszEngineVer;
#endif

	return strEngineVer;
}

CString CAgoraObject::GetSDKVersionEx()
{
	int nBuildNumber = 0;
	const char *lpszEngineVer = getAgoraRtcEngineVersion(&nBuildNumber);

	CString strEngineVer;
	CString strVerEx;
	SYSTEMTIME sysTime;

#ifdef UNICODE
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpszEngineVer, -1, strEngineVer.GetBuffer(256), 256);
	strEngineVer.ReleaseBuffer();
#else
	strEngineVer = lpszEngineVer;
#endif

	::GetLocalTime(&sysTime);
	strVerEx.Format(_T("V%s, Build%d, %d/%d/%d, V%s"), strEngineVer, nBuildNumber, sysTime.wYear, sysTime.wMonth, sysTime.wDay, strEngineVer);
	
	return strVerEx;
}

IRtcEngine *CAgoraObject::GetEngine()
{
	if(m_lpAgoraEngine == NULL)
		m_lpAgoraEngine = createAgoraRtcEngine();

	return m_lpAgoraEngine;
}

CAgoraObject *CAgoraObject::GetAgoraObject(LPCTSTR lpVendorKey)
{
	if (m_lpAgoraObject == NULL)
		m_lpAgoraObject = new CAgoraObject();

	if(m_lpAgoraEngine == NULL)
		m_lpAgoraEngine = createAgoraRtcEngine();

	// 如果VendorKey为空则直接返回对象
	if (!m_strAppID.IsEmpty())
	{
		return m_lpAgoraObject;
	}
	if (lpVendorKey == nullptr)
	{
		return m_lpAgoraObject;
	}

	RtcEngineContext ctx;

	ctx.eventHandler = &m_EngineEventHandler;

#ifdef UNICODE
	char szVendorKey[128];

	::WideCharToMultiByte(CP_ACP, 0, lpVendorKey, -1, szVendorKey, 128, NULL, NULL);
	ctx.appId = szVendorKey;
#else
	ctx.appId = lpVendorKey;
#endif

	m_lpAgoraEngine->initialize(ctx);
	if (lpVendorKey != NULL)
		m_strAppID = lpVendorKey;

	return m_lpAgoraObject;
}

void CAgoraObject::CloseAgoraObject()
{
	if(m_lpAgoraEngine != NULL)
		m_lpAgoraEngine->release();

	if(m_lpAgoraObject != NULL)
		delete m_lpAgoraObject;

	m_lpAgoraEngine = NULL;
	m_lpAgoraObject = NULL;
}

void CAgoraObject::SetMsgHandlerWnd(HWND hWnd)
{
	m_EngineEventHandler.SetMsgReceiver(hWnd);
}

HWND CAgoraObject::GetMsgHandlerWnd()
{
	return m_EngineEventHandler.GetMsgReceiver();
}


void CAgoraObject::SetNetworkTestFlag(BOOL bEnable)
{
	if(bEnable)
		m_dwEngineFlag |= AG_ENGFLAG_ENNETTEST;
	else
		m_dwEngineFlag &= (~AG_ENGFLAG_ENNETTEST);
}

BOOL CAgoraObject::GetNetworkTestFlag()
{
	return (m_dwEngineFlag & AG_ENGFLAG_ENNETTEST) != 0;
}

void CAgoraObject::SetEchoTestFlag(BOOL bEnable)
{
	if(bEnable)
		m_dwEngineFlag |= AG_ENGFLAG_ECHOTEST;
	else
		m_dwEngineFlag &= (~AG_ENGFLAG_ECHOTEST);
}

BOOL CAgoraObject::GetEchoTestFlag()
{
	return (m_dwEngineFlag & AG_ENGFLAG_ECHOTEST) != 0;
}

void CAgoraObject::SetSpeakerphoneTestFlag(BOOL bEnable)
{
	if(bEnable)
		m_dwEngineFlag |= AG_ENGFLAG_SPKPHTEST;
	else
		m_dwEngineFlag &= (~AG_ENGFLAG_SPKPHTEST);
}

BOOL CAgoraObject::GetSpeakerphoneTestFlag()
{
	return (m_dwEngineFlag & AG_ENGFLAG_SPKPHTEST) != 0;
}

void CAgoraObject::SetMicrophoneTestFlag(BOOL bEnable)
{
	if(bEnable)
		m_dwEngineFlag |= AG_ENGFLAG_MICPHTEST;
	else
		m_dwEngineFlag &= (~AG_ENGFLAG_MICPHTEST);
}

BOOL CAgoraObject::GetMicrophoneTestFlag()
{
	return (m_dwEngineFlag & AG_ENGFLAG_MICPHTEST) != 0;
}


void CAgoraObject::SetVideoTestFlag(BOOL bEnable)
{
	if (bEnable)
		m_dwEngineFlag |= AG_ENGFLAG_VIDEOTEST;
	else
		m_dwEngineFlag &= (~AG_ENGFLAG_VIDEOTEST);
}

BOOL CAgoraObject::GetVideoTestFlag()
{
	return (m_dwEngineFlag & AG_ENGFLAG_VIDEOTEST) != 0;
}

BOOL CAgoraObject::SetLogFilePath(LPCTSTR lpLogPath)
{
	ASSERT(m_lpAgoraEngine != NULL);

	CHAR szLogPathA[MAX_DEVICE_ID_LENGTH];
	CHAR szLogPathTrans[MAX_DEVICE_ID_LENGTH];

	int ret = 0;
	RtcEngineParameters rep(*m_lpAgoraEngine);

	if (::GetFileAttributes(lpLogPath) == INVALID_FILE_ATTRIBUTES) {
		::GetModuleFileNameA(NULL, szLogPathA, MAX_DEVICE_ID_LENGTH);
		LPSTR lpLastSlash = strrchr(szLogPathA, '\\')+1;
		strcpy_s(lpLastSlash, 64, "AgoraSDK.log");
	}
	else {
#ifdef UNICODE
		::WideCharToMultiByte(CP_UTF8, 0, lpLogPath, -1, szLogPathA, MAX_DEVICE_ID_LENGTH, NULL, NULL);
#else
		::MultiByteToWideChar(CP_UTF8, 0, lpLogPath, -1, (WCHAR *)szLogPathA, MAX_DEVICE_ID_LENGTH, NULL, NULL);
#endif
	}
	
	if (_T("") == lpLogPath){

		CAGResourceVisitor::TransWinPathA(szLogPathA, szLogPathTrans, MAX_DEVICE_ID_LENGTH);

		ret = rep.setLogFile(szLogPathTrans);

		return ret == 0 ? TRUE : FALSE;
	}
	else{
		ret = rep.setLogFile(gbk2utf8(CStringA(lpLogPath).GetBuffer()).data());
		return ret == 0 ? TRUE : FALSE;
	}
}

BOOL CAgoraObject::SetVideoProfile(VIDEO_PROFILE_TYPE nVideoProfileIndex, BOOL bSwitchWidhtHeight /*= FALSE*/)
{
	int ret = m_lpAgoraEngine->setVideoProfile(nVideoProfileIndex,bSwitchWidhtHeight);

	return ret == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::SetVideoProfileEx(int nWidth, int nHeight, int nFrameRate, int nBitRate)
{
	IRtcEngine2 *lpRtcEngineEx = (IRtcEngine2 *)m_lpAgoraEngine;
	int nRet = lpRtcEngineEx->setVideoProfileEx(nWidth, nHeight, nFrameRate, nBitRate);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::SetAudioProfile(AUDIO_PROFILE_TYPE audioProfile, AUDIO_SCENARIO_TYPE scenario)
{
	ASSERT(NULL != m_lpAgoraEngine);

	int nRet = 0;

	nRet = m_lpAgoraEngine->setAudioProfile(audioProfile, scenario);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::SetAudioRecordParam(int nSampleRate, int nChannel, int nSamplePerCall)
{
	ASSERT(NULL != m_lpAgoraEngine);

	int nRet = 0;
	RtcEngineParameters rep(*m_lpAgoraEngine);

	nRet = rep.setRecordingAudioFrameParameters(nSampleRate, nChannel, RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, nSamplePerCall);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::JoinChannel(LPCTSTR lpChannelName, UINT nUID, LPCSTR lpDynamicKey)
{
	int nRet = 0;
	
	LPCSTR lpStreamInfo = "{\"owner\":true,\"width\":640,\"height\":480,\"bitrate\":500}";
#ifdef UNICODE
	CHAR szChannelName[128];

	::WideCharToMultiByte(CP_ACP, 0, lpChannelName, -1, szChannelName, 128, NULL, NULL);
	nRet = m_lpAgoraEngine->joinChannel(lpDynamicKey, szChannelName, lpStreamInfo, nUID);
#else
	nRet = m_lpAgoraEngine->joinChannel(lpDynamicKey, lpChannelName, lpStreamInfo, nUID);
#endif

	if (nRet == 0)
		m_strChannelName = lpChannelName;
	
	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::LeaveCahnnel()
{
	m_lpAgoraEngine->stopPreview();
	int nRet = m_lpAgoraEngine->leaveChannel();

	m_nSelfUID = 0;

	return nRet == 0 ? TRUE : FALSE;
}

CString CAgoraObject::GetChanelName()
{
	return m_strChannelName;
}

CString CAgoraObject::GetCallID()
{
	agora::util::AString uid;
	CString strUID;

	m_lpAgoraEngine->getCallId(uid);

#ifdef UNICODE
	::MultiByteToWideChar(CP_ACP, 0, uid->c_str(), -1, strUID.GetBuffer(128), 128);
	strUID.ReleaseBuffer();
#else
	strUID = uid->c_str();
#endif

	return strUID;
}

BOOL CAgoraObject::EnableVideo(BOOL bEnable)
{
	int nRet = 0;

	if (bEnable)
		nRet = m_lpAgoraEngine->enableVideo();
	else
		nRet = m_lpAgoraEngine->disableVideo();

	if (nRet == 0)
		m_bVideoEnable = bEnable;

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::IsVideoEnabled()
{
	return m_bVideoEnable;
}


BOOL CAgoraObject::EnableScreenCapture(HWND hWnd, int nCapFPS, LPCRECT lpCapRect, BOOL bEnable)
{
	ASSERT(m_lpAgoraEngine != NULL);

	int ret = 0;
	RtcEngineParameters rep(*m_lpAgoraEngine);

	Rect rcCap;

	if (bEnable) {
		if (lpCapRect == NULL)
			ret = rep.startScreenCapture(hWnd, nCapFPS, NULL);
		else {
			rcCap.left = lpCapRect->left;
			rcCap.right = lpCapRect->right;
			rcCap.top = lpCapRect->top;
			rcCap.bottom = lpCapRect->bottom;

			ret = rep.startScreenCapture(hWnd, nCapFPS, &rcCap);
		}
	}
	else
		ret = rep.stopScreenCapture();

	if (ret == 0)
		m_bScreenCapture = bEnable;

	return ret == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::IsScreenCaptureEnabled()
{
	return m_bScreenCapture;
}

BOOL CAgoraObject::MuteLocalAudio(BOOL bMuted)
{
	ASSERT(m_lpAgoraEngine != NULL);

	RtcEngineParameters rep(*m_lpAgoraEngine);

	int ret = rep.muteLocalAudioStream((bool)bMuted);
	if (ret == 0)
		m_bLocalAudioMuted = bMuted;

	return ret == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::IsLocalAudioMuted()
{
	return m_bLocalAudioMuted;
}

BOOL CAgoraObject::MuteAllRemoteAudio(BOOL bMuted)
{
	ASSERT(m_lpAgoraEngine != NULL);

	RtcEngineParameters rep(*m_lpAgoraEngine);

	int ret = rep.muteAllRemoteAudioStreams((bool)bMuted);
	if (ret == 0)
		m_bAllRemoteAudioMuted = bMuted;

	return ret == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::IsAllRemoteAudioMuted()
{
	return m_bAllRemoteAudioMuted;
}

BOOL CAgoraObject::MuteLocalVideo(BOOL bMuted)
{
	ASSERT(m_lpAgoraEngine != NULL);

	RtcEngineParameters rep(*m_lpAgoraEngine);

	int ret = rep.muteLocalVideoStream((bool)bMuted);
	if (ret == 0)
		m_bLocalVideoMuted = bMuted;

	return ret == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::IsLocalVideoMuted()
{
	return m_bLocalVideoMuted;
}

BOOL CAgoraObject::MuteAllRemoteVideo(BOOL bMuted)
{
	ASSERT(m_lpAgoraEngine != NULL);

	RtcEngineParameters rep(*m_lpAgoraEngine);

	int ret = rep.muteAllRemoteVideoStreams((bool)bMuted);
	if (ret == 0)
		m_bAllRemoteVideoMuted = bMuted;

	return ret == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::IsAllRemoteVideoMuted()
{
	return m_bAllRemoteVideoMuted;
}

BOOL CAgoraObject::EnableLoopBack(BOOL bEnable)
{
	int nRet = 0;

	AParameter apm(*m_lpAgoraEngine);

	if (bEnable)
		nRet = apm->setParameters("{\"che.audio.loopback.recording\":true}");
	else
		nRet = apm->setParameters("{\"che.audio.loopback.recording\":false}");

	m_bLoopBack = bEnable;

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::IsLoopBackEnabled()
{
	return m_bLoopBack;
}

BOOL CAgoraObject::SetChannelProfile(BOOL bBroadcastMode)
{
	int nRet = 0;

	if (!bBroadcastMode){
		m_nChannelProfile = CHANNEL_PROFILE_COMMUNICATION;
		nRet = m_lpAgoraEngine->setChannelProfile(CHANNEL_PROFILE_COMMUNICATION);
	}
	else {
		m_nChannelProfile = CHANNEL_PROFILE_LIVE_BROADCASTING;
		nRet = m_lpAgoraEngine->setChannelProfile(CHANNEL_PROFILE_LIVE_BROADCASTING);
	}

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::IsBroadcastMode()
{
	return m_nChannelProfile == CHANNEL_PROFILE_LIVE_BROADCASTING ? TRUE : FALSE;
}

void CAgoraObject::SetWantedRole(CLIENT_ROLE_TYPE role)
{
	m_nWantRoleType = role;
}

BOOL CAgoraObject::SetClientRole(CLIENT_ROLE_TYPE role, LPCSTR lpPermissionKey)
{
	int nRet = m_lpAgoraEngine->setClientRole(role, lpPermissionKey);

	m_nRoleType = role;

	return nRet == 0 ? TRUE : FALSE;
}


BOOL CAgoraObject::EnableExtendAudioCapture(BOOL bEnable, IAudioFrameObserver* lpAudioFrameObserver)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(m_lpAgoraEngine, agora::rtc::AGORA_IID_MEDIA_ENGINE);

	int nRet = 0;

	if (mediaEngine.get() == NULL)
		return FALSE;

	if (bEnable && lpAudioFrameObserver != NULL)
		nRet = mediaEngine->registerAudioFrameObserver(lpAudioFrameObserver);
	else
		nRet = mediaEngine->registerAudioFrameObserver(NULL);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::EnableExtendVideoCapture(BOOL bEnable, IVideoFrameObserver* lpVideoFrameObserver)
{
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(m_lpAgoraEngine, agora::rtc::AGORA_IID_MEDIA_ENGINE);

	int nRet = 0;
	AParameter apm(*m_lpAgoraEngine);

	if (mediaEngine.get() == NULL)
		return FALSE;

	if (bEnable && lpVideoFrameObserver != NULL) {
		apm->setParameters("{\"che.video.local.camera_index\":1024}");
		nRet = mediaEngine->registerVideoFrameObserver(lpVideoFrameObserver);
	}
	else {
		nRet = mediaEngine->registerVideoFrameObserver(NULL);
		apm->setParameters("{\"che.video.local.camera_index\":0}");
	}

	return nRet == 0 ? TRUE : FALSE;
}


BOOL CAgoraObject::EnableAudioRecording(BOOL bEnable, LPCTSTR lpFilePath)
{
	int ret = 0;

	RtcEngineParameters rep(*m_lpAgoraEngine);

	if (bEnable) {
#ifdef UNICODE
		CHAR szFilePath[MAX_DEVICE_ID_LENGTH];
		::WideCharToMultiByte(CP_ACP, 0, lpFilePath, -1, szFilePath, MAX_DEVICE_ID_LENGTH, NULL, NULL);
		ret = rep.startAudioRecording(szFilePath, AUDIO_RECORDING_QUALITY_HIGH);
#else
		ret = rep.startAudioRecording(lpFilePath);
#endif
	}
	else
		ret = rep.stopAudioRecording();

	return ret == 0 ? TRUE : FALSE;
}


BOOL CAgoraObject::EnableLastmileTest(BOOL bEnable)
{
	int ret = 0;

	if (bEnable)
		ret = m_lpAgoraEngine->enableLastmileTest();
	else
		ret = m_lpAgoraEngine->disableLastmileTest();

	return ret == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::LocalVideoPreview(HWND hVideoWnd, BOOL bPreviewOn)
{
	int nRet = 0;

	if (bPreviewOn) {
		VideoCanvas vc;

		vc.uid = 0;
		vc.view = hVideoWnd;
		vc.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;

		m_lpAgoraEngine->setupLocalVideo(vc);
		nRet = m_lpAgoraEngine->startPreview();
	}
	else
		nRet = m_lpAgoraEngine->stopPreview();

	return nRet == 0 ? TRUE : FALSE;
}


BOOL CAgoraObject::SetLogFilter(LOG_FILTER_TYPE logFilterType, LPCTSTR lpLogPath)
{
	int nRet = 0;
	RtcEngineParameters rep(*m_lpAgoraEngine);

	nRet = rep.setLogFilter(logFilterType);

    if (lpLogPath != NULL) {
#ifdef UNICODE
        CHAR szFilePath[MAX_DEVICE_ID_LENGTH];
        ::WideCharToMultiByte(CP_ACP, 0, lpLogPath, -1, szFilePath, MAX_DEVICE_ID_LENGTH, NULL, NULL);
        nRet |= rep.setLogFile(szFilePath);
#else
        nRet |= rep.setLogFile(lpLogPath);
#endif
    }

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::SetEncryptionSecret(LPCTSTR lpKey, int nEncryptType)
{
	CHAR szUTF8[MAX_DEVICE_ID_LENGTH];

#ifdef UNICODE
	::WideCharToMultiByte(CP_UTF8, 0, lpKey, -1, szUTF8, MAX_DEVICE_ID_LENGTH, NULL, NULL);
#else
	WCHAR szAnsi[MAX_DEVICE_ID_LENGTH];
	::MultiByteToWideChar(CP_ACP, 0, lpKey, -1, szAnsi, MAX_DEVICE_ID_LENGTH);
	::WideCharToMultiByte(CP_UTF8, 0, szAnsi, -1, szUTF8, MAX_DEVICE_ID_LENGTH, NULL, NULL);
#endif
    switch (nEncryptType)
    {
    case 0:
        m_lpAgoraEngine->setEncryptionMode("aes-128-xts");
    	break;
    case 1:
        m_lpAgoraEngine->setEncryptionMode("aes-256-xts");
        break;
    default:
        m_lpAgoraEngine->setEncryptionMode("aes-128-xts");
        break;
    }
	int nRet = m_lpAgoraEngine->setEncryptionSecret(szUTF8);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::EnableLocalRender(BOOL bEnable)
{
	int nRet = 0;

	AParameter apm(*m_lpAgoraEngine);

	if (bEnable)
		nRet = apm->setParameters("{\"che.video.local.render\":true}");
	else
		nRet = apm->setParameters("{\"che.video.local.render\":false}");

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::EnableLocalMirrorImage(BOOL bMirrorLocal)
{
	int nRet = 0;

	AParameter apm(*m_lpAgoraEngine);

	if (bMirrorLocal)
		nRet = apm->setParameters("{\"che.video.localViewMirrorSetting\":\"forceMirror\"}");
	else
		nRet = apm->setParameters("{\"che.video.localViewMirrorSetting\":\"disableMirror\"}");

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::SetVideoRenderType(int nType)
{
	int	nRet = 0;

	AParameter apm(this->m_lpAgoraEngine);
	switch (nType)
	{
	case 1:
		nRet = apm->setInt("che.video.renderer.type", VideoRenderType::kRenderWindowsGDI);
		break;
	case 2:
		nRet = apm->setInt("che.video.renderer.type", VideoRenderType::kRenderWindowsD3D);
		break;
	case 3:
		nRet = apm->setInt("che.video.renderer.type", VideoRenderType::kRenderWindowsD2D);
		break;
	default:
		nRet = apm->setInt("che.video.renderer.type", VideoRenderType::kRenderWindowsD3D);
		break;
	}

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::EnableWebSdkInteroperability(BOOL bEnable)
{
	RtcEngineParameters rep(*m_lpAgoraEngine);

	int	nRet = rep.enableWebSdkInteroperability(bEnable);

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::EnableLocalPublishLeftRotate90(BOOL bEnable)
{
	int nRet = 0;

	AParameter apm(*m_lpAgoraEngine);

	if (bEnable){
		nRet = apm->setParameters("{\"che.video.capture.rotate\":90}");
	}

	return nRet;
}

int CAgoraObject::CreateMessageStream()
{
    int nDataStream = 0;
    m_lpAgoraEngine->createDataStream(&nDataStream, true, true);

    return nDataStream;
}

BOOL CAgoraObject::SendChatMessage(int nStreamID, LPCTSTR lpChatMessage)
{
    _ASSERT(nStreamID != 0);
    int nMessageLen = _tcslen(lpChatMessage);
    _ASSERT(nMessageLen < 128);

    CHAR szUTF8[256];

#ifdef UNICODE
    int nUTF8Len = ::WideCharToMultiByte(CP_UTF8, 0, lpChatMessage, nMessageLen, szUTF8, 256, NULL, NULL);
#else
    int nUTF8Len = ::MultiByteToWideChar(CP_UTF8, lpChatMessage, nMessageLen, szUTF8, 256);
#endif

    int nRet = m_lpAgoraEngine->sendStreamMessage(nStreamID, szUTF8, nUTF8Len);

    return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::SetHighQualityAudioPreferences(BOOL bFullBand, BOOL bStereo, BOOL bFullBitrate)
{
	int nRet = 0;
	RtcEngineParameters rep(*m_lpAgoraEngine);

	nRet = rep.setHighQualityAudioParameters(bFullBand, bStereo, bFullBitrate);

	m_bFullBand = bFullBand;
	m_bStereo = bStereo;
	m_bFullBitrate = bFullBitrate;

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::StartAudioMixing(LPCTSTR lpMusicPath, BOOL bLoopback, BOOL bReplace, int nCycle)
{
	int nRet = 0;
	RtcEngineParameters rep(*m_lpAgoraEngine);

#ifdef UNICODE
	CHAR szFilePath[MAX_DEVICE_ID_LENGTH];
	::WideCharToMultiByte(CP_UTF8, 0, lpMusicPath, -1, szFilePath, MAX_DEVICE_ID_LENGTH, NULL, NULL);
	nRet = rep.startAudioMixing(szFilePath, bLoopback, bReplace, nCycle);
#else
	nRet = rep.startAudioMixing(lpMusicPath, bLoopback, bReplace, nCycle);
#endif

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::StopAudioMixing()
{
	int nRet = 0;
	RtcEngineParameters rep(*m_lpAgoraEngine);

	nRet = rep.stopAudioMixing();

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::PauseAudioMixing()
{
	int nRet = 0;
	RtcEngineParameters rep(*m_lpAgoraEngine);

	nRet = rep.pauseAudioMixing();

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::ResumeAudioMixing()
{
	int nRet = 0;
	RtcEngineParameters rep(*m_lpAgoraEngine);

	nRet = rep.resumeAudioMixing();

	return nRet == 0 ? TRUE : FALSE;
}

/*
BOOL CAgoraObject::EnableInEarMonitoring(BOOL bEnable)
{
	int nRet = m_lpAgoraEngineEx->enable

	nRet = rep.enablee

	return nRet == 0 ? TRUE : FALSE;
}
*/

BOOL CAgoraObject::EnableAudio(BOOL bEnable)
{
	int nRet = 0;
	
	if (bEnable)
		nRet = m_lpAgoraEngine->enableAudio();
	else
		nRet = m_lpAgoraEngine->disableAudio();

	m_bAudioEnable = bEnable;

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::IsAudioEnabled()
{
	return m_bAudioEnable;
}

void CAgoraObject::SetSEIInfo(UINT nUID, LPSEI_INFO lpSEIInfo)
{
	SEI_INFO seiInfo;

	ASSERT(nUID != 0xcccccccc);
	memset(&seiInfo, 0, sizeof(SEI_INFO));

	seiInfo.nUID = nUID;
	if (lpSEIInfo != NULL)
		memcpy(&seiInfo, lpSEIInfo, sizeof(SEI_INFO));

	m_mapSEIInfo.SetAt(nUID, seiInfo);
}

void CAgoraObject::RemoveSEIInfo(UINT nUID)
{
	m_mapSEIInfo.RemoveKey(nUID);
}

void CAgoraObject::RemoveAllSEIInfo()
{
	m_mapSEIInfo.RemoveAll();
}

BOOL CAgoraObject::GetSEIInfo(UINT nUID, LPSEI_INFO lpSEIInfo)
{
	SEI_INFO seiInfo;

	if (!m_mapSEIInfo.Lookup(nUID, seiInfo))
		return FALSE;

	memcpy(lpSEIInfo, &seiInfo, sizeof(SEI_INFO));

	return TRUE;
}

BOOL CAgoraObject::GetSEIInfoByIndex(int nIndex, LPSEI_INFO lpSEIInfo)
{
	int		nCounter = 0;

	if (nIndex < 0 || nIndex >= m_mapSEIInfo.GetCount())
		return FALSE;

	POSITION pos = m_mapSEIInfo.GetStartPosition();
	while (pos != NULL && nCounter < nIndex) {
		m_mapSEIInfo.GetNext(pos);
		nCounter++;
	}

	*lpSEIInfo = m_mapSEIInfo.GetValueAt(pos);
	
	return TRUE;
}

BOOL CAgoraObject::EnableSEIPush(BOOL bEnable, COLORREF crBack)
{
	CStringA	strBackColor;
	VideoCompositingLayout layout;
	int	nRet = 0;

	int nVideoCount = m_mapSEIInfo.GetCount();
	if (nVideoCount <= 0)
		return FALSE;

	if (!bEnable) {
		nRet = m_lpAgoraEngine->clearVideoCompositingLayout();
		return nRet == 0 ? TRUE : FALSE;
	}

	VideoCompositingLayout::Region *lpRegion = new VideoCompositingLayout::Region[nVideoCount];
	memset(lpRegion, 0, sizeof(VideoCompositingLayout::Region)*nVideoCount);

	POSITION pos = m_mapSEIInfo.GetStartPosition();
	int nIndex = 0;

	while (pos != NULL) {
		SEI_INFO &seiInfo = m_mapSEIInfo.GetNextValue(pos);

		lpRegion[nIndex].height = seiInfo.nHeight < m_nCanvasHeight ? (seiInfo.nHeight *1.0)/ m_nCanvasHeight : 1;
		lpRegion[nIndex].width = seiInfo.nWidth < m_nCanvasWidth ? seiInfo.nWidth*1.0 / m_nCanvasWidth : 1;
		lpRegion[nIndex].uid = seiInfo.nUID;
		
		lpRegion[nIndex].x = seiInfo.x < m_nCanvasWidth ? seiInfo.x*1.0 / m_nCanvasWidth : 1;
		lpRegion[nIndex].y = seiInfo.y < m_nCanvasHeight ? seiInfo.y*1.0 / m_nCanvasHeight : 1;
		lpRegion[nIndex].renderMode = RENDER_MODE_FIT;
		lpRegion[nIndex].zOrder = seiInfo.nIndex;
		lpRegion[nIndex].alpha = 1;
		nIndex++;
	}

	strBackColor.Format("#%08X", crBack);
	layout.backgroundColor = strBackColor;
	layout.canvasWidth = m_nCanvasWidth;
	layout.canvasHeight = m_nCanvasHeight;
	layout.regions = lpRegion;
	layout.regionCount = nVideoCount;
	
	nRet = m_lpAgoraEngine->setVideoCompositingLayout(layout);

	delete[] lpRegion;

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::EnableH264Compatible()
{
	AParameter apm(*m_lpAgoraEngine);

	int nRet = apm->setParameters("{\"che.video.h264_profile\":66}");

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::AdjustVolume(int nRcdVol, int nPlaybackVol, int nMixVol)
{
	int nRet = 0;
	RtcEngineParameters rep(*m_lpAgoraEngine);

	nRet &= rep.adjustRecordingSignalVolume(nRcdVol);
	nRet &= rep.adjustPlaybackSignalVolume(nPlaybackVol);
	nRet &= rep.adjustAudioMixingVolume(nMixVol);

	return nRet == 0 ? TRUE : FALSE;
}

void CAgoraObject::GetVolume(int *nRcdVol, int *nPlaybackVol, int *nMixVol)
{
	*nRcdVol = m_nRcdVol;
	*nPlaybackVol = m_nPlaybackVol;
	*nMixVol = m_nMixVol;
}

int CAgoraObject::GetAudioMixingPos()
{
	RtcEngineParameters rep(*m_lpAgoraEngine);

	return rep.getAudioMixingCurrentPosition();
}

int CAgoraObject::GetAudioMixingDuration()
{
	RtcEngineParameters rep(*m_lpAgoraEngine);

	return rep.getAudioMixingDuration();
}


void CAgoraObject::SetSelfResolution(int nWidth, int nHeight)
{
	m_nCanvasWidth = nWidth;
	m_nCanvasHeight = nHeight;
}

void CAgoraObject::GetSelfResolution(int *nWidth, int *nHeight)
{
	*nWidth = m_nCanvasWidth;
	*nHeight = m_nCanvasHeight;
}

BOOL CAgoraObject::EnableWhiteboardVer(BOOL bEnable)
{
	// HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Internet Explorer\MAIN\FeatureControl\FEATURE_BROWSER_EMULATION
	HKEY hKey = NULL;

	LSTATUS lStatus = ::RegCreateKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Wow6432Node\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION"), 0, REG_OPTION_NON_VOLATILE
		, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

	if (lStatus != ERROR_SUCCESS)
		return FALSE;

	DWORD dwIEVer = 11001;

	if (bEnable)
		lStatus = ::RegSetValueEx(hKey, _T("AgoraVideoCall.exe"), 0, REG_DWORD, (const BYTE*)&dwIEVer, sizeof(DWORD));
	else
		lStatus = ::RegDeleteKeyValue(hKey, NULL, _T("AgoraVideoCall.exe"));

	::RegCloseKey(hKey);

	return lStatus == ERROR_SUCCESS ? TRUE : FALSE;
}

BOOL CAgoraObject::EnableWhiteboardFeq(BOOL bEnable)
{
	// HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Internet Explorer\MAIN\FeatureControl\FEATURE_BROWSER_EMULATION
	HKEY hKey = NULL;
	
	LSTATUS lStatus = ::RegCreateKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Wow6432Node\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_MANAGE_SCRIPT_CIRCULAR_REFS"), 0, REG_OPTION_NON_VOLATILE
		, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL);

	if (lStatus != ERROR_SUCCESS)
		return FALSE;

	DWORD dwValue = 1;

	if (bEnable)
		lStatus = ::RegSetValueEx(hKey, _T("AgoraVideoCall.exe"), 0, REG_DWORD, (const BYTE*)&dwValue, sizeof(DWORD));
	else
		lStatus = ::RegDeleteKeyValue(hKey, NULL, _T("AgoraVideoCall.exe"));

	::RegCloseKey(hKey);

	return lStatus == ERROR_SUCCESS ? TRUE : FALSE;
}

void CAgoraObject::setPublishParam(const AGE_PUBLISH_PARAM &publishparam)
{
	m_publishParam = publishparam;
}

void CAgoraObject::getPublishParam(AGE_PUBLISH_PARAM &publishParam)
{
	publishParam = m_publishParam;
}

bool CAgoraObject::enablePublish(bool enable /*= true*/)
{
	if (enable){
		PublisherConfiguration publishparam;
		publishparam.width = m_publishParam.width;
		publishparam.height = m_publishParam.height;
		publishparam.framerate = m_publishParam.fps;
		publishparam.bitrate = m_publishParam.bitrate;
		publishparam.rawStreamUrl = m_publishParam.rtmpUrl.data();
		publishparam.extraInfo = "{\"lowDelay\":true}";

		m_lpAgoraEngine->configPublisher(publishparam);
	}
	else
	{

	}

	return true;
}

BOOL CAgoraObject::setAudioAEC(BOOL bEnable /*= FALSE*/)
{
	int nRet = 0;

	AParameter apm(m_lpAgoraEngine);

	if (bEnable)
		nRet = apm->setParameters("{\"che.audio.enable.aec\":true}");
	else
		nRet = apm->setParameters("{\"che.audio.enable.aec\":false}");

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::setAudioNS(BOOL bEnable /*= FALSE*/)
{
	int nRet = 0;

	AParameter apm(m_lpAgoraEngine);

	if (bEnable)
		nRet = apm->setParameters("{\"che.audio.enable.ns\":true}");
	else
		nRet = apm->setParameters("{\"che.audio.enable.ns\":false}");

	return nRet == 0 ? TRUE : FALSE;
}

BOOL CAgoraObject::setAudioAgcOn(BOOL bEnable /*= FALSE*/)
{
	int nRet = 0;

	AParameter apm(m_lpAgoraEngine);

	if (bEnable)
		nRet = apm->setParameters("{\"che.audio.agcOn\":true}");
	else
		nRet = apm->setParameters("{\"che.audio.agcOn\":false}");

	return nRet == 0 ? TRUE : FALSE;
}