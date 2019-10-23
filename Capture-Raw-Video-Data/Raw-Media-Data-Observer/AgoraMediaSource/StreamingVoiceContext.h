#pragma once

#include "C:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include/xaudio2.h"

struct StreamingVoiceContext : public IXAudio2VoiceCallback
{
	STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32)
	{
	}
	STDMETHOD_(void, OnVoiceProcessingPassEnd)()
	{
	}
	STDMETHOD_(void, OnStreamEnd)()
	{
	}
	STDMETHOD_(void, OnBufferStart)(void*)
	{
	}
	STDMETHOD_(void, OnBufferEnd)(void*)
	{
		SetEvent(hBufferEndEvent);
	}
	STDMETHOD_(void, OnLoopEnd)(void*)
	{
	}
	STDMETHOD_(void, OnVoiceError)(void*, HRESULT)
	{
	}

	HANDLE hBufferEndEvent;

	StreamingVoiceContext() : hBufferEndEvent(CreateEvent(NULL, FALSE, FALSE, NULL))
	{
	}
	virtual ~StreamingVoiceContext()
	{
		CloseHandle(hBufferEndEvent);
	}
};