//
//  Agora Rtc Engine SDK
//
//  Copyright (c) 2018 Agora IO. All rights reserved.
//

#ifndef AGORA_RTC_LIVE_ENGINE_H
#define AGORA_RTC_LIVE_ENGINE_H

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

#include "IAgoraRtcEngine.h"
#include "IAgoraLivePublisher.h"
#include "IAgoraLiveSubscriber.h"
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
namespace agora {

namespace rtc {

/*
* Media stream type.
*/
enum MEDIA_TYPE
{
	/*
	* No audio and video.
	*/
    MEDIA_TYPE_NONE = 0,
	/*
	* Audio only.
	*/
    MEDIA_TYPE_AUDIO_ONLY = 1,
	/*
	* Video only.
	*/
    MEDIA_TYPE_VIDEO_ONLY = 2,
	/*
	* Audio and video.
	*/
    MEDIA_TYPE_AUDIO_AND_VIDEO = 3
};

/*
* Channel configuration.
*/
struct LiveChannelConfig {
    bool videoEnabled;
};

/*
* Stream statitics.
*/
typedef RtcStats LiveStats;

/**
* Event callback interface.
*/
class ILiveEngineEventHandler
{
public:
    virtual ~ILiveEngineEventHandler() {}

    /** A warning message has occurred.
     * @param warn Warning code: #WARN_CODE_TYPE

     */
    virtual void onWarning(int warn) {
        (void)warn;
    }

    /** An error message has occurred.
     * @param err Error code: #ERROR_CODE_TYPE
     */
    virtual void onError(int err) {
        (void)err;
    }

    /** A user or host has joined the channel.
    * @param channel Channel name
    * @param uid User ID of the user or host who joined the channel
    * @param elapsed Time elapsed (ms) from \ref ILiveEngine::joinChannel "joinChannel" until this callback function is triggered.
    */
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
        (void)channel;
        (void)uid;
        (void)elapsed;
    }

    /** A user has rejoined the channel.
    * @param channel Channel name
    * @param uid User ID of the user who rejoined the channel
    * @param elapsed Time elapsed (ms) from \ref ILiveEngine::joinChannel "joinChannel" until this callback function is triggered.
    */
    virtual void onRejoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
        (void)channel;
        (void)uid;
        (void)elapsed;
    }

	/** A user has left the channel.
	*/
    virtual void onLeaveChannel() {
    }

    /** The RTC engine statistics reported once every two seconds.
     * @param stats RTC engine statistics: RtcStats

    */
    virtual void onLiveStats(const LiveStats& stats) {
        (void)stats;
    }

    /** Reports the network quality once every two seconds.
	* @param uid User ID of the remote user
	* @param txQuality Uplink transmission quality rating of the user in terms of the transmission bitrate, packet loss rate, average RTT (Round-Trip Time), and jitter of the uplink network. @p txQuality is a quality rating helping you understand how well the current uplink network conditions can support the selected VideoEncoderConfiguration. For example, a 1000 Kbps uplink network may be adequate for video frames with a resolution of 640 &times; 480 and a frame rate of 15 fps in the Live Broadcast profile, but may be inadequate for resolutions higher than 1280 &times; 720. See #QUALITY_TYPE.
    * @param rxQaulity Downlink network quality rating of the user in terms of the packet loss rate, average RTT, and jitter of the downlink network. See: #QUALITY_TYPE.
	*/
    virtual void onNetworkQuality(uid_t uid, int txQuality, int rxQuality) {
		(void)uid;
		(void)txQuality;
		(void)rxQuality;
    }

    /** The connection between the SDK and the server is lost.
    */
    virtual void onConnectionLost() {}

    /** The local user is disconnected and the SDK tries to reconnect with the local user.
    */
    virtual void onConnectionInterrupted() {}

    /** The Token has expired.

    This callback function is triggered when the enabled token is invalid or has expired.
    * The application should request for a new token and call \ref ILiveEngine::renewToken "renewToken" to renew the token.
    * @note #ERR_TOKEN_EXPIRED or #ERR_INVALID_TOKEN will be reported by the \ref ILiveEngineEventHandler::onError "onError" callback function.

    */
    virtual void onRequestToken() {
    }
/** The Token privilege will expire in 30 seconds.

 If the Token you specified when calling \ref ILiveEngine::joinChannel "joinChannel" expires, you will become offline. This callback function is triggered 30 seconds before the Token expires to remind the application to renew the Token. You then need to generate a new Token on your server and call \ref agora::rtc::ILiveEngine::renewToken "renewToken" to pass the new Token to the SDK.

@param token The Token that will expire in 30 seconds.

@return

- 0: Success.
- < 0: Failure.
 */
    virtual void onTokenPrivilegeWillExpire(const char *token) {
        (void)token;
    }

};
/** ILiveEngine is the base class for managing a live broadcast. ILiveEngine manages the channel and all actions apart from publishing and subscribing.
 */
class ILiveEngine
{
protected:
    virtual ~ILiveEngine(){}
public:

    /** Initializes the RTC engine.

@param appId RTC engine context

@return

- 0: Success.
- < 0: Failure.
     */
    virtual int initialize(const char *appId) = 0;

    /** Retrieves the SDK version.

     * @param build Build number
     * @return The current SDK version in the sting format. For example, 2.3.0
     */
    virtual const char* getVersion(int* build) = 0;

	/*
	* Gets the IRtcEngine interface.
	*/
    virtual IRtcEngine* getRtcEngine() = 0;


    /**
    * Releases the engine resources.

     @param sync

- true: Releases the engine resources and returns after all resources are destroyed.
- false: Notifies the engine to release the resources and returns without waiting for the resources to be  destroyed
    */
    virtual void release(bool sync=false) = 0;

	/*
	* Sets the event handler for ILiveEngine.
	*/
    virtual int setEventHandler(ILiveEngineEventHandler* eventHandler) = 0;

    /** Allows a useer to join the channel. If a channel has not been created, it will been created automatically.

@note A channel does not accept duplicate uids, such as two users with the same uid. If you set uid as 0, the system will assign a uid.

@param token The Token.
- If you initialized the engine with an available App ID, set token as null.
- If you enabled the Token in Dashboard, set it here.
@param channelId Channel ID
@param config Channel configuration: LiveChannelConfig
@param uid User ID. If uid is set to 0, the system will assign a uid.

@return

- 0: Success.
- < 0: Failure.
    */
    virtual int joinChannel(const char* token, const char* channelId, struct LiveChannelConfig *config, uid_t uid) = 0;

    /** Allows a user to leave the channel.

@return

- 0: Success.
- < 0: Failure.
    */
    virtual int leaveChannel() = 0;

    /** Renews the token.

@param token Token to be renewed.

@return

- 0: Success.
- < 0: Failure.
    */
    virtual int renewToken(const char* token) = 0;

    /** Starts the local video preview.

@return

- 0: Success.
- < 0: Failure.
    */
    virtual int startPreview(view_t view, RENDER_MODE_TYPE type) = 0;

    /** Stops the local video preview.

@return

- 0: Success.
- < 0: Failure.
    */
    virtual int stopPreview() = 0;
};


} //namespace rtc
} // namespace agora

/** Creates the RTC live engine object and returns the pointer.
* @return Returns the pointer of the RTC engine object.
*/
AGORA_API agora::rtc::ILiveEngine* AGORA_CALL createAgoraLiveEngine();

#endif
