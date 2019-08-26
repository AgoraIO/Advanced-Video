//
//  DialViewController.m
//  OpenDuo
//
//  Created by suleyu on 2017/10/31.
//  Copyright © 2017 Agora. All rights reserved.
//

#import "DialViewController.h"
#import "CallViewController.h"
#import "AlertUtil.h"
#import "NSObject+JSONString.h"
#import <AgoraRtmKit/AgoraRtmKit.h>

@interface DialViewController () <AgoraRtmCallDelegate>
@property (weak, nonatomic) IBOutlet UILabel *lacalAccountLabel;
@property (weak, nonatomic) IBOutlet UILabel *remoteAccountLabel;
@property (strong, nonatomic) AgoraRtmCallKit *callKit;
@end

@implementation DialViewController
- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.callKit = [self.signalEngine getRtmCallKit];
    
    self.navigationItem.hidesBackButton = YES;
    self.lacalAccountLabel.text = [NSString stringWithFormat:@"Your account ID is %@", self.localAccount];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    self.remoteAccountLabel.text = @"";
    self.callKit.callDelegate = self;
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    if (self.isMovingFromParentViewController) {
        self.callKit.callDelegate = nil;
    }
}

- (IBAction)logout:(id)sender {
//    __weak typeof(self) weakSelf = self;
    [self.signalEngine logoutWithCompletion:^(AgoraRtmLogoutErrorCode errorCode) {
        NSLog(@"onLogout, ecode: %ld", (long)errorCode);
//        dispatch_async(dispatch_get_main_queue(), ^{
//            UIViewController *presentedVC = weakSelf.presentedViewController;
//            if (weakSelf.presentedViewController) {
//                [weakSelf dismissViewControllerAnimated:NO completion:nil];
//                if ([presentedVC isMemberOfClass:[CallViewController class]]) {
//                    [(CallViewController *)presentedVC logout];
//                }
//            }
//            [weakSelf.navigationController popViewControllerAnimated:NO];
//        });
    }];
    
    [self.navigationController popViewControllerAnimated:YES];
}

- (IBAction)numberButtonClicked:(UIButton *)sender {
    NSString *remoteAccount = self.remoteAccountLabel.text;
    remoteAccount = [remoteAccount stringByAppendingString:sender.titleLabel.text];
    self.remoteAccountLabel.text = remoteAccount;
}

- (IBAction)callButtonClicked:(UIButton *)sender {
    NSString *remoteAccount = self.remoteAccountLabel.text;
    if (remoteAccount.length > 0) {
        if ([remoteAccount isEqualToString:self.localAccount]) {
            [AlertUtil showAlert:@"Cannot call yourself"];
            self.remoteAccountLabel.text = @"";
        } else {
            [self showCallViewForInvitation:nil remoteAccount:remoteAccount];
        }
    }
}

- (IBAction)deleteButtonClicked:(UIButton *)sender {
    NSString *remoteAccount = self.remoteAccountLabel.text;
    if (remoteAccount.length > 0) {
        remoteAccount = [remoteAccount substringToIndex:remoteAccount.length - 1];
        self.remoteAccountLabel.text = remoteAccount;
    }
}

- (void)showCallViewForInvitation:(AgoraRtmRemoteInvitation* )invitation remoteAccount:(NSString *)account {
    CallViewController *callVC = [self.storyboard instantiateViewControllerWithIdentifier:@"CallViewController"];
    callVC.localAccount = self.localAccount;
    callVC.remoteAccount = account;
    callVC.signalEngine = self.signalEngine;
    callVC.remoteInvitation = invitation;
    [self presentViewController:callVC animated:NO completion:nil];
}

- (void)refuseInvitation:(AgoraRtmRemoteInvitation* )invitation {
    [self.callKit refuseRemoteInvitation:invitation completion:nil];
}

- (void)rtmCallKit:(AgoraRtmCallKit *)callKit remoteInvitationReceived:(AgoraRtmRemoteInvitation *)remoteInvitation {
    NSString *channelId = remoteInvitation.content;
    NSString *callerId = remoteInvitation.callerId;
    NSLog(@"remoteInvitationReceived, channel: %@, uid: %@", channelId, callerId);
    
    if (self.presentedViewController == nil) {
        [self showCallViewForInvitation:remoteInvitation remoteAccount:callerId];
    } else {
        [self refuseInvitation:remoteInvitation];
    }
}
@end
