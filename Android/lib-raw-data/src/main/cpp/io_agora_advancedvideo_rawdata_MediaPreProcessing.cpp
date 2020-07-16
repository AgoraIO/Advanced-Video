#include <jni.h>
#include <android/log.h>
#include <cstring>
#include "include/IAgoraMediaEngine.h"

#include "include/IAgoraRtcEngine.h"
#include <string.h>
#include "io_agora_advancedvideo_rawdata_MediaPreProcessing.h"
#include "include/VMUtil.h"

#include <map>
#include <thread>
#include <mutex>

using namespace std;

jobject j_o_audio_observer = nullptr;
jobject j_o_video_observer = nullptr;

jmethodID j_mid_record_audio = nullptr;
jmethodID j_mid_playback_audio = nullptr;
jmethodID j_mid_playback_audio_before_mix = nullptr;
jmethodID j_mid_mix_audio = nullptr;
jmethodID j_mid_capture_video = nullptr;
jmethodID j_mid_render_video = nullptr;

void *j_direct_buffer_record_audio = nullptr;
void *j_direct_buffer_playback_audio = nullptr;
void *j_direct_buffer_playback_audio_before_mix = nullptr;
void *j_direct_buffer_mix_audio = nullptr;
void *j_direct_buffer_capture_video = nullptr;

std::mutex mutex_record_audio;
std::mutex mutex_playback_audio;
std::mutex mutex_playback_audio_before_mix;
std::mutex mutex_mix_audio;
std::mutex mutex_capture_video;
std::mutex mutex_render_video;
int playback_audio_before_mix_operating_count = 0;
int render_video_operating_count = 0;

map<int, void *> map_decoded_buffer;

static JavaVM *gJVM = nullptr;

class AgoraVideoFrameObserver : public agora::media::IVideoFrameObserver {

public:
    AgoraVideoFrameObserver() {
    }

    ~AgoraVideoFrameObserver() {
    }

    void getVideoFrame(VideoFrame &videoFrame, _jmethodID *jmethodID, void *_byteBufferObject,
                       unsigned int uid) {
        if (_byteBufferObject == nullptr) {
            return;
        }

        int width = videoFrame.width;
        int height = videoFrame.height;
        size_t widthAndHeight = (size_t) videoFrame.yStride * height;
        size_t length = widthAndHeight * 3 / 2;

        AttachThreadScoped ats(gJVM);
        JNIEnv *env = ats.env();

        memcpy(_byteBufferObject, videoFrame.yBuffer, widthAndHeight);
        memcpy((uint8_t *) _byteBufferObject + widthAndHeight, videoFrame.uBuffer,
               widthAndHeight / 4);
        memcpy((uint8_t *) _byteBufferObject + widthAndHeight * 5 / 4, videoFrame.vBuffer,
               widthAndHeight / 4);

        if(!j_o_video_observer)
            return;

        if (uid == 0) {
            env->CallVoidMethod(j_o_video_observer, jmethodID, videoFrame.type, width, height,
                                length,
                                videoFrame.yStride, videoFrame.uStride,
                                videoFrame.vStride, videoFrame.rotation,
                                videoFrame.renderTimeMs);
        } else {
            env->CallVoidMethod(j_o_video_observer, jmethodID, uid, videoFrame.type, width, height,
                                length,
                                videoFrame.yStride, videoFrame.uStride,
                                videoFrame.vStride, videoFrame.rotation,
                                videoFrame.renderTimeMs);
        }
    }

    void writebackVideoFrame(VideoFrame &videoFrame, void *byteBuffer) {
        if (byteBuffer == nullptr) {
            return;
        }

        int width = videoFrame.width;
        int height = videoFrame.height;
        size_t widthAndHeight = (size_t) videoFrame.yStride * height;

        memcpy(videoFrame.yBuffer, byteBuffer, widthAndHeight);
        memcpy(videoFrame.uBuffer, (uint8_t *) byteBuffer + widthAndHeight, widthAndHeight / 4);
        memcpy(videoFrame.vBuffer, (uint8_t *) byteBuffer + widthAndHeight * 5 / 4,
               widthAndHeight / 4);
    }

public:
    virtual bool onCaptureVideoFrame(VideoFrame &videoFrame) override {
        unique_lock<mutex> lock(mutex_capture_video);
        if (j_mid_capture_video) {
            getVideoFrame(videoFrame, j_mid_capture_video, j_direct_buffer_capture_video, 0);
            writebackVideoFrame(videoFrame, j_direct_buffer_capture_video);
        }
        return true;
    }

    virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame &videoFrame) override {
        {
            unique_lock<mutex> lock(mutex_render_video);
            render_video_operating_count++;
        }
        if (j_mid_render_video) {
            map<int, void *>::iterator it_find;
            it_find = map_decoded_buffer.find(uid);

            if (it_find != map_decoded_buffer.end()) {
                if (it_find->second != nullptr) {
                    getVideoFrame(videoFrame, j_mid_render_video, it_find->second, uid);
                    writebackVideoFrame(videoFrame, it_find->second);
                }
            }
        }
        {
            unique_lock<mutex> lock(mutex_render_video);
            render_video_operating_count--;
        }
        return true;
    }

};


class AgoraAudioFrameObserver : public agora::media::IAudioFrameObserver {

public:
    AgoraAudioFrameObserver() {
        j_o_audio_observer = nullptr;
    }

    ~AgoraAudioFrameObserver() {
    }

    void getAudioFrame(AudioFrame &audioFrame, _jmethodID *jmethodID, void *_byteBufferObject,
                       unsigned int uid) {
        if (_byteBufferObject == nullptr) {
            return;
        }

        AttachThreadScoped ats(gJVM);
        JNIEnv *env = ats.env();
        if (env == nullptr) {
            return;
        }
        int len = audioFrame.samples * audioFrame.bytesPerSample;
        memcpy(_byteBufferObject, audioFrame.buffer, (size_t) len); // * sizeof(int16_t)

        if(!j_o_audio_observer)
            return;

        if (uid == 0) {
            env->CallVoidMethod(j_o_audio_observer, jmethodID, audioFrame.type, audioFrame.samples,
                                audioFrame.bytesPerSample,
                                audioFrame.channels, audioFrame.samplesPerSec,
                                audioFrame.renderTimeMs, len);
        } else {
            env->CallVoidMethod(j_o_audio_observer, jmethodID, uid, audioFrame.type,
                                audioFrame.samples,
                                audioFrame.bytesPerSample,
                                audioFrame.channels, audioFrame.samplesPerSec,
                                audioFrame.renderTimeMs, len);
        }
    }

    void writebackAudioFrame(AudioFrame &audioFrame, void *byteBuffer) {
        if (byteBuffer == nullptr) {
            return;
        }

        int len = audioFrame.samples * audioFrame.bytesPerSample;
        memcpy(audioFrame.buffer, byteBuffer, (size_t) len);
    }

public:
    virtual bool onRecordAudioFrame(AudioFrame &audioFrame) override {
        unique_lock<mutex> lock(mutex_record_audio);
        if (j_mid_record_audio) {
            getAudioFrame(audioFrame, j_mid_record_audio, j_direct_buffer_record_audio, 0);
            writebackAudioFrame(audioFrame, j_direct_buffer_record_audio);
        }
        return true;
    }

    virtual bool onPlaybackAudioFrame(AudioFrame &audioFrame) override {
        unique_lock<mutex> lock(mutex_playback_audio);
        if (j_mid_playback_audio) {
            getAudioFrame(audioFrame, j_mid_playback_audio, j_direct_buffer_playback_audio, 0);
            writebackAudioFrame(audioFrame, j_direct_buffer_playback_audio);
        }
        return true;
    }

    virtual bool
    onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame &audioFrame) override {
        {
            unique_lock<mutex> lock(mutex_playback_audio_before_mix);
            playback_audio_before_mix_operating_count++;
        }
        if (j_mid_playback_audio_before_mix) {
            getAudioFrame(audioFrame, j_mid_playback_audio_before_mix,
                          j_direct_buffer_playback_audio_before_mix,
                          uid);
            writebackAudioFrame(audioFrame, j_direct_buffer_playback_audio_before_mix);
        }
        {
            unique_lock<mutex> lock(mutex_playback_audio_before_mix);
            playback_audio_before_mix_operating_count--;
        }
        return true;
    }

    virtual bool onMixedAudioFrame(AudioFrame &audioFrame) override {
        unique_lock<mutex> lock(mutex_mix_audio);
        if (j_mid_mix_audio) {
            getAudioFrame(audioFrame, j_mid_mix_audio, j_direct_buffer_mix_audio, 0);
            writebackAudioFrame(audioFrame, j_direct_buffer_mix_audio);
        }
        return true;
    }
};


static AgoraAudioFrameObserver s_audioFrameObserver;
static AgoraVideoFrameObserver s_videoFrameObserver;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_registerAudioFrameObserver
        (JNIEnv *env, jclass, jobject audio_frame_observer, jlong rtc_engine_native_handle) {

    LOGI("registerAudioFrameObserver");
    if (!rtc_engine_native_handle) return;

    env->GetJavaVM(&gJVM);

    agora::rtc::IRtcEngine *pEngine = (agora::rtc::IRtcEngine *) rtc_engine_native_handle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(pEngine, agora::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine) {
        int ret = mediaEngine->registerAudioFrameObserver(&s_audioFrameObserver);
        if (!ret) {
            LOGI("registerAudioFrameObserver success");
        }
    }

    if (!j_o_audio_observer) {
        j_o_audio_observer = env->NewGlobalRef(audio_frame_observer);
        jclass j_c_audio_observer = env->GetObjectClass(j_o_audio_observer);
        j_mid_record_audio = env->GetMethodID(j_c_audio_observer, "onRecordAudioFrame",
                                              "(IIIIIJI)V");
        j_mid_playback_audio = env->GetMethodID(j_c_audio_observer, "onPlaybackAudioFrame",
                                                "(IIIIIJI)V");
        j_mid_playback_audio_before_mix = env->GetMethodID(j_c_audio_observer,
                                                           "onPlaybackAudioFrameBeforeMixing",
                                                           "(IIIIIIJI)V");
        j_mid_mix_audio = env->GetMethodID(j_c_audio_observer, "onMixedAudioFrame", "(IIIIIJI)V");
    }
}

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_unRegisterAudioFrameObserver(JNIEnv *env,
                                                                                    jclass clazz,
                                                                                    jlong rtc_engine_native_handle) {
    LOGI("unRegisterAudioFrameObserver");
    if (!rtc_engine_native_handle) return;

    env->GetJavaVM(&gJVM);

    agora::rtc::IRtcEngine *pEngine = (agora::rtc::IRtcEngine *) rtc_engine_native_handle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(pEngine, agora::AGORA_IID_MEDIA_ENGINE);

    if (mediaEngine) {
        mediaEngine->registerAudioFrameObserver(nullptr);
    }
}

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_registerVideoFrameObserver(JNIEnv *env,
                                                                                  jclass clazz,
                                                                                  jobject observer,
                                                                                  jlong rtc_engine_native_handle) {
    LOGI("registerVideoFrameObserver");
    if (!rtc_engine_native_handle) return;

    env->GetJavaVM(&gJVM);

    agora::rtc::IRtcEngine *pEngine = (agora::rtc::IRtcEngine *) rtc_engine_native_handle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(pEngine, agora::AGORA_IID_MEDIA_ENGINE);

    if (mediaEngine) {
        int ret = mediaEngine->registerVideoFrameObserver(&s_videoFrameObserver);
        if (!ret) {
            LOGI("registerVideoFrameObserver success");
        }
    }
    if (!j_o_video_observer) {
        j_o_video_observer = env->NewGlobalRef(observer);
        jclass j_c_video_observer = env->GetObjectClass(j_o_video_observer);
        j_mid_capture_video = env->GetMethodID(j_c_video_observer, "onCaptureVideoFrame",
                                               "(IIIIIIIIJ)V");
        j_mid_render_video = env->GetMethodID(j_c_video_observer, "onRenderVideoFrame",
                                              "(IIIIIIIIIJ)V");
    }
}

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_unRegisterVideoFrameObserver(JNIEnv *env,
                                                                                    jclass clazz,
                                                                                    jlong rtc_engine_native_handle) {
    LOGI("unRegisterVideoFrameObserver");
    if (!rtc_engine_native_handle) return;

    env->GetJavaVM(&gJVM);

    agora::rtc::IRtcEngine *pEngine = (agora::rtc::IRtcEngine *) rtc_engine_native_handle;
    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(pEngine, agora::AGORA_IID_MEDIA_ENGINE);

    if (mediaEngine) {
        mediaEngine->registerVideoFrameObserver(nullptr);
    }
}

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setVideoCaptureByteBuffer
        (JNIEnv *env, jclass, jobject bytebuffer) {
    j_direct_buffer_capture_video = env->GetDirectBufferAddress(bytebuffer);
}

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setAudioRecordByteBuffer
        (JNIEnv *env, jclass, jobject bytebuffer) {
    j_direct_buffer_record_audio = env->GetDirectBufferAddress(bytebuffer);
}

JNIEXPORT void JNICALL Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setAudioPlayByteBuffer
        (JNIEnv *env, jclass, jobject bytebuffer) {
    j_direct_buffer_playback_audio = env->GetDirectBufferAddress(bytebuffer);
}

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setBeforeAudioMixByteBuffer
        (JNIEnv *env, jclass, jobject bytebuffer) {
    j_direct_buffer_playback_audio_before_mix = env->GetDirectBufferAddress(bytebuffer);
}

JNIEXPORT void JNICALL Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setAudioMixByteBuffer
        (JNIEnv *env, jclass, jobject bytebuffer) {
    j_direct_buffer_mix_audio = env->GetDirectBufferAddress(bytebuffer);
}

JNIEXPORT void JNICALL
Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_setVideoDecodeByteBuffer
        (JNIEnv *env, jclass, jint uid, jobject byteBuffer) {
    if (byteBuffer == nullptr) {
        map_decoded_buffer.erase(uid);
    } else {
        void *_javaDirectDecodeBuffer = env->GetDirectBufferAddress(byteBuffer);
        map_decoded_buffer.insert(make_pair(uid, _javaDirectDecodeBuffer));
        LOGI("setVideoDecodeByteBuffer uid: %u, _javaDirectDecodeBuffer: %p", uid,
             _javaDirectDecodeBuffer);
    }
}

JNIEXPORT void JNICALL Java_io_agora_advancedvideo_rawdata_MediaPreProcessing_release
        (JNIEnv *env, jclass) {

    if (j_o_audio_observer != nullptr) {
        {
            unique_lock<mutex> lock(mutex_record_audio);
            j_mid_record_audio = nullptr;
        }
        {
            unique_lock<mutex> lock(mutex_playback_audio);
            j_mid_playback_audio = nullptr;
        }
        {
            unique_lock<mutex> lock(mutex_mix_audio);
            j_mid_mix_audio = nullptr;
        }

        int retryCount = 5;
        bool need_wait = false;
        do {
            if (need_wait) {
                this_thread::sleep_for(chrono::milliseconds(30));
            }
            unique_lock<mutex> lock(mutex_playback_audio_before_mix);
            if (playback_audio_before_mix_operating_count > 0 && retryCount > 0) {
                need_wait = true;
            } else {
                j_mid_playback_audio_before_mix = nullptr;
                break;
            }
            retryCount--;
        } while (true);

        env->DeleteGlobalRef(j_o_audio_observer);
        j_o_audio_observer = nullptr;
    }

    j_direct_buffer_record_audio = nullptr;
    j_direct_buffer_playback_audio = nullptr;
    j_direct_buffer_playback_audio_before_mix = nullptr;
    j_direct_buffer_mix_audio = nullptr;

    if (j_o_video_observer != nullptr) {
        {
            unique_lock<mutex> lock(mutex_capture_video);
            j_mid_capture_video = nullptr;
        }

        int retryCount = 5;
        bool need_wait = false;
        do {
            if (need_wait) {
                this_thread::sleep_for(chrono::milliseconds(50));
            }
            unique_lock<mutex> lock(mutex_render_video);
            if (render_video_operating_count > 0 && retryCount > 0) {
                need_wait = true;
            } else {
                j_mid_render_video = nullptr;
                break;
            }
            retryCount--;
        } while (true);

        env->DeleteGlobalRef(j_o_video_observer);
        j_o_video_observer = nullptr;
    }

    j_direct_buffer_capture_video = nullptr;
    map_decoded_buffer.clear();
}

#ifdef __cplusplus
}
#endif
