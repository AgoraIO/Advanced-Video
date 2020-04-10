#pragma once

#include "xaudio2.h"

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