#include <jni.h>
#include <android/log.h>
#include <cstring>
#include "include/IAgoraMediaEngine.h"

#include "include/IAgoraRtcEngine.h"
#include <string.h>
#include "media_preprocessing_plugin_jni.h"
#include "include/VMUtil.h"

#include <map>

using namespace std;

jobject gCallBack = nullptr;
jclass gCallbackClass = nullptr;
jmethodID recordAudioMethodId = nullptr;
jmethodID playbackAudioMethodId = nullptr;
jmethodID playBeforeMixAudioMethodId = nullptr;
jmethodID mixAudioMethodId = nullptr;
jmethodID captureVideoMethodId = nullptr;
jmethodID renderVideoMethodId = nullptr;
void *_javaDirectPlayBufferCapture = nullptr;
void *_javaDirectPlayBufferRecordAudio = nullptr;
void *_javaDirectPlayBufferPlayAudio = nullptr;
void *_javaDirectPlayBufferBeforeMixAudio = nullptr;
void *_javaDirectPlayBufferMixAudio = nullptr;
map<int, void *> decodeByfferMap;

static JavaVM *gJVM = nullptr;

class AgoraVideoFrameObserver : public agora::media::IVideoFrameObserver {


public:
    AgoraVideoFrameObserver() {

    }

    ~AgoraVideoFrameObserver() {

    }

    void
    getVideoFrame(VideoFrame &videoFrame, _jmethodID *jmethodID, void *_byteBufferObject, int uid) {

        if (_byteBufferObject) {
            int width = videoFrame.width;
            int height = videoFrame.height;
            int widthAndheight = width * height;
            int length = widthAndheight * 3 / 2;

            AttachThreadScoped ats(gJVM);
            JNIEnv *env = ats.env();

            memcpy(_byteBufferObject, videoFrame.yBuffer, widthAndheight);
            memcpy(_byteBufferObject + widthAndheight, videoFrame.uBuffer, widthAndheight / 4);
            memcpy(_byteBufferObject + widthAndheight * 5 / 4, videoFrame.vBuffer,
                   widthAndheight / 4);

            env->CallVoidMethod(gCallBack, jmethodID, uid, videoFrame.type, width, height, length,
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
        int widthAndHeight = width * height;

        memcpy(videoFrame.yBuffer, byteBuffer, widthAndHeight);
        memcpy(videoFrame.uBuffer, byteBuffer + widthAndHeight, widthAndHeight/4);
        memcpy(videoFrame.vBuffer, byteBuffer + widthAndHeight * 5 / 4, widthAndHeight/4);
    }

public:
    virtual bool onCaptureVideoFrame(VideoFrame &videoFrame) override {
        getVideoFrame(videoFrame, captureVideoMethodId, _javaDirectPlayBufferCapture, 0);
        writebackVideoFrame(videoFrame, _javaDirectPlayBufferCapture);
        return true;
    }

    virtual bool onRenderVideoFrame(unsigned int uid, VideoFrame &videoFrame) override {


        map<int, void *>::iterator it_find;
        it_find = decodeByfferMap.find(uid);

        if (it_find != decodeByfferMap.end()) {
            if (it_find->second != nullptr) {
                getVideoFrame(videoFrame, renderVideoMethodId, it_find->second, uid);
                writebackVideoFrame(videoFrame, it_find->second);
            }

        }

        return true;
    }


};


class AgoraAudioFrameObserver : public agora::media::IAudioFrameObserver {

public:
    AgoraAudioFrameObserver() {
        gCallBack = nullptr;
    }

    ~AgoraAudioFrameObserver() {
    }

    void getAudioFrame(AudioFrame &audioFrame, _jmethodID *jmethodID, void *_byteBufferObject) {
        if (_byteBufferObject) {
            AttachThreadScoped ats(gJVM);
            JNIEnv *env = ats.env();
            if (env == nullptr) {
                return;
            }
            int len = audioFrame.samples * audioFrame.bytesPerSample;
            memcpy(_byteBufferObject, audioFrame.buffer, len);//* sizeof(int16_t)
            env->CallVoidMethod(gCallBack, jmethodID, audioFrame.type, audioFrame.samples,
                                audioFrame.bytesPerSample,
                                audioFrame.channels, audioFrame.samplesPerSec,
                                audioFrame.renderTimeMs, len);
        }

    }

    void writebackAudioFrame(AudioFrame &audioFrame, void *byteBuffer) {
        int len = audioFrame.samples * audioFrame.bytesPerSample;
        memcpy(audioFrame.buffer, byteBuffer, len);
    }

public:
    virtual bool onRecordAudioFrame(AudioFrame &audioFrame) override {
        getAudioFrame(audioFrame, recordAudioMethodId, _javaDirectPlayBufferRecordAudio);
        writebackAudioFrame(audioFrame, _javaDirectPlayBufferRecordAudio);
        return true;
    }

    virtual bool onPlaybackAudioFrame(AudioFrame &audioFrame) override {
        getAudioFrame(audioFrame, playbackAudioMethodId, _javaDirectPlayBufferPlayAudio);
        writebackAudioFrame(audioFrame, _javaDirectPlayBufferPlayAudio);
        return true;
    }

    virtual bool
    onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame &audioFrame) override {
        getAudioFrame(audioFrame, playBeforeMixAudioMethodId, _javaDirectPlayBufferBeforeMixAudio);
        writebackAudioFrame(audioFrame, _javaDirectPlayBufferBeforeMixAudio);
        return true;
    }

    virtual bool onMixedAudioFrame(AudioFrame &audioFrame) override {
        getAudioFrame(audioFrame, mixAudioMethodId, _javaDirectPlayBufferMixAudio);
        writebackAudioFrame(audioFrame, _javaDirectPlayBufferMixAudio);
        return true;
    }
};


static AgoraAudioFrameObserver s_audioFrameObserver;

static AgoraVideoFrameObserver s_videoFrameObserver;
static agora::rtc::IRtcEngine *rtcEngine = nullptr;

#ifdef __cplusplus
extern "C" {
#endif

int __attribute__((visibility("default")))
loadAgoraRtcEnginePlugin(agora::rtc::IRtcEngine *engine) {
    __android_log_print(ANDROID_LOG_DEBUG, "plugin", "plugin loadAgoraRtcEnginePlugin");
    rtcEngine = engine;
    return 0;
}

void __attribute__((visibility("default")))
unloadAgoraRtcEnginePlugin(agora::rtc::IRtcEngine *engine) {
    __android_log_print(ANDROID_LOG_DEBUG, "plugin", "plugin unloadAgoraRtcEnginePlugin");

    rtcEngine = nullptr;

}


JNIEXPORT void JNICALL
Java_io_agora_rtc_plugin_rawdata_MediaPreProcessing_setCallback(JNIEnv *env, jobject obj,
                                                                jobject callback) {
    if (!rtcEngine) return;

    env->GetJavaVM(&gJVM);

    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtcEngine, agora::INTERFACE_ID_TYPE::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine) {

        mediaEngine->registerVideoFrameObserver(&s_videoFrameObserver);
        mediaEngine->registerAudioFrameObserver(&s_audioFrameObserver);

    }

    if (gCallBack == nullptr) {
        gCallBack = env->NewGlobalRef(callback);
        gCallbackClass = env->GetObjectClass(gCallBack);
        recordAudioMethodId = env->GetMethodID(gCallbackClass, "onRecordAudioFrame", "(IIIIIJI)V");
        playbackAudioMethodId = env->GetMethodID(gCallbackClass, "onPlaybackAudioFrame",
                                                 "(IIIIIJI)V");
        playBeforeMixAudioMethodId = env->GetMethodID(gCallbackClass,
                                                      "onPlaybackAudioFrameBeforeMixing",
                                                      "(IIIIIJI)V");
        mixAudioMethodId = env->GetMethodID(gCallbackClass, "onMixedAudioFrame", "(IIIIIJI)V");

        captureVideoMethodId = env->GetMethodID(gCallbackClass, "onCaptureVideoFrame",
                                                "(IIIIIIIIIJ)V");
        renderVideoMethodId = env->GetMethodID(gCallbackClass, "onRenderVideoFrame",
                                               "(IIIIIIIIIJ)V");
        __android_log_print(ANDROID_LOG_DEBUG, "setCallback", "Callback is set successfully");

    }

}

JNIEXPORT void JNICALL Java_io_agora_rtc_plugin_rawdata_MediaPreProcessing_setVideoCaptureByteBUffer
        (JNIEnv *env, jobject obj, jobject bytebuffer) {
    _javaDirectPlayBufferCapture = env->GetDirectBufferAddress(bytebuffer);

}

JNIEXPORT void JNICALL Java_io_agora_rtc_plugin_rawdata_MediaPreProcessing_setAudioRecordByteBUffer
        (JNIEnv *env, jobject obj, jobject bytebuffer) {
    _javaDirectPlayBufferRecordAudio = env->GetDirectBufferAddress(bytebuffer);

}
JNIEXPORT void JNICALL Java_io_agora_rtc_plugin_rawdata_MediaPreProcessing_setAudioPlayByteBUffer
        (JNIEnv *env, jobject obj, jobject bytebuffer) {
    _javaDirectPlayBufferPlayAudio = env->GetDirectBufferAddress(bytebuffer);

}
JNIEXPORT void JNICALL
Java_io_agora_rtc_plugin_rawdata_MediaPreProcessing_setBeforeAudioMixByteBUffer
        (JNIEnv *env, jobject obj, jobject bytebuffer) {
    _javaDirectPlayBufferBeforeMixAudio = env->GetDirectBufferAddress(bytebuffer);

}
JNIEXPORT void JNICALL Java_io_agora_rtc_plugin_rawdata_MediaPreProcessing_setAudioMixByteBUffer
        (JNIEnv *env, jobject obj, jobject bytebuffer) {
    _javaDirectPlayBufferMixAudio = env->GetDirectBufferAddress(bytebuffer);

}

JNIEXPORT void JNICALL
Java_io_agora_rtc_plugin_rawdata_MediaPreProcessing_setVideoDecodeByteBUffer(JNIEnv *env,
                                                                             jobject type,
                                                                             jint uid,
                                                                             jobject byteBuffer) {
    if (byteBuffer == nullptr) {
        decodeByfferMap.erase(uid);
    } else {
        void *_javaDirectDecodeBuffer = env->GetDirectBufferAddress(byteBuffer);
        decodeByfferMap.insert(make_pair(uid, _javaDirectDecodeBuffer));
        __android_log_print(ANDROID_LOG_DEBUG, "plugin",
                            "yttest setVideoDecodeByteBUffer uid :%d, _javaDirectDecodeBuffer : %p",
                            uid, _javaDirectDecodeBuffer);
    }


}
JNIEXPORT void JNICALL
Java_io_agora_rtc_plugin_rawdata_MediaPreProcessing_releasePoint(JNIEnv *env, jobject type) {


    agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
    mediaEngine.queryInterface(rtcEngine, agora::INTERFACE_ID_TYPE::AGORA_IID_MEDIA_ENGINE);
    if (mediaEngine) {

        mediaEngine->registerVideoFrameObserver(NULL);
        mediaEngine->registerAudioFrameObserver(NULL);

    }

    gCallBack = nullptr;
    gCallbackClass = nullptr;
    gCallBack = nullptr;
    gCallbackClass = nullptr;

    recordAudioMethodId = nullptr;
    playbackAudioMethodId = nullptr;
    playBeforeMixAudioMethodId = nullptr;
    mixAudioMethodId = nullptr;
    captureVideoMethodId = nullptr;
    renderVideoMethodId = nullptr;

    _javaDirectPlayBufferCapture = nullptr;
    _javaDirectPlayBufferRecordAudio = nullptr;
    _javaDirectPlayBufferPlayAudio = nullptr;
    _javaDirectPlayBufferBeforeMixAudio = nullptr;
    _javaDirectPlayBufferMixAudio = nullptr;

    decodeByfferMap.clear();


}


#ifdef __cplusplus
}
#endif
