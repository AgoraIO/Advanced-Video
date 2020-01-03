# Android Samples for Advanced Applications of Agora Video SDK APIs

This demo will give examples of how to use Agora Video SDK combined with Android framework APIs, as well as potential scenarios that applies audio & video modules.

## Configuration

## Modules

### Customized Video Sources and Video Rendering

Agora Video SDK provides MediaIO interfaces which can be used to define videos from sources other than captured by Agora SDK itself. This facilitates developers when they have their own video sources available (from other devices, for example), but it needs controlling of the whole life cycles of video sources.

This project exemplifies two kinds of external video sources (compared to videos captured by Agora SDK) and how to push them into the pipeline of Agora SDK APIs. Besides, it shows the way that you may choose to render videos.

Refer to [this page](https://docs.agora.io/en/Video/custom_video_android?platform=Android) for the explanation of how to use MediaIO sources and rendering interfaces.

The video frames are usually in the forms of I420, NV21 or texture. Here we take textures as an example of how to maintain the OpenGL context appropriately for both video sources and rendering. 

There is another reference project [grafika](https://github.com/google/grafika) from Google where this project's idea of maintaining OpenGLES contexts comes from.

To held distinguish between MediaIO video sources and the videos from different sources, we define the video frames from screen sharing or local videos as inputs. 

#### Screen Share

Screen Sharing is one of the features introduced in API 21, so the minimum level should be equal to or higher than that.

In order to keep the screen sharing running properly even in the Launcher or other Apps, and to insure it is not killed by the system unexpectedly, the media projection is created and started in another foreground Service. Do not forget to show a notification, especially when the target is Android Oreo or later.

#### Local Videos

To keep the code simple, the demo requires the users to copy an .mp4 video file to the app-owned video folder. Developers can implement in any way of finding a video file.

Video frame data (stored in ByteBuffer) is extracted from video tracks of packaged media files (.mp4 files) using [MediaExtractor](https://developer.android.com/reference/android/media/MediaExtractor), then the frame data is sent to a decoder [MediaCodec](https://developer.android.com/reference/android/media/MediaCodec) at the pace of the frame's presentation time.

To keep consistent, results of the decoder are received by an output Surface which is created from a texture, then we can update and draw to any SurfaceView or TextureView using that texture directly (usually as local preview).

#### Augmented Reality
Not implemented yet.


#### Switching between different video inputs

We have provided a sample of switching between different inputs of external videos. Special care about the differences between "video inputs" and "video sources", where "sources" refers to Agora RTC MediaIO interface "IVideoSource" which means the transmission entry of Agora API, while "inputs" are defined as where the video frames are generated (mp4 or screens, etc.). 

Only one video input instance is created in charge of generating video frames, and the frames are obtained from one input at one time but can be switched to another input at any time.

To keep consistent, video frames are sent in a Service. So it continues to send video even if the app goes to background until stopped. An OpenGL worker thread is managed by the Service and the thread is used throughout the application to avoid OpenGL problems. All video inputs will have the same output Surface created in this thread, and frame data is updated from the Surface periodically. The intervals are obtained from the specific video inputs.

### Audio & Video Customized Packet Encryption Using Raw-data Plugin Interface

Go to [Raw Audio Data](https://docs.agora.io/en/Video/raw_data_audio_android?platform=Android) and [Raw Video Data](https://docs.agora.io/en/Video/raw_data_video_android?platform=Android) for the descriptions of using audio & video raw data. 

Packet encryption is one of the specific applications of audio & video raw data interface. Developers can choose to perform any transformation of raw data on the sending side, and do the inverse to recover data on the receiving side.

Note that this project only provides C++ raw data interface. There are also Java layer interfaces before, but the video interfaces have been removed from earlier SDK releases for performance concerns. The Java audio interfaces are still usable, but C++ interfaces are recommended for the same reason.

The App's compiled library name should start with a pre-defined prefix like **"libapm-xxx.so"**, RTC engine will search in the library directory .so files of names of this form as Agora plugins and load the libraries during the plugin manager initialization phase.

Plugins use header files which are in the SDK downloaded zip files but not a part of JCenter dependency. The headers are submitted along with the project right now, but if Agora SDK dependency needs to be upgraded, the headers might as well. Once necessary, we suggest that developers download the latest SDK version and replace those headers. Or rather, download and copy the whole SDK instead of introducing from JCenter.  

## Licence
The MIT License (MIT).
