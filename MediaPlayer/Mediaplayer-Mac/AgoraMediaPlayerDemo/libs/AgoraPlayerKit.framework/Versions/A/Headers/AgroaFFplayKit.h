//
//  AgroaFFplayKit.h
//  AgoraPlayerKit
//
//  Created by zhanxiaochao on 2018/11/2.
//  Copyright © 2018年 agora. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface AgroaFFplayKit : NSObject
+(instancetype)shareInstance;

@property (nonatomic, strong) void(^yuvDataCallBack)(int linsize[_Nullable], void *data,int w , int h);
@property (nonatomic, strong) void(^audioDataCallBack)(void *data,int length);
@property (nonatomic, strong) void(^videoInfoDataCallBack)(long duation,int sample_rate, int channels);
// 加载播放的文件/可以是链接
-(void)loadVideo:(NSString *)videoPath isAutoPlay:(bool) isAutoPlay;
// 获取视频时长
-(int)getDuation;
// 获取当前的视频位置
-(long)getCurrentPosition;
// 拖动到视频播放的位置
-(void)seekTo:(int) msec;
// 调节本地播放音量
-(void)adjustPlaybackSignalVolume:(int) val;
// 视频的播放/暂停
-(void)play;
-(void)pause;
// 卸载视频
-(void)unload;
//// 销毁 VideoPlayerKit
//-(void)destroy;

@end

NS_ASSUME_NONNULL_END
