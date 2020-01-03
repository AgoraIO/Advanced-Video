//
//  VideoChatViewController.m
//  Agora iOS Tutorial Objective-C
//
//  Created by James Fang on 7/15/16.
//  Copyright © 2016 Agora.io. All rights reserved.
//

#import <AgoraRtcKit/AgoraRtcEngineKit.h>
#import "VideoChatViewController.h"
#import "AgoraMediaDataPlugin.h"
#import "KeyCenter.h"

@interface VideoChatViewController () <AgoraRtcEngineDelegate, AgoraVideoDataPluginDelegate, AgoraAudioDataPluginDelegate, AgoraPacketDataPluginDelegate>
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
    
    [self addGestureRecognizer];
    [self hideVideoMuted];
    [self initializeAgoraEngine];
    [self setupVideo];
    [self setupLocalVideo];
    [self initializeMediaDataPlugin];
    [self joinChannel];
}

- (void)addGestureRecognizer {
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
    self.agoraKit = [AgoraRtcEngineKit sharedEngineWithAppId:KeyCenter.AppId delegate:self];
}

- (void)setupVideo {
    [self.agoraKit enableVideo];
    [self.agoraKit setVideoEncoderConfiguration:[[AgoraVideoEncoderConfiguration alloc] initWithSize:AgoraVideoDimension640x360
                                                                                           frameRate:AgoraVideoFrameRateFps15
                                                                                             bitrate:0
                                                                                     orientationMode:AgoraVideoOutputOrientationModeAdaptative]];
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
    
    // Register audio observer
    ObserverAudioType audioType = ObserverAudioTypeRecordAudio | ObserverAudioTypePlaybackAudioFrameBeforeMixing | ObserverAudioTypeMixedAudio | ObserverAudioTypePlaybackAudio;
    [self.agoraMediaDataPlugin registerAudioRawDataObserver:audioType];
    self.agoraMediaDataPlugin.audioDelegate = self;
    
    [self.agoraKit setRecordingAudioFrameParametersWithSampleRate:44100
                                                          channel:1
                                                             mode:AgoraAudioRawFrameOperationModeReadWrite
                                                   samplesPerCall:4410];
    [self.agoraKit setMixedAudioFrameParametersWithSampleRate:44100
                                               samplesPerCall:4410];
    [self.agoraKit setPlaybackAudioFrameParametersWithSampleRate:44100
                                                         channel:1
                                                            mode:AgoraAudioRawFrameOperationModeReadWrite
                                                  samplesPerCall:4410];
    
    // Register video observer
    ObserverVideoType videoType = ObserverVideoTypeCaptureVideo | ObserverVideoTypeRenderVideo;
    [self.agoraMediaDataPlugin registerVideoRawDataObserver:videoType];
    self.agoraMediaDataPlugin.videoDelegate = self;
    
    // Register packet observer
    ObserverPacketType packetType = ObserverPacketTypeSendAudio | ObserverPacketTypeSendVideo | ObserverPacketTypeReceiveAudio | ObserverPacketTypeReceiveVideo;
    [self.agoraMediaDataPlugin registerPacketRawDataObserver:packetType];
    self.agoraMediaDataPlugin.packetDelegate = self;
}

- (void)joinChannel {
    [self.agoraKit joinChannelByToken:nil
                            channelId:@"rawdatademo1"
                                 info:nil
                                  uid:0
                          joinSuccess:^(NSString * _Nonnull channel, NSUInteger uid, NSInteger elapsed) {
        
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
    videoCanvas.renderMode = AgoraVideoRenderModeFit;
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

- (AgoraVideoRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin willRenderVideoRawData:(AgoraVideoRawData *)videoRawData ofUid:(uint)uid {
    return videoRawData;
}

#pragma mark - <AgoraAudioDataPluginDelegate>
- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin didRecordAudioRawData:(AgoraAudioRawData *)audioRawData {
    return audioRawData;
}

- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin willPlaybackAudioRawData:(AgoraAudioRawData *)audioRawData {
    return audioRawData;
}

- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin willPlaybackBeforeMixingAudioRawData:(AgoraAudioRawData *)audioRawData ofUid:(uint)uid {
    return audioRawData;
}

- (AgoraAudioRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin didMixedAudioRawData:(AgoraAudioRawData *)audioRawData {
    return audioRawData;
}

#pragma mark - <AgoraPacketDataPluginDelegate>
- (AgoraPacketRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin willSendAudioPacket:(AgoraPacketRawData *)audioPacket {
    char newBuffer[audioPacket.bufferSize];

    for (int i = 0; i < audioPacket.bufferSize; i ++) {
        int temp = audioPacket.buffer[i] ^ 0x55;
        newBuffer[i] = temp;
    }

    memcpy((void*)audioPacket.buffer, newBuffer, audioPacket.bufferSize);
    
    return audioPacket;
}

- (AgoraPacketRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin willSendVideoPacket:(AgoraPacketRawData *)videoPacket {
    char newBuffer[videoPacket.bufferSize];

    for (int i = 0; i < videoPacket.bufferSize; i ++) {
        int temp = videoPacket.buffer[i] ^ 0x55;
        newBuffer[i] = temp;
    }

    memcpy((void*)videoPacket.buffer, newBuffer, videoPacket.bufferSize);
    
    return videoPacket;
}

- (AgoraPacketRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin didReceivedAudioPacket:(AgoraPacketRawData *)audioPacket {
    char newBuffer[audioPacket.bufferSize];

    for (int i = 0; i < audioPacket.bufferSize; i ++) {
        int temp = audioPacket.buffer[i] ^ 0x55;
        newBuffer[i] = temp;
    }

    memcpy((void*)audioPacket.buffer, newBuffer, audioPacket.bufferSize);

    return audioPacket;
}

- (AgoraPacketRawData *)mediaDataPlugin:(AgoraMediaDataPlugin *)mediaDataPlugin didReceivedVideoPacket:(AgoraPacketRawData *)videoPacket {
    char newBuffer[videoPacket.bufferSize];

    for (int i = 0; i < videoPacket.bufferSize; i ++) {
        int temp = videoPacket.buffer[i] ^ 0x55;
        newBuffer[i] = temp;
    }

    memcpy((void*)videoPacket.buffer, newBuffer, videoPacket.bufferSize);
    
    return videoPacket;
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
    [self.agoraMediaDataPlugin localSnapshot:^(AGImage * _Nonnull image) {
        UIImageWriteToSavedPhotosAlbum(image, nil, nil, nil);
    }];
}

- (IBAction)didClickRemoteVideoShot:(UIButton *)sender {
    [self.agoraMediaDataPlugin remoteSnapshotWithUid:self.remoteUid image:^(AGImage * _Nonnull image) {
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
