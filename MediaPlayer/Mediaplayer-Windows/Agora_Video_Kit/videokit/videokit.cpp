#include "videokit.h"
#include "videoplayertool.h"
#include "AgoraBase.h"
using namespace agora::media;
using namespace agora::rtc;


static void videoRawDataCallBack(int linsize[], void *data, int w, int h){
	
	VideoKit::Get()->onVideoDataCallBack(linsize,data,w,h);
}
static void audioRawDataCallBack(void *data, int len){
	VideoKit::Get()->onAudioDataCallBack(data, len);
}
static void audioInfoCallBack(int sample_rate, int channles, long duation,int width,int height){
	VideoKit::Get()->onAudioInfoCallBack(sample_rate, channles, duation,width,height);
}
static void videoStateCallBack(VIDEO_STATE state){
	VideoKit::Get()->onVideoStateCallBack((FF_VIDEO_STATE)state);

}
void VideoKit::onVideoDataCallBack(int linesize[], void *data, int w, int h){

	if (isExit)
	{
		return;
	}

	int size = w * h * 3 / 2;
	byte *tmp = (byte *)malloc(size);
	memset(tmp, 0, size);
	if (linesize[0] != w)
	{
		for (int i = 0; i < h; i++)
		{
			memcpy(tmp + i* w ,(byte *)data + i * linesize[0],w);
		}
		for (int i = 0; i < h/2; i++)
		{
			memcpy(tmp + w * h + w / 2 * i, (byte *)data +linesize[0] * h  + i * linesize[0] / 2, w/2);

		}
		for (int i = 0; i < h/2; i++)
		{
			memcpy(tmp +w*h * 5/4 +  w/2 * i, (byte *)data +linesize[0]*h*5/4  + linesize[0]/ 2 * i, w/2);

		}
	}
	else {
		memcpy(tmp, data, size);

	}
	d3dRender.Render((char *)tmp);
	ExternalVideoFrame frame;
	frame.stride = w;
	frame.height = h;
	frame.timestamp = static_cast<long long >(GetHighAccuracyTickCount());
	frame.rotation = 0;
	frame.type = ExternalVideoFrame::VIDEO_BUFFER_TYPE::VIDEO_BUFFER_RAW_DATA;
	frame.format = ExternalVideoFrame::VIDEO_PIXEL_FORMAT::VIDEO_PIXEL_I420;
	frame.cropLeft = 0;
	frame.cropTop = 0;
	frame.cropBottom = 0;
	frame.cropRight = 0;
	frame.buffer = tmp;
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(_rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);

	if (mediaEngine)
		mediaEngine->pushVideoFrame(&frame);

	free(tmp);
}

void VideoKit::onAudioDataCallBack(void *data, int len){

	if (isVieoPublish) {
		return;
	}
	m_mutex.lock();
    CExtendAudioFrameObserver::Get()->pushAudioData(data, len);
	m_mutex.unlock();

}
WNDPROC OLDWNDPROC;
LRESULT CALLBACK MyWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
	switch (message)
	{
	case  WM_SIZE:
	{
         //让显示窗口刷新重启
		VideoKit::Get()->refreshWindows();
		
	}
	break;
	default:
		return ::CallWindowProc(OLDWNDPROC, hWnd, message, wParam, lParam);
	}

	return 0;
}


void VideoKit::onAudioInfoCallBack(int sample_rate, int channels, long duration,int width,int height){

	CExtendAudioFrameObserver::Get()->re_channels = channels;
	CExtendAudioFrameObserver::Get()->re_sample_rate = sample_rate;
	this->duration = duration;
	pic_width = width;
	pic_height = height;
	//监听windows窗口大小改变的回调
	if (!isSetWindows)
	{
		OLDWNDPROC = (WNDPROC)SetWindowLong((HWND)m_hwnd, GWL_WNDPROC, (LONG)(MyWndProc));
		isSetWindows = true;
	}
	refreshWindows();
}
void VideoKit::onVideoStateCallBack(FF_VIDEO_STATE state){

	if (!this->handler)
	{
		return;
	}

	switch (state)
	{
	case FF_VIDEO_START:
	{
	    break;
	}
	case FF_VIDEO_ERROR:

		break;
	case FF_VIDEO_EXIT:

		break;
	case FF_VIDEO_COMPLETE:
	{
		this->handler->onPlayerStopped();
		break;
	}
	case FF_VIDEO_PREPARE:
	{
		this->handler->onVideoLoaded(FF_LOAD_VIDEO_ERROR_PREPARED);

	    break;
	}
	case FF_VIDEO_PAUSE:
	{
		this->handler->onPlayerSate(FF_PLAYER_STATE_PAUSE);
		break;

	}
	case FF_VIDEO_PLAY:
	{
		this->handler->onPlayerSate(FF_PLAYER_STATE_PLAY);
		break;
	}
	case FF_VIDEO_FILE_NULL:
	{
		this->handler->onVideoLoaded(FF_LOAD_VIDEO_ERROR_NULL_FILE);
		break;
	}
	case FF_VIDEO_INVALID_ARGUMENT:
	{	
		this->handler->onVideoLoaded(FF_LOAD_VIDEO_ERROR_UNSORPPORT_ARGUMENT);
		break;
	}
	default:
		break;
	}

}
void VideoKit::CreateVideoPlayerKitWithRtcEngine(agora::rtc::IRtcEngine *rtcEngine, int sampleRate){

	//enableVideoSource
	m_mutex.lock();

	CExtendAudioFrameObserver::Get()->sample_rate = sampleRate;
	_rtcEngine = rtcEngine;
	agora::util::AutoPtr<agora::media::IMediaEngine> mediaEngine;
	mediaEngine.queryInterface(rtcEngine, agora::AGORA_IID_MEDIA_ENGINE);
	if (mediaEngine)
		mediaEngine->registerAudioFrameObserver(CExtendAudioFrameObserver::Get());

	AParameter apm(rtcEngine);
	apm->setParameters("{\"che.video.local.camera_index\":0}");
	if (mediaEngine)
		mediaEngine->setExternalVideoSource(true, false);
	// set samplerate chanels
	RtcEngineParameters rep(rtcEngine);
	// rep.setExternalAudioSource(false,sampleRate,2);
    rep.setRecordingAudioFrameParameters(sampleRate, 2, RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, sampleRate * 0.01 * 2);
    rep.setPlaybackAudioFrameParameters(sampleRate,2,RAW_AUDIO_FRAME_OP_MODE_READ_WRITE, sampleRate * 0.01 * 2);
	m_mutex.unlock();

}
void VideoKit::loadVideo(char * str,bool isAutoPlay){
	if (isPlaying)
		return;
	isPlaying = true;
	isExit = false;

	videopool = new VideoPlayerTool();
	videopool->setVideoCallBack(videoRawDataCallBack);
	videopool->setAudioCallBack(audioRawDataCallBack);
	videopool->setAudioInfoCallBack(audioInfoCallBack);
	videopool->setVideoStateCallBack(videoStateCallBack);
	videopool->load_video(str, isAutoPlay);
	delete videopool;
}
//从头开始播放视频
void VideoKit::play(){

	m_mutex.lock();
	CExtendAudioFrameObserver::Get()->reset();
	videopool->video_seek(0);
	m_mutex.unlock();
}
void VideoKit::pause(){

	m_mutex.lock();
	videopool->video_play_pause();
	CExtendAudioFrameObserver::Get()->reset();
	m_mutex.unlock();

}
void VideoKit::resume() {
	m_mutex.lock();
	videopool->video_play_pause();
	CExtendAudioFrameObserver::Get()->reset();
	m_mutex.unlock();
}
void VideoKit::unload(){


	m_mutex.lock();
	isPlaying = false;
	isExit = true;
	duration = 0;
	videopool->video_quit();
	m_mutex.unlock();

}
long VideoKit::getCurrentPosition(){
	if (this->duration == 0)
	{
		return 0;
	}
	m_mutex.lock();
	double ret = videopool->getVideoCurrentPos();
	m_mutex.unlock();
	return ret;
}
void VideoKit::seekTo(long msec){

	m_mutex.lock();
	CExtendAudioFrameObserver::Get()->reset();
	videopool->video_seek(msec);
	m_mutex.unlock();

}

void VideoKit::setHandler(VideoPlayerKitEventHandler *handler){
	m_mutex.lock();
	this->handler = handler;
	m_mutex.unlock();
}
void VideoKit::setAudioMixing(bool isAudioMix){
	m_mutex.lock();
    CExtendAudioFrameObserver::Get()->setAudioMixing(isAudioMix);
	m_mutex.unlock();
}
void VideoKit::publish(){
	m_mutex.lock();
	isVieoPublish = false;
	m_mutex.unlock();

}
void VideoKit::unpublish
() {
	m_mutex.lock();
	isVieoPublish = true;
	m_mutex.unlock();
}
int VideoKit::setPreviewWindow(void *hwnd) {
	m_hwnd = hwnd;
    return 0;

}
void VideoKit::refreshWindows() {
	
	RECT rctA;
	::GetWindowRect((HWND)m_hwnd, &rctA);
	int width = rctA.right - rctA.left;
	int height = rctA.bottom - rctA.top;
	m_mutex.lock();
	d3dRender.Init((HWND)m_hwnd, pic_width, pic_height, true, width, height);
	m_mutex.unlock();
}