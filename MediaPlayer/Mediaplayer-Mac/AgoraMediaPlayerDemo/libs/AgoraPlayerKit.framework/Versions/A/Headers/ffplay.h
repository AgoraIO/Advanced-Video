//
//  ffplay.h
//  AgoraPlayDemo
//
//  Created by zhanxiaochao on 2018/10/16.
//  Copyright © 2018年 agora. All rights reserved.



#ifndef ffplay_h
#define ffplay_h

struct VideoState;
int  video_main(const char *url,int isAutoPlay);
void video_play_pause(void);
void video_seek(int time);
void video_seek_bar(int pos);
void video_quit(void);
void video_yuvCallBack(int linsize[], void *data,int w , int h);
long video_get_current_position(void);
void audio_dataCallBack(void *data,int length);
void video_set_param_videoInfo(struct VideoState *is);
void video_videoInfoCallBack(long duration,int sampleRate,int channels);
//0 ~ 128
void video_adjustPlaybackVolume(int val);
#endif /* ffplay_h */
