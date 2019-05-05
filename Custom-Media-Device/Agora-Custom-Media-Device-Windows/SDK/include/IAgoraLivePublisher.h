//
//  Agora Rtc Engine SDK
//
//  Created by Sting Feng in 2015-02.
//  Copyright (c) 2015 Agora IO. All rights reserved.
//

#ifndef AGORA_RTC_LIVE_PUBLISHER_H
#define AGORA_RTC_LIVE_PUBLISHER_H

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define AGORA_CALL __cdecl
#if defined(AGORARTC_EXPORT)
#define AGORA_API extern "C" __declspec(dllexport)
#else
#define AGORA_API extern "C" __declspec(dllimport)
#endif
#elif defined(__APPLE__)
#define AGORA_API __attribute__((visibility("default"))) extern "C"
#define AGORA_CALL
#elif defined(__ANDROID__) || defined(__linux__)
#define AGORA_API extern "C" __attribute__((visibility("default")))
#define AGORA_CALL
#else
#define AGORA_API extern "C"
#define AGORA_CALL
#endif
#include "IAgoraRtcEngine.h"
namespace agora {

    namespace rtc {
        class ILiveEngine;
        enum MEDIA_TYPE;
        /**
        * The IPublisherEventHandler event callback interface.
        */
        class IPublisherEventHandler
        {
        public:
            virtual ~IPublisherEventHandler() {}

            /**
            * A publish URL is added.

            * @param url Added URL.
            */
            virtual void onUrlAdded(const char *url) {
                (void)url;
            }

            /**
            * A publish URL is removed.

            * @param url Removed URL.
            */
            virtual void onUrlRemoved(const char *url) {
                (void)url;
            }

            /**
            * Publish has succeeded.
            */
            virtual void onPublished() {
            }

			/*
			* Unpublish has succeeded.
			*/
            virtual void onUnpublished() {
            }

            /**
            * Transcoding is updated.
            */
            virtual void onTranscodingUpdated() {
            }

			/*
			* An inject stream is requested.
			*/
            virtual void onStreamInjectedStatus(const char* url, uid_t uid, int status) {
                (void)url;
                (void)uid;
                (void)status;
            }
        };

        class IPublisherEngine
        {
        protected:
            virtual ~IPublisherEngine(){}
        public:
            /**
            * Releases the engine resource
            */
            virtual void release() = 0;

            /**
            * Initializes the engine

            * @param engine ILiveEngine instance
            @return

            - 0: Success.
            - < 0: Failure.
            */
            virtual int initialize(ILiveEngine *engine) = 0;

			/*
			* Sets the event handler for the publisher engine.

			* @param eventHandler Registered event handler.
			*/
            virtual int setEventHandler(IPublisherEventHandler *eventHandler) = 0;

			/*
			* Publishes a stream.
			*/
            virtual int publish() = 0;

			/*
			* Unpublishes a stream.
			*/
            virtual int unpublish() = 0;

			/*
			* Adds a publish stream URL.

			* @param url Publish stream URL.
			* @param transcodingEnabled Whether the transcoding is enabled: true/false.
			*/
            virtual int addStreamUrl(const char *url, bool transcodingEnabled) = 0;

			/*
			* Removes a publish stream URL.

			* @param url Publish stream URL to remove.
			*/
            virtual int removeStreamUrl(const char *url) = 0;

			/*
			* Sets the publish media type.

			* @param type The media type: #MEDIA_TYPE
			*/
            virtual int setMediaType(MEDIA_TYPE type) = 0;

			/*
			* Sets the live transcoding.

			* @param transcoding The transcoding option: #LiveTranscoding
			*/
            virtual int setLiveTranscoding(const LiveTranscoding &transcoding) = 0;

			/*
			* Adds the inject stream URL.

			* @param url The added injected stream URL.
			* @param config The inject stream configuration: InjectStreamConfig
			*/
            virtual int addInjectStreamUrl(const char* url, const InjectStreamConfig& config) = 0;

			/*
			* Removes the injected stream URL.

			* @param url The injected stream URL to remove.
			*/
            virtual int removeInjectStreamUrl(const char* url) = 0;

			/*
			* Sets the video profile.

			* @param profile The video profile: VIDEO_PROFILE_TYPE
			*/
            virtual int setVideoProfile(VIDEO_PROFILE_TYPE profile) = 0;
        };
    }
}

/**
* Creates the RTC live publisher engine object and returns the pointer.

* @return The pointer of the RTC engine object.
*/
AGORA_API agora::rtc::IPublisherEngine* AGORA_CALL createAgoraLivePublisherEngine();
#endif
