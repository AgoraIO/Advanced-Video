//
//  Agora Rtc Engine SDK
//
//  Created by Sting Feng in 2015-02.
//  Copyright (c) 2015 Agora IO. All rights reserved.
//

#ifndef AGORA_RTC_LIVE_SUBSCRIBER_H
#define AGORA_RTC_LIVE_SUBSCRIBER_H

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
        /**
        * The ISubscriberEventHandler event callback interface.
        */
        class ISubscriberEventHandler
        {
        public:
            virtual ~ISubscriberEventHandler() {}

            /** The first remote video frame is decoded.

            * @param uid UID of the remote user.
            * @param width Width of the video frame.
            * @param height Height of the video frame.
            * @param elapsed Time elapsed (ms) from joining a channel.
            */
            virtual void onFirstRemoteVideoDecoded(uid_t uid, int width, int height, int elapsed) {
                (void)uid;
                (void)width;
                (void)height;
                (void)elapsed;
            }

            /** The video size or rotation changed.

             * @param uid UID of the remote or local user (0)
             * @param width New width of the video.
             * @param height New height of the video.
             * @param rotation New rotation of the video.
             */
            virtual void onVideoSizeChanged(uid_t uid, int width, int height, int rotation) {
                (void)uid;
                (void)width;
                (void)height;
                (void)rotation;
            }

			/*
			* The remote user has published.

			* @param uid UID of the remote user
			* @param The media type published.
			*/
            virtual void onUserPublished(uid_t uid, MEDIA_TYPE type) {
                (void)uid;
                (void)type;
            }

			/*
			* The remote user has unpublished.

			* @param uid UID of the remote user.
			*/
            virtual void onUserUnpublished(uid_t uid) {
                (void)uid;
            }

			/*
			* The remote user's publish stream type changed.

			* @param uid UID of the remote user.
			* @param newType New media type.
			*/
            virtual void onStreamTypeChanged(uid_t uid, MEDIA_TYPE newType) {
                (void)uid;
                (void)newType;
            }
        };

        class ILiveEngine;
        enum MEDIA_TYPE;

        class ISubscriberEngine
        {
        protected:
            ~ISubscriberEngine(){}
        public:
            /**
            * Releases the subscriber engine resource.

            * @param sync

            - true: Releases the engine resources and returns after all resources have been destroyed. The application should not call release(true) in the engine's callbacks, call it this way in a separate thread instead.
            - false: Notifies engine to release its resources and returns without waiting for resources are really destroyed
            */
            virtual void release(bool sync = false) = 0;

            /**
            * Initializes the engine.

            @param engine RTC engine context
            @return

            - 0: Success.
            - < 0: Failure.
            */
            virtual int initialize(ILiveEngine *engine) = 0;

			/*
			* Sets the event handler of the subscriber engine.

			* @param eventHandler Event handler.
			*/
            virtual int setEventHandler(ISubscriberEventHandler *eventHandler) = 0;

			/* Subscribes to a remote user.
			* @param uid UID of the remote user.
			* @param type Media type.
			* @param view Reserved view.
			* @param mode Render mode.
			* @param streamType Remote user stream type.
			*/
            virtual int subscribe(uid_t uid, MEDIA_TYPE type, view_t view, RENDER_MODE_TYPE mode, REMOTE_VIDEO_STREAM_TYPE streamType) = 0;

			/* Unsubscribes a user.
			* @param uid UID of the remote user.
			*/
            virtual int unsubscribe(uid_t uid) = 0;

        };
    }
}

/** Creates the RTC live subscriber engine object and returns the pointer.
* @return The pointer of the RTC engine object.
*/
AGORA_API agora::rtc::ISubscriberEngine* AGORA_CALL createAgoraLiveSubscriberEngine();

#endif
