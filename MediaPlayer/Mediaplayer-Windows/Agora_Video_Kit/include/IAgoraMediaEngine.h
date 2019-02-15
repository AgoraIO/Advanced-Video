#ifndef AGORA_MEDIA_ENGINE_H
#define AGORA_MEDIA_ENGINE_H
#if defined _WIN32 || defined __CYGWIN__
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

namespace agora {
namespace media {
/** @deprecated
 Type of audio device.
 */
enum MEDIA_SOURCE_TYPE {
  /** Audio playback device.
   */
  AUDIO_PLAYOUT_SOURCE = 0,
  /** Microphone.
   */
  AUDIO_RECORDING_SOURCE = 1,
};

class IAudioFrameObserver {
 public:
  enum AUDIO_FRAME_TYPE {
    FRAME_TYPE_PCM16 = 0,  // PCM 16bit little endian
  };
  /** Definition of AudioFrame */
  struct AudioFrame {
    AUDIO_FRAME_TYPE type;
    /** Number of samples in the audio frame: samples = (int)samplesPerCall = (int)(sampleRate &times; sampleInterval)
     */
    int samples;  //number of samples in this frame
    /** Number of bytes per audio sample. For example, each PCM audio sample usually takes up 16 bits (2 bytes).
     */
    int bytesPerSample;  //number of bytes per sample: 2 for PCM16
    /** Number of audio channels.
     - 1: Mono
     - 2: Stereo (the data is interleaved)
     */
    int channels;  //number of channels (data are interleaved if stereo)
    /** Sampling rate per second for the audio frame: 8000, 16000, 32000, 44100, or 48000 bps.
     */
    int samplesPerSec;  //sampling rate
    /** Audio frame data buffer. The valid data length is: samples &times; channels &times; bytesPerSample
     */
    void* buffer;  //data buffer
    /** Timestamp to render the audio stream.
     */
    int64_t renderTimeMs;
    int avsync_type;
  };

 public:
  /** Callback occuring once every 10 ms. Retrieves the recorded audio frame.

   @param audioFrame Pointer to AudioFrame.
   @return
   - true: Valid buffer in AudioFrame, and the recorded audio frame is sent out.
   - false: Invalid buffer in AudioFrame, and the recorded audio frame ise discarded.
   */
  virtual bool onRecordAudioFrame(AudioFrame& audioFrame) = 0;
  /** Callback occuring once every 10 ms. Retrieves the audio playback frame.

   This callback returns after \ref agora::rtc::RtcEngineParameters::playEffect "playEffect" is successfully called.

   @param audioFrame Pointer to AudioFrame.
   @return
   - true: Valid buffer in AudioFrame, and the audio playback frame is sent out.
   - false: Invalid buffer in AudioFrame, and the audio playback frame is discarded.
   */
  virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame) = 0;
  /** Callback occuring every 10 ms. Retrieves the mixed recorded and playback audio frame.

   @note This method only returns the single-channel data.

   @param audioFrame Pointer to AudioFrame.
   @return
   - true: Valid buffer in AudioFrame and the mixed recorded and playback audio frame is sent out.
   - false: Invalid buffer in AudioFrame and the mixed recorded and playback audio frame is discarded.
   */
  virtual bool onMixedAudioFrame(AudioFrame& audioFrame) = 0;
  /** Callback occuring once every 10 ms. Retrieves the audio frame of a specified user before mixing.

  @param uid The user ID
  @param audioFrame Pointer to AudioFrame.
  @return
  - true: Valid buffer in AudioFrame, and the mixed recorded and playback audio frame is sent out.
  - false: Invalid buffer in AudioFrame, and the mixed recorded and playback audio frame is discarded.
  */
  virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid,
      AudioFrame& audioFrame) = 0;
};

class IVideoFrameObserver {
 public:
  enum VIDEO_FRAME_TYPE {
    FRAME_TYPE_YUV420 = 0,  // YUV 420 format
  };
  /** Video frame information. The video data format is YUV420. The buffer provides a pointer to a pointer. The interface cannot modify the pointer of the buffer, but can modify the content of the buffer only.
   */
  struct VideoFrame {
    VIDEO_FRAME_TYPE type;
    /** Video pixel width.
     */
    int width;  //width of video frame
    /** Video pixel height.
     */
    int height;  //height of video frame
    /** Line span of the Y buffer within the YUV data.
     */
    int yStride;  //stride of Y data buffer
    /** Line span of the U buffer within the YUV data.
     */
    int uStride;  //stride of U data buffer
    /** Line span of the V buffer within the YUV data.
     */
    int vStride;  //stride of V data buffer
    /** Pointer to the Y buffer pointer within the YUV data.
     */
    void* yBuffer;  //Y data buffer
    /** Pointer to the U buffer pointer within the YUV data.
     */
    void* uBuffer;  //U data buffer
    /** Pointer to the V buffer pointer within the YUV data.
     */
    void* vBuffer;  //V data buffer
    /** Set the rotation of this frame before rendering the video. Supports 0, 90, 180, 270 degrees clockwise.
     */
    int rotation; // rotation of this frame (0, 90, 180, 270)
    /** Timestamp (ms) for the video stream render. Use this timestamp to synchronize the video stream render while rendering the video streams.

     @note This timestamp is for rendering the video stream, and not for capturing the video stream.
     */
    int64_t renderTimeMs;
    int avsync_type;
  };

 public:
  /** Retrieves the camera captured image.

   @param videoFrame VideoFrame
   */
  virtual bool onCaptureVideoFrame(VideoFrame& videoFrame) = 0;
  /** Processes the received image of the specified user (post-processing).

   @param uid User ID of the specified user sending the image.
   @param videoFrame VideoFrame
   */
  virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame) = 0;
};

class IVideoFrame {
 public:
  enum PLANE_TYPE { Y_PLANE = 0, U_PLANE = 1, V_PLANE = 2, NUM_OF_PLANES = 3 };
  enum VIDEO_TYPE {
    VIDEO_TYPE_UNKNOWN = 0,
    VIDEO_TYPE_I420 = 1,
    VIDEO_TYPE_IYUV = 2,
    VIDEO_TYPE_RGB24 = 3,
    VIDEO_TYPE_ABGR = 4,
    VIDEO_TYPE_ARGB = 5,
    VIDEO_TYPE_ARGB4444 = 6,
    VIDEO_TYPE_RGB565 = 7,
    VIDEO_TYPE_ARGB1555 = 8,
    VIDEO_TYPE_YUY2 = 9,
    VIDEO_TYPE_YV12 = 10,
    VIDEO_TYPE_UYVY = 11,
    VIDEO_TYPE_MJPG = 12,
    VIDEO_TYPE_NV21 = 13,
    VIDEO_TYPE_NV12 = 14,
    VIDEO_TYPE_BGRA = 15,
    VIDEO_TYPE_RGBA = 16,
  };
  virtual void release() = 0;
  virtual const unsigned char* buffer(PLANE_TYPE type) const = 0;

  /** Copies the frame.

   If the required size is larger than the allocated size, new buffers of the adequate size will be allocated.

   @param dest_frame Address of the returned destination frame. See IVideoFrame.
   @return
   - 0: Success.
   - -1: Failure.
   */
  virtual int copyFrame(IVideoFrame** dest_frame) const = 0;
  /** Converts the frame.

   @note The source and destination frames have equal heights.

   @param dst_video_type Type of the output video.
   @param dst_sample_size Required only for the parsing of M-JPEG.
   @param dst_frame Pointer to a destination frame. See IVideoFrame.
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int convertFrame(VIDEO_TYPE dst_video_type, int dst_sample_size,
                           unsigned char* dst_frame) const = 0;
  /** Retrieves the specified component in the YUV space.

   @param type Component type: #PLANE_TYPE
   */
  virtual int allocated_size(PLANE_TYPE type) const = 0;
  /** Retrieves the stride of the specified component in the YUV space.

   @param type Component type: #PLANE_TYPE
   */
  virtual int stride(PLANE_TYPE type) const = 0;
  /** Retrieves the width of the frame.
   */
  virtual int width() const = 0;
  /** Retrieves the height of the frame.
   */
  virtual int height() const = 0;
  /** Retrieves the timestamp (90 ms) of the frame.
   */
  virtual unsigned int timestamp() const = 0;
  /** Retrieves the render time (ms).
   */
  virtual int64_t render_time_ms() const = 0;
  /** Checks if a plane is of zero size.

   @return
   - true: The plane is of zero size.
   - false: The plane is not of zero size.
   */
  virtual bool IsZeroSize() const = 0;
};
/** @deprecated
 */
class IExternalVideoRenderCallback {
 public:
  virtual void onViewSizeChanged(int width, int height) = 0;
  virtual void onViewDestroyed() = 0;
};
/** @deprecated
 */
struct ExternalVideoRenerContext {
  IExternalVideoRenderCallback* renderCallback;
  /** Video display window.
   */
  void* view;
  /** Video display mode: \ref agora::rtc::RENDER_MODE_TYPE "RENDER_MODE_TYPE" */
  int renderMode;
  /** The image layer location.

   - 0: (Default) The image is at the bottom of the stack
   - 100: The image is at the top of the stack.

   @note If the value is set to below 0 or above 100, the #ERR_INVALID_ARGUMENT error occurs.
   */
  int zOrder;
  /** Video layout distance from the left.
   */
  float left;
  /** Video layout distance from the top.
   */
  float top;
  /** Video layout distance from the right.
   */
  float right;
  /** Video layout distance from the bottom.
   */
  float bottom;
};

class IExternalVideoRender {
 public:
  virtual void release() = 0;
  virtual int initialize() = 0;
  virtual int deliverFrame(const IVideoFrame& videoFrame, int rotation,
                           bool mirrored) = 0;
};

class IExternalVideoRenderFactory {
 public:
  virtual IExternalVideoRender* createRenderInstance(
      const ExternalVideoRenerContext& context) = 0;
};

/** The external video frame.
 */
struct ExternalVideoFrame
{
    /** The video buffer type.
     */
    enum VIDEO_BUFFER_TYPE
    {
        /** 1: The video buffer in the format of raw data.
         */
        VIDEO_BUFFER_RAW_DATA = 1,
    };

    /** The video pixel format.
     */
    enum VIDEO_PIXEL_FORMAT
    {
        /** 0: The video pixel format is unknown.
         */
        VIDEO_PIXEL_UNKNOWN = 0,
        /** 1: The video pixel format is I420.
         */
        VIDEO_PIXEL_I420 = 1,
        /** 2: The video pixel format is BGRA.
         */
        VIDEO_PIXEL_BGRA = 2,
        /** 8: The video pixel format is NV12.
         */
        VIDEO_PIXEL_NV12 = 8,
    };

    /** The buffer type. See #VIDEO_BUFFER_TYPE
     */
    VIDEO_BUFFER_TYPE type;
    /** The pixel format. See #VIDEO_PIXEL_FORMAT
     */
    VIDEO_PIXEL_FORMAT format;
    /** The video buffer.
     */
    void* buffer;
    /** Line spacing of the incoming video frame, which must be in pixels instead of bytes. For textures, it is the width of the texture.
     */
    int stride;
    /** Height of the incoming video frame.
     */
    int height;
    /** [Raw data related parameter] The number of pixels trimmed from the left. The default value is 0.
     */
    int cropLeft;
    /** [Raw data related parameter] The number of pixels trimmed from the top. The default value is 0.
     */
    int cropTop;
    /** [Raw data related parameter] The number of pixels trimmed from the right. The default value is 0.
     */
    int cropRight;
    /** [Raw data related parameter] The number of pixels trimmed from the bottom. The default value is 0.
     */
    int cropBottom;
    /** [Raw data related parameter] The clockwise rotation of the video frame. You can set the rotation angle as 0, 90, 180, or 270. The default value is 0.
     */
    int rotation;
    /** Timestamp of the incoming video frame (ms). An incorrect timestamp results in frame loss or unsynchronized audio and video.
     */
    long long timestamp;
};

class IMediaEngine {
 public:
  virtual void release() = 0;
  /** Registers an audio frame observer object.

   This method is used to register an audio frame observer object (register a callback). This method is required to register callbacks when the engine is required to provide an \ref IAudioFrameObserver::onRecordAudioFrame "onRecordAudioFrame" or \ref IAudioFrameObserver::onPlaybackAudioFrame "onPlaybackAudioFrame" callback.

   @param observer Audio frame observer object instance. If NULL is passed in, the registration is canceled.
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int registerAudioFrameObserver(IAudioFrameObserver* observer) = 0;
  /** Registers a video frame observer object.

   This method is required to register callbacks when the engine is required to provide an \ref IVideoFrameObserver::onCaptureVideoFrame "onCaptureVideoFrame" or \ref IVideoFrameObserver::onRenderVideoFrame "onRenderVideoFrame" callback.

   @param observer Video frame observer object instance. If NULL is passed in, the registration is canceled.
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int registerVideoFrameObserver(IVideoFrameObserver* observer) = 0;
  /** @deprecated
   */
  virtual int registerVideoRenderFactory(IExternalVideoRenderFactory* factory) = 0;
  /** @deprecated Use \ref agora::media::IMediaEngine::pushAudioFrame(IAudioFrameObserver::AudioFrame* frame) "pushAudioFrame(IAudioFrameObserver::AudioFrame* frame)" instead.
   
   Pushes the external audio frame.

   @param type Type of audio capture device: #MEDIA_SOURCE_TYPE.
   @param frame Audio frame pointer: \ref IAudioFrameObserver::AudioFrame "AudioFrame".
   @param wrap Whether to use the placeholder. Agora recommends setting the default value.
   - true: Use.
   - false: (Default) Not use.

   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int pushAudioFrame(MEDIA_SOURCE_TYPE type,
                             IAudioFrameObserver::AudioFrame* frame,
                             bool wrap) = 0;
  /** Pushes the external audio frame.
   
   @param frame Pointer to the audio frame: \ref IAudioFrameObserver::AudioFrame "AudioFrame".
    
   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int pushAudioFrame(IAudioFrameObserver::AudioFrame* frame) = 0;
  /** Pulls the external audio frame for external audio playback.

   The application pulls the mixed audio frame from the audio engine for external audio playback. Agora recommends using *pullAudioFrame* over \ref agora::media::IAudioFrameObserver::onPlaybackAudioFrame "onPlaybackAudioFrame".
   
   @note This method overrides the \ref agora::media::IMediaEngine::registerAudioFrameObserver "registerAudioFrameObserver" method. After calling *pullAudioFrame*, the application will not retrieve any audio frame from the \ref agora::media::IAudioFrameObserver::onPlaybackAudioFrame "onPlaybackAudioFrame" callback.

   Before calling this method:
   1. Call \ref agora::rtc::RtcEngineParameters::setExternalAudioSink "setExternalAudioSink" to inform the audio engine that the application will call *pullAudioFrame* to pull audio frames from the sink.
   2. Call \ref agora::rtc::RtcEngineParameters::setPlaybackAudioFrameParameters "setPlaybackAudioFrameParameters" to set the parameters of the audio frame to be pulled, including the audio sampling rate and number of the audio channel.

   Differences between \ref agora::media::IAudioFrameObserver::onPlaybackAudioFrame "onPlaybackAudioFrame" and *pullAudioFrame*:
   - \ref agora::media::IAudioFrameObserver::onPlaybackAudioFrame "onPlaybackAudioFrame" sends audio frames to the application once every 10 ms. Any delay in processing the audio frames may result in audio jitter.
   - With *pullAudioFrame*, the application pulls the audio frame. The SDK specifies the number of audio samples for playback by the @p sample parameter in \ref agora::media::IAudioFrameObserver::AudioFrame "AudioFrame", adjusts the frame buffer, and allows for a delay in processing the audio frames by the application. This method avoids problems caused by jitter in the external audio playback, such as an unsynchronized audio playback.

   @param frame Pointer to the audio frame. See: \ref IAudioFrameObserver::AudioFrame "AudioFrame".

   @return
   - 0: Success.
   - < 0: Failure.
   */
  virtual int pullAudioFrame(IAudioFrameObserver::AudioFrame* frame) = 0;
    /** Configures the external video source.

    @param enable Sets whether to use the external video source:
    - true: Use the external video source.
    - false: (Default) Do not use the external video source.

    @param useTexture Sets whether to use texture as an input:
    - true: Use texture as an input.
    - false: (Default) Do not use texture as an input.

    @return
    - 0: Success.
    - < 0: Failure.
    */
    virtual int setExternalVideoSource(bool enable, bool useTexture) = 0;
    /** Pushes the video frame using the \ref ExternalVideoFrame "ExternalVideoFrame" and passes the video frame to the Agora SDK.

    @param frame Video frame to be pushed. See \ref ExternalVideoFrame "ExternalVideoFrame".

    @note This method supports pushing textured video frames only in the live-broadcast profile, not in the communication profile.

    @return
    - 0: Success.
    - < 0: Failure.
    */
    virtual int pushVideoFrame(ExternalVideoFrame *frame) = 0;
};

}  // namespace media

}  // namespace agora

#endif  // AGORA_MEDIA_ENGINE_H
