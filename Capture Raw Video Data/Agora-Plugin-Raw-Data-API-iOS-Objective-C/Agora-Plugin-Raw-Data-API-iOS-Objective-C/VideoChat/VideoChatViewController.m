//
//  VideoChatViewController.m
//  Agora iOS Tutorial Objective-C
//
//  Created by James Fang on 7/15/16.
//  Copyright © 2016 Agora.io. All rights reserved.
//

#import "VideoChatViewController.h"
#import "AgoraMediaDataPlugin.h"

@interface VideoChatViewController () <AgoraRtcEngineDelegate, AgoraVideoDataPluginDelegate, AgoraAudioDataPluginDelegate>
@property (weak, nonatomic) IBOutlet UIView *localVideo;
@property (weak, nonatomic) IBOutlet UIView *remoteVideo;
@property (weak, nonatomic) IBOutlet UIView *controlButtons;
@property (weak, nonatomic) IBOutlet UIImageView *remoteVideoMutedIndicator;
@property (weak, nonatomic) IBOutlet UIImageView *localVideoMutedBg;
@property (weak, nonatomic) IBOutlet UIImageView *localVideoMutedIndicator;
@property (nonatomic, strong) AgoraRtcEngineKit *agoraKit;
@property (nonatomic, strong) AgoraMediaDataPlugin *agoraMediaDataPlugin;
@property (nonatomic, assign) NSUInteger remoteUid;
@end

@implementation VideoChatViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self setupButtons];
    [self hideVideoMuted];
    [self initializeAgoraEngine];
    [self setupVideo];
    [self setupLocalVideo];
    [self initializeMediaDataPlugin];
    [self joinChannel];
}

- (void)setupButtons {
    [self performSelector:@selector(hideControlButtons) withObject:nil afterDelay:3];
    UITapGestureRecognizer *tapGestureRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(remoteVideoTapped:)];
    [self.view addGestureRecognizer:tapGestureRecognizer];
    self.view.userInteractionEnabled = true;
}

- (void)hideVideoMuted {
    self.remoteVideoMutedIndicator.hidden = true;
    self.localVideoMutedBg.hidden = true;
    self.localVideoMutedIndicator.hidden = true;
}

- (void)initializeAgoraEngine {
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithAppId:appID delegate:self];
}

- (void)setupVideo {
    [self.agoraKit enableVideo];
    [self.agoraKit setVideoProfile:AgoraVideoProfilePortrait360P swapWidthAndHeight: true];
}

- (void)setupLocalVideo {
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = 0;
    videoCanvas.view = self.localVideo;
    videoCanvas.renderMode = AgoraVideoRenderModeFit;
    [self.agoraKit setupLocalVideo:videoCanvas];
}

- (void)initializeMediaDataPlugin {
    self.agoraMediaDataPlugin = [AgoraMediaDataPlugin mediaDataPluginWithAgoraKit:self.agoraKit];
    self.agoraMediaDataPlugin.videoDelegate = self;
    self.agoraMediaDataPlugin.audioDelegate = self;
    
    [self.agoraKit setRecordingAudioFrameParametersWithSampleRate:44100 channel:1 mode:AgoraAudioRawFrameOperationModeReadWrite samplesPerCall:4410];
    [self.agoraKit setPlaybackAudioFrameParametersWithSampleRate:44100 channel:1 mode:AgoraAudioRawFrameOperationModeReadWrite samplesPerCall:4410];
    [self.agoraKit setMixedAudioFrameParametersWithSampleRate:44100 samplesPerCall:4410];
}

- (void)joinChannel {
    [self.agoraKit joinChannelByToken:nil channelId:@"cavan123" info:nil uid:0 joinSuccess:^(NSString * _Nonnull channel, NSUInteger uid, NSInteger elapsed) {
        [self.agoraKit setEnableSpeakerphone:YES];
        [UIApplication sharedApplication].idleTimerDisabled = YES;
    }];
}

#pragma mark - <AgoraRtcEngineDelegate>
- (void)rtcEngine:(AgoraRtcEngineKit *)engine firstRemoteVideoDecodedOfUid:(NSUInteger)uid size: (CGSize)size elapsed:(NSInteger)elapsed {
    if (self.remoteVideo.hidden)
        self.remoteVideo.hidden = NO;
    
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = uid;
    
    videoCanvas.view = self.remoteVideo;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.agoraKit setupRemoteVideo:videoCanvas];
    
    self.remoteUid = uid;
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    self.remoteVideo.hidden = YES;
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didVideoMuted:(BOOL)muted byUid:(NSUInteger)uid {
    self.remoteVideo.hidden = muted;
    self.remoteVideoMutedIndicator.hidden = !muted;
}

#pragma mark - <AgoraVideoDataPluginDelegate>
- (AgoraVideoRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin didCapturedVideoRawData:(AgoraVideoRawData *)videoRawData {
    return videoRawData;
}

- (AgoraVideoRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin willRenderVideoRawData:(AgoraVideoRawData *)videoRawData {
    return videoRawData;
}

#pragma mark - <AgoraAudioDataPluginDelegate>
- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin didRecordAudioRawData:(AgoraAudioRawData *)audioRawData {
    return audioRawData;
}

- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin willPlaybackAudioRawData:(AgoraAudioRawData *)audioRawData {
    return audioRawData;
}

- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin willPlaybackBeforeMixingAudioRawData:(AgoraAudioRawData *)audioRawData {
    return audioRawData;
}

- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin didMixedAudioRawData:(AgoraAudioRawData *)audioRawData {
    return audioRawData;
}

#pragma mark - <Button Click>
- (IBAction)hangUpButton:(UIButton *)sender {
    [self leaveChannel];
}

- (IBAction)didClickMuteButton:(UIButton *)sender {
    sender.selected = !sender.selected;
    [self.agoraKit muteLocalAudioStream:sender.selected];
    [self resetHideButtonsTimer];
}

- (IBAction)didClickVideoMuteButton:(UIButton *)sender {
    sender.selected = !sender.selected;
    [self.agoraKit muteLocalVideoStream:sender.selected];
    self.localVideo.hidden = sender.selected;
    self.localVideoMutedBg.hidden = !sender.selected;
    self.localVideoMutedIndicator.hidden = !sender.selected;
    [self resetHideButtonsTimer];
}

- (IBAction)didClickSwitchCameraButton:(UIButton *)sender {
    sender.selected = !sender.selected;
    [self.agoraKit switchCamera];
    [self resetHideButtonsTimer];
}

- (IBAction)didClickLocalVideoShot:(UIButton *)sender {
    [self.agoraMediaDataPlugin screenShotDidCaptureLocalWithImage:^(UIImage *image) {
        UIImageWriteToSavedPhotosAlbum(image, nil, nil, nil);
    }];
}

- (IBAction)didClickRemoteVideoShot:(UIButton *)sender {
    [self.agoraMediaDataPlugin screenShotWillRenderWithUid:self.remoteUid image:^(UIImage *image) {
        UIImageWriteToSavedPhotosAlbum(image, nil, nil, nil);
    }];
}

- (void)leaveChannel {
    [self.agoraKit leaveChannel:^(AgoraChannelStats * _Nonnull stat) {
        [self hideControlButtons];     // Tutorial Step 8
        [UIApplication sharedApplication].idleTimerDisabled = NO;
        [self.remoteVideo removeFromSuperview];
        [self.localVideo removeFromSuperview];
        self.agoraKit = nil;
        self.agoraMediaDataPlugin.audioDelegate = nil;
        self.agoraMediaDataPlugin.videoDelegate = nil;
    }];
}

- (void)remoteVideoTapped:(UITapGestureRecognizer *)recognizer {
    if (self.controlButtons.hidden) {
        self.controlButtons.hidden = false;
        [self performSelector:@selector(hideControlButtons) withObject:nil afterDelay:3];
    }
}

- (void)hideControlButtons {
    self.controlButtons.hidden = true;
}

- (void)resetHideButtonsTimer {
    [VideoChatViewController cancelPreviousPerformRequestsWithTarget:self];
    [self performSelector:@selector(hideControlButtons) withObject:nil afterDelay:3];
}

@end
