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
#import <AgoraSigKit/AgoraSigKit.h>
#import "KeyCenter.h"
#import "AlertUtil.h"
#import "NSObject+JSONString.h"

@interface CallViewController () <AgoraRtcEngineDelegate>
{
    AVAudioPlayer *audioPlayer;
    AgoraAPI *signalEngine;
    AgoraRtcEngineKit *mediaEngine;
}

@property (weak, nonatomic) IBOutlet UIView *remoteVideo;
@property (weak, nonatomic) IBOutlet UIView *localVideo;
@property (weak, nonatomic) IBOutlet UILabel *callingLabel;
@property (weak, nonatomic) IBOutlet UIStackView *buttonStackView;
@property (weak, nonatomic) IBOutlet UIButton *hangupButton;
@property (weak, nonatomic) IBOutlet UIButton *acceptButton;

@end

@implementation CallViewController

-(void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self loadSignalEngine];
    
    if (self.channel.length == 0) {
        self.callingLabel.text = [NSString stringWithFormat:@"%@ is being called ...", self.remoteAccount];
        self.buttonStackView.axis = UILayoutConstraintAxisVertical;
        [self.acceptButton removeFromSuperview];
        
        [signalEngine queryUserStatus:self.remoteAccount];
    }
    else {
        self.callingLabel.text = [NSString stringWithFormat:@"%@ is calling ...", self.remoteAccount];
        [self loadMediaEngine];
        [self startLocalVideo];
        [self playRing:@"ring"];
    }
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(applicationWillTerminate:)
                                                 name:UIApplicationWillTerminateNotification
                                               object:nil];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    
    [AgoraRtcEngineKit sharedEngineWithAppId:[KeyCenter appId] delegate:nil];
    
    signalEngine.onError = nil;
    signalEngine.onQueryUserStatusResult = nil;
    signalEngine.onInviteReceivedByPeer = nil;
    signalEngine.onInviteFailed = nil;
    signalEngine.onInviteAcceptedByPeer = nil;
    signalEngine.onInviteRefusedByPeer = nil;
    signalEngine.onInviteEndByPeer = nil;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)applicationWillTerminate:(NSNotification *)noti
{
    if (self.channel.length > 0) {
        [signalEngine channelInviteEnd:self.channel account:self.remoteAccount uid:0];
    }
}

- (void)loadSignalEngine {
    signalEngine = [AgoraAPI getInstanceWithoutMedia:[KeyCenter appId]];
    
    __weak typeof(self) weakSelf = self;
    
    signalEngine.onError = ^(NSString* name, AgoraEcode ecode, NSString* desc) {
        NSLog(@"onError, name: %@, code:%lu, desc: %@", name, (unsigned long)ecode, desc);
        if ([name isEqualToString:@"query_user_status"]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                [AlertUtil showAlert:desc completion:^{
                    [weakSelf dismissViewControllerAnimated:NO completion:nil];
                }];
            });
        }
    };
    
    signalEngine.onQueryUserStatusResult = ^(NSString *name, NSString *status) {
        NSLog(@"onQueryUserStatusResult, name: %@, status: %@", name, status);
        dispatch_async(dispatch_get_main_queue(), ^{
            if ([status intValue] == 0) {
                NSString *message = [NSString stringWithFormat:@"%@ is not online", name];
                [AlertUtil showAlert:message completion:^{
                    [weakSelf dismissViewControllerAnimated:NO completion:nil];
                }];
            }
            else {
                [weakSelf loadMediaEngine];
                [weakSelf startLocalVideo];
                [weakSelf sendInviteRequest];
            }
        });
    };
    
    signalEngine.onInviteReceivedByPeer = ^(NSString* channelID, NSString *account, uint32_t uid) {
        NSLog(@"onInviteReceivedByPeer, channel: %@, account: %@, uid: %u", channelID, account, uid);
        if (![channelID isEqualToString:weakSelf.channel] || ![account isEqualToString:weakSelf.remoteAccount]) {
            return;
        }
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf playRing:@"tones"];
        });
    };
    
    signalEngine.onInviteFailed = ^(NSString* channelID, NSString* account, uint32_t uid, AgoraEcode ecode, NSString *extra) {
        NSLog(@"Call %@ failed, ecode: %lu", account, (unsigned long)ecode);
        if (![channelID isEqualToString:weakSelf.channel] || ![account isEqualToString:weakSelf.remoteAccount]) {
            return;
        }
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf leaveChannel];
            
            [AlertUtil showAlert:@"Call failed" completion:^{
                [weakSelf dismissViewControllerAnimated:NO completion:nil];
            }];
        });
    };
    
    signalEngine.onInviteAcceptedByPeer = ^(NSString* channelID, NSString *account, uint32_t uid, NSString *extra) {
        NSLog(@"onInviteAcceptedByPeer, channel: %@, account: %@, uid: %u, extra: %@", channelID, account, uid, extra);
        if (![channelID isEqualToString:weakSelf.channel] || ![account isEqualToString:weakSelf.remoteAccount]) {
            return;
        }
        
        dispatch_async(dispatch_get_main_queue(), ^() {
            weakSelf.callingLabel.hidden = YES;
            [weakSelf adjustLocalVideoView];
            
            [weakSelf stopRing];
            [weakSelf joinChannel];
        });
    };
    
    signalEngine.onInviteRefusedByPeer = ^(NSString* channelID, NSString *account, uint32_t uid, NSString *extra) {
        NSLog(@"onInviteRefusedByPeer, channel: %@, account: %@, uid: %u, extra: %@", channelID, account, uid, extra);
        if (![channelID isEqualToString:weakSelf.channel] || ![account isEqualToString:weakSelf.remoteAccount]) {
            return;
        }
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf stopRing];
            [weakSelf leaveChannel];
            
            NSData *data = [extra dataUsingEncoding:NSUTF8StringEncoding];
            NSDictionary *dic = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
            if ([dic[@"status"] intValue] == 1) {
                NSString *message = [NSString stringWithFormat:@"%@ is busy", account];
                [AlertUtil showAlert:message completion:^{
                    [weakSelf dismissViewControllerAnimated:NO completion:nil];
                }];
            }
            else {
                [weakSelf dismissViewControllerAnimated:NO completion:nil];
            }
        });
    };
    
    signalEngine.onInviteEndByPeer = ^(NSString* channelID, NSString *account, uint32_t uid, NSString *extra) {
        NSLog(@"onInviteEndByPeer, channel: %@, account: %@, uid: %u, extra: %@", channelID, account, uid, extra);
        if (![channelID isEqualToString:weakSelf.channel] || ![account isEqualToString:weakSelf.remoteAccount]) {
            return;
        }
        
        dispatch_async(dispatch_get_main_queue(), ^() {
            [weakSelf stopRing];
            [weakSelf leaveChannel];
            [weakSelf dismissViewControllerAnimated:NO completion:nil];
        });
    };
}

- (void)sendInviteRequest {
    self.channel = [NSString stringWithFormat:@"%@-%@-%f", self.localAccount, self.remoteAccount, [NSDate date].timeIntervalSinceReferenceDate];
    
    NSDictionary *extraDic = @{@"_require_peer_online": @(1)};
    
    [signalEngine channelInviteUser2:self.channel account:self.remoteAccount extra:[extraDic JSONString]];
}

- (IBAction)muteButtonClicked:(UIButton *)sender {
    if (mediaEngine) {
        [sender setSelected:!sender.isSelected];
        [mediaEngine muteLocalAudioStream:sender.isSelected];
    }
}

- (IBAction)switchCameraButtonClicked:(UIButton *)sender {
    if (mediaEngine) {
        [sender setSelected:!sender.isSelected];
        [mediaEngine switchCamera];
    }
}

- (IBAction)hangupButtonClicked:(UIButton *)sender {
    if (self.acceptButton) {
        // called by other
        NSDictionary *extraDic = @{@"status": @(0)};
        [signalEngine channelInviteRefuse:self.channel account:self.remoteAccount uid:0 extra:[extraDic JSONString]];
        
        [self stopRing];
    }
    else {
        [signalEngine channelInviteEnd:self.channel account:self.remoteAccount uid:0];
        
        if (self.callingLabel.hidden) {
            // already accepted
            [self leaveChannel];
        }
        else {
            // calling other
            [self stopRing];
        }
    }
    
    [self dismissViewControllerAnimated:NO completion:nil];
}

- (IBAction)acceptButtonClicked:(UIButton *)sender {
    [signalEngine channelInviteAccept:self.channel account:self.remoteAccount uid:0 extra:nil];
    
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
    audioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:path error:nil];
    audioPlayer.numberOfLoops = 1;
    [audioPlayer play];
}

- (void)stopRing {
    if (audioPlayer) {
        [audioPlayer stop];
        audioPlayer = nil;
    }
}

- (void)loadMediaEngine {
    mediaEngine = [AgoraRtcEngineKit sharedEngineWithAppId:[KeyCenter appId] delegate:self];
    
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy-MM-dd ah-mm-ss"];
    NSString *logFilePath = [NSString stringWithFormat:@"%@/AgoraRtcEngine %@.log",
                      NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask, YES).firstObject,
                      [dateFormatter stringFromDate:[NSDate date]]];
    [mediaEngine setLogFile:logFilePath];
    //[mediaEngine setParameters:@"{\"rtc.log_filter\":65535}"];
    
    [mediaEngine enableVideo];
    
    AgoraVideoEncoderConfiguration *videoConfiguration = [[AgoraVideoEncoderConfiguration alloc] initWithSize:AgoraVideoDimension640x360
                                                                                                    frameRate:AgoraVideoFrameRateFps15
                                                                                                      bitrate:AgoraVideoBitrateStandard
                                                                                              orientationMode:AgoraVideoOutputOrientationModeAdaptative];
    [mediaEngine setVideoEncoderConfiguration:videoConfiguration];
}

- (void)startLocalVideo {
    AgoraRtcVideoCanvas *videoCanvas = [[AgoraRtcVideoCanvas alloc] init];
    videoCanvas.uid = 0;
    videoCanvas.view = self.localVideo;
    videoCanvas.renderMode = AgoraVideoRenderModeHidden;
    [mediaEngine setupLocalVideo:videoCanvas];
    [mediaEngine startPreview];
}

- (void)adjustLocalVideoView {
    self.localVideo.frame = CGRectMake(self.view.frame.size.width - 110, 20, 90, 160);
    self.localVideo.autoresizingMask = UIViewAutoresizingFlexibleLeftMargin | UIViewAutoresizingFlexibleBottomMargin;
}

- (void)joinChannel {
    NSString *mediaToken = nil;
    int result = [mediaEngine joinChannelByToken:mediaToken channelId:self.channel info:nil uid:self.localUID joinSuccess:nil];
    if (result != AgoraEcode_SUCCESS) {
        NSLog(@"Join channel failed: %d", result);
        
        [signalEngine channelInviteEnd:self.channel account:self.remoteAccount uid:0];
        
        __weak typeof(self) weakSelf = self;
        [AlertUtil showAlert:[NSString stringWithFormat:@"Join channel failed"] completion:^{
            [weakSelf dismissViewControllerAnimated:NO completion:nil];
        }];
    }
}

- (void)leaveChannel {
    if (mediaEngine) {
        [[UIApplication sharedApplication] setIdleTimerDisabled:NO];
        [mediaEngine stopPreview];
        [mediaEngine setupLocalVideo:nil];
        [mediaEngine leaveChannel:nil];
        mediaEngine = nil;
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
    [mediaEngine setupRemoteVideo:videoCanvas];
}

- (void)rtcEngine:(AgoraRtcEngineKit *)engine didOfflineOfUid:(NSUInteger)uid reason:(AgoraUserOfflineReason)reason {
    NSLog(@"rtcEngine:didOfflineOfUid: %ld", (long)uid);
    // only receive this callback if remote user logout unexpected
    [self leaveChannel];
    [self dismissViewControllerAnimated:NO completion:nil];
}

@end
