//
//  CallViewController.m
//  OpenDuo
//
//  Created by suleyu on 2017/10/31.
//  Copyright © 2017 Agora. All rights reserved.
//

#import "CallViewController.h"
#import <AVFoundation/AVFoundation.h>
#import <AgoraRtcEngineKit/AgoraRtcEngineKit.h>
#import <AgoraRtmKit/AgoraRtmKit.h>
#import "KeyCenter.h"
#import "AlertUtil.h"
#import "NSObject+JSONString.h"

@interface CallViewController () <AgoraRtcEngineDelegate, AgoraRtmCallDelegate>

@property (weak, nonatomic) IBOutlet UIView *remoteVideo;
@property (weak, nonatomic) IBOutlet UIView *localVideo;
@property (weak, nonatomic) IBOutlet UILabel *callingLabel;
@property (weak, nonatomic) IBOutlet UIStackView *buttonStackView;
@property (weak, nonatomic) IBOutlet UIButton *hangupButton;
@property (weak, nonatomic) IBOutlet UIButton *acceptButton;

@property (strong, nonatomic) AgoraRtcEngineKit *mediaEngine;
@property (strong, nonatomic) AVAudioPlayer *audioPlayer;

@property (strong, nonatomic) AgoraRtmCallKit *callKit;
@property (strong, nonatomic) AgoraRtmLocalInvitation *localInvitation;
@end

@implementation CallViewController
-(void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.callKit = [self.signalEngine getRtmCallKit];
    self.callKit.callDelegate = self;
    
//    self.signalEngine.agoraRtmDelegate = self;
    
    if (self.remoteInvitation) {
        self.callingLabel.text = [NSString stringWithFormat:@"%@ is calling ...", self.remoteAccount];
        [self loadMediaEngine];
        [self startLocalVideo];
        [self playRing:@"ring"];
    } else {
        self.callingLabel.text = [NSString stringWithFormat:@"%@ is being called ...", self.remoteAccount];
        self.buttonStackView.axis = UILayoutConstraintAxisVertical;
        [self.acceptButton removeFromSuperview];
        
        __weak typeof(self) weakSelf = self;
        [self.signalEngine queryPeersOnlineStatus:@[self.remoteAccount] completion:^(NSArray<AgoraRtmPeerOnlineStatus *> *peerOnlineStatus, AgoraRtmQueryPeersOnlineErrorCode errorCode) {
            AgoraRtmPeerOnlineStatus *status = peerOnlineStatus.firstObject;
            NSLog(@"onQueryPeersOnlineStatus, peerId: %@, isOnline: %d", status.peerId, status.isOnline);
            if (status.isOnline) {
                [weakSelf loadMediaEngine];
                [weakSelf startLocalVideo];
                [weakSelf sendInviteRequest];
            } else {
                NSString *message = [NSString stringWithFormat:@"%@ is not online", status.peerId];
                [AlertUtil showAlert:message completion:^{
                    [weakSelf dismissViewControllerAnimated:NO completion:nil];
                }];
            }
        }];
    }
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    self.mediaEngine.delegate = nil;
}

- (void)rtmCallKit:(AgoraRtmCallKit *)callKit localInvitationReceivedByPeer:(AgoraRtmLocalInvitation *)localInvitation {
    NSLog(@"localInvitationReceivedByPeer: %@, channel: %@", localInvitation.calleeId, localInvitation.channelId);
    if (![localInvitation.channelId isEqualToString:self.localInvitation.channelId] || ![localInvitation.calleeId isEqualToString:self.remoteAccount]) {
        return;
    }
    
    [self playRing:@"tones"];
}

- (void)rtmCallKit:(AgoraRtmCallKit *)callKit localInvitationAccepted:(AgoraRtmLocalInvitation *)localInvitation withResponse:(NSString *)response {
    NSLog(@"localInvitationAccepted by: %@, response: %@", localInvitation.calleeId, response);
    if (![localInvitation.channelId isEqualToString:self.localInvitation.channelId] || ![localInvitation.calleeId isEqualToString:self.remoteAccount]) {
        return;
    }
    
    self.callingLabel.hidden = YES;
    [self adjustLocalVideoView];
    
    [self stopRing];
    [self joinChannel];
}

- (void)rtmCallKit:(AgoraRtmCallKit *)callKit localInvitationRefused:(AgoraRtmLocalInvitation *)localInvitation withResponse:(NSString *)response {
    NSLog(@"localInvitationRefused: %@, response: %@", localInvitation.calleeId, response);
    if (![localInvitation.channelId isEqualToString:self.localInvitation.channelId] || ![localInvitation.calleeId isEqualToString:self.remoteAccount]) {
        return;
    }
    
    [self stopRing];
    [self leaveChannel];
    
    if ([response isEqualToString:@"busy"]) {
        NSString *message = [NSString stringWithFormat:@"%@ is busy", localInvitation.calleeId];
        [AlertUtil showAlert:message completion:^{
            [self dismissViewControllerAnimated:NO completion:nil];
        }];
    } else {
        [self dismissViewControllerAnimated:NO completion:nil];
    }
}

- (void)rtmCallKit:(AgoraRtmCallKit *)callKit remoteInvitationCanceled:(AgoraRtmRemoteInvitation *)remoteInvitation {
    if (!self.callingLabel.hidden) {
        __weak typeof(self) weakSelf = self;
        [AlertUtil showAlert:@"Call canceled" completion:^{
            [weakSelf dismissViewControllerAnimated:NO completion:nil];
        }];
    }
}

- (void)sendInviteRequest {
    AgoraRtmLocalInvitation *invitation = [[AgoraRtmLocalInvitation alloc] initWithCalleeId:self.remoteAccount];
    NSString *channel = [NSString stringWithFormat:@"%@-%@-%f", self.localAccount, self.remoteAccount, [NSDate date].timeIntervalSinceReferenceDate];
    invitation.channelId = channel;
    self.localInvitation = invitation;
    
    __weak typeof(self) weakSelf = self;
    [self.callKit sendLocalInvitation:invitation completion:^(AgoraRtmInvitationApiCallErrorCode errorCode) {
        if (errorCode != AgoraRtmInvitationApiCallErrorOk) {
            NSLog(@"Call %@ failed, ecode: %lu", self.remoteAccount, (unsigned long)errorCode);
            if (![invitation.calleeId isEqualToString:weakSelf.remoteAccount]) {
                return;
            }
            
            [weakSelf leaveChannel];
            
            [AlertUtil showAlert:@"Call failed" completion:^{
                [weakSelf dismissViewControllerAnimated:NO completion:nil];
            }];
        }
    }];
}

- (IBAction)muteButtonClicked:(UIButton *)sender {
    if (self.mediaEngine) {
        [sender setSelected:!sender.isSelected];
        [self.mediaEngine muteLocalAudioStream:sender.isSelected];
    }
}

- (IBAction)switchCameraButtonClicked:(UIButton *)sender {
    if (self.mediaEngine) {
        [sender setSelected:!sender.isSelected];
        [self.mediaEngine switchCamera];
    }
}

- (IBAction)hangupButtonClicked:(UIButton *)sender {
    if (self.acceptButton) {
        // called by other
        [self.callKit refuseRemoteInvitation:self.remoteInvitation completion:nil];
        [self stopRing];
    } else {
        if (self.callingLabel.hidden) {
            // already accepted
            [self leaveChannel];
        } else {
            // calling other
            [self.callKit cancelLocalInvitation:self.localInvitation completion:nil];
            [self stopRing];
        }
    }
    
    [self dismissViewControllerAnimated:NO completion:nil];
}

- (IBAction)acceptButtonClicked:(UIButton *)sender {
    [self.callKit acceptRemoteInvitation:self.remoteInvitation completion:nil];
    
    self.callingLabel.hidden = YES;
    self.buttonStackView.axis = UILayoutConstraintAxisVertical;
    [self.acceptButton removeFromSuperview];
    [self adjustLocalVideoView];
    
    [self stopRing];
    [self joinChannel];
}

- (void)logout {
    [self stopRing];
    [self leaveChannel];
}

- (void)playRing:(NSString *)name {
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    [audioSession setCategory:AVAudioSessionCategoryPlayback error:nil];
    [audioSession setActive:YES error:nil];
    
    NSURL *path = [[NSBundle mainBundle] URLForResource:name withExtension:@"mp3"];
    self.audioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:path error:nil];
    self.audioPlayer.numberOfLoops = 1;
    [self.audioPlayer play];
}

- (void)stopRing {
    if (self.audioPlayer) {
        [self.audioPlayer stop];
        self.audioPlayer = nil;
    }
}

- (void)loadMediaEngine {
    self.mediaEngine = [AgoraRtcEngineKit sharedEngineWithAppId:[KeyCenter appId] delegate:self];
    
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy-MM-dd ah-mm-ss"];
    NSString *logFilePath = [NSString stringWithFormat:@"%@/AgoraRtcEngine %@.log",
                      NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES).firstObject,
                      [dateFormatter stringFromDate:[NSDate date]]];
    [self.mediaEngine setLogFile:logFilePath];
    //[mediaEngine setParameters:@"{\"rtc.log_filter\":65535}"];
    
    [self.mediaEngine enableVideo];
    
    AgoraVideoEncoderConfiguration *videoConfiguration = [[AgoraVideoEncoderConfiguration alloc] initWithSize:AgoraVideoDimension640x360
                                                                                                    frameRate:AgoraVideoFrameRateFps15
                                                                                                      bitrate:AgoraVideoBitrateStandard
                                                                                              orientationMode:AgoraVideoOutputOrientationModeAdaptative];
    [self.mediaEngine setVideoEncoderConfiguration:videoConfiguration];
}

- (void)startLocalVideo {
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = 0;
    videoCanvas.view = self.localVideo;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.mediaEngine setupLocalVideo:videoCanvas];
    [self.mediaEngine startPreview];
}

- (void)adjustLocalVideoView {
    self.localVideo.frame = CGRectMake(self.view.frame.size.width - 110, 20, 90, 160);
    self.localVideo.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleBottomMargin;
}

- (void)joinChannel {
    NSString *mediaToken = nil;
    NSString *channel;
    if (self.localInvitation) {
        channel = self.localInvitation.channelId;
    } else {
        channel = self.remoteInvitation.channelId;
    }
    [self.mediaEngine joinChannelByToken:mediaToken channelId:channel info:nil uid:0 joinSuccess:nil];
}

- (void)leaveChannel {
    if (self.mediaEngine) {
        [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
        [self.mediaEngine stopPreview];
        [self.mediaEngine setupLocalVideo:nil];
        [self.mediaEngine leaveChannel:nil];
        self.mediaEngine = nil;
    }
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOccurWarning:(AgoraWarningCode)warningCode {
    NSLog(@"rtcEngine:didOccurWarning: %ld", (long)warningCode);
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOccurError:(AgoraErrorCode)errorCode {
    NSLog(@"rtcEngine:didOccurError: %ld", (long)errorCode);
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinChannel:(NSString*)channel withUid:(NSUInteger)uid elapsed:(NSInteger) elapsed {
    NSLog(@"rtcEngine:didJoinChannel: %@", channel);
    [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didJoinedOfUid:(NSUInteger)uid elapsed:(NSInteger)elapsed {
    NSLog(@"rtcEngine:didJoinedOfUid: %ld", (long)uid);
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = uid;
    videoCanvas.view = self.remoteVideo;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [self.mediaEngine setupRemoteVideo:videoCanvas];
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    NSLog(@"rtcEngine:didOfflineOfUid: %ld", (long)uid);
    // only receive this callback if remote user logout unexpected
    [self leaveChannel];
    [self dismissViewControllerAnimated:NO completion:nil];
}

@end
