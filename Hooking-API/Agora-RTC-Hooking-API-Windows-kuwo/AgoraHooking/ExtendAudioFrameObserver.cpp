#include "stdafx.h"
#include "ExtendAudioFrameObserver.h"
#include "AGResourceVisitor.h"
#include "CHookPlayerInstance.h"

CExtendAudioFrameObserver::CExtendAudioFrameObserver()
{
	pPlayerData = new BYTE[0x800000];
	DeleteFile(_T("./AgoraHookLog/MusicDest.pcm"));
	DeleteFile(_T("./AgoraHookLog/FrameMix.pcm"));
	DeleteFile(_T("./AgoraHookLog/PlayOut.pcm"));
}

CExtendAudioFrameObserver::~CExtendAudioFrameObserver()
{
	delete[] pPlayerData;
}

bool CExtendAudioFrameObserver::onRecordAudioFrame(AudioFrame& audioFrame)
{
	SIZE_T nSize = audioFrame.channels*audioFrame.samples * 2;

	if (gIsDebugMode) {

		static int nCountAudioCallBack = 0;
		nCountAudioCallBack++;
		static DWORD dwLastStamp = GetTickCount();
		DWORD dwCurrStamp = GetTickCount();
		if (5000 < dwCurrStamp - dwLastStamp) {

			float fRecordAudioFrame = nCountAudioCallBack * 1000.0 / (dwCurrStamp - dwLastStamp);
			char logMsg[128] = { '\0' };
			sprintf_s(logMsg, "RecordAudioFrame :%d , %d,16,%d [ Rate : %.2f]\n", nSize, audioFrame.channels, audioFrame.samplesPerSec, fRecordAudioFrame);
			OutputDebugStringA(logMsg);

			FILE* log;
			log = fopen("./AgoraHookLog/PlayerHookerV6_1.log", ("a+"));
			if (log != NULL)
			{
				SYSTEMTIME st;
				GetLocalTime(&st);
				fprintf(log, "%d%02d%02d-%02d%02d%02d%03d:  %s", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, logMsg);
				fclose(log);
			}

			dwLastStamp = dwCurrStamp;
			nCountAudioCallBack = 0;
		}
	}

	unsigned int datalen = 0;
	//CHookPlayerInstance::getHookInstance()->popAudioBuffer(pPlayerData, nSize, datalen);
	CHookPlayerInstance::getHookInstance()->getCicleBuffer()->readBuffer(pPlayerData, nSize, &datalen);

	if (gIsSaveDumpPcm)
	{
		FILE* outfile1 = fopen("./AgoraHookLog/MusicDest.pcm", "ab+");
		if (outfile1)
		{
			fwrite(this->pPlayerData, 1, datalen, outfile1);
			fclose(outfile1);
			outfile1 = NULL;
		}
	}

	int nMixLen = nSize;
	if (nSize > 0 && datalen > 0)
	{
		int nMixLen = datalen > nSize ? nSize : datalen;
		CAGResourceVisitor::MixerAddS16((int16_t*)audioFrame.buffer, (int16_t*)pPlayerData, (audioFrame.channels * audioFrame.bytesPerSample) *  audioFrame.samples / sizeof(int16_t));
	}

	if (gIsSaveDumpPcm)
	{
		FILE* outfile = fopen("./AgoraHookLog/FrameMix.pcm", "ab+");
		if (outfile)
		{
			fwrite(audioFrame.buffer, 1, nMixLen, outfile);
			fclose(outfile);
			outfile = NULL;
		}
	}

	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame)
{
	SIZE_T nSize = audioFrame.channels*audioFrame.samples * 2;
//	CAudioPlayPackageQueue::GetInstance()->PushAudioPackage(audioFrame.buffer, nSize);
// 	FILE * recordf = fopen("d:/playback.pcm", "ab+");
// 	fwrite(audioFrame.buffer, 1, nSize, recordf);
// 	fclose(recordf);
#if 0
	if (bIsDebugMode)
	{
		FILE* outfile1 = fopen("./AgoraHookLog/PlayOut.pcm", "ab+");
		if (outfile1)
		{
			fwrite(this->pPlayerData, 1, nSize, outfile1);
			fclose(outfile1);
			outfile1 = NULL;
		}
	}
#endif
	
	return true;
}

bool CExtendAudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame)
{
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame)
{
	return true;
}
