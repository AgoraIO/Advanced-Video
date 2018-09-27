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
#import "KeyCenter.h"
#import <AgoraSigKit/AgoraSigKit.h>

@interface DialViewController ()
{
    AgoraAPI *signalEngine;
}

@property (weak, nonatomic) IBOutlet UILabel *lacalAccountLabel;
@property (weak, nonatomic) IBOutlet UILabel *remoteAccountLabel;

@end

@implementation DialViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.navigationItem.hidesBackButton = YES;
    self.lacalAccountLabel.text = [NSString stringWithFormat:@"Your account ID is %@", self.localAccount];
    
    signalEngine = [AgoraAPI getInstanceWithoutMedia:[KeyCenter appId]];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    self.remoteAccountLabel.text = @"";
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    __weak typeof(self) weakSelf = self;
    
    signalEngine.onLogout = ^(AgoraEcode ecode) {
        NSLog(@"onLogout, ecode: %lu", (unsigned long)ecode);
        dispatch_async(dispatch_get_main_queue(), ^{
            UIViewController *presentedVC = weakSelf.presentedViewController;
            if (weakSelf.presentedViewController) {
                [weakSelf dismissViewControllerAnimated:NO completion:nil];
                if ([presentedVC isMemberOfClass:[CallViewController class]]) {
                    [(CallViewController *)presentedVC logout];
                }
            }
            [weakSelf.navigationController popViewControllerAnimated:NO];
        });
    };

    signalEngine.onInviteReceived = ^(NSString* channelID, NSString *account, uint32_t uid, NSString *extra) {
        NSLog(@"onInviteReceived, channel: %@, account: %@, uid: %u, extra: %@", channelID, account, uid, extra);
        
        dispatch_async(dispatch_get_main_queue(), ^{
            if (weakSelf.presentedViewController == nil) {
                [weakSelf showCallView:channelID remoteAccount:account];
            }
            else {
                [weakSelf refuseInvite:channelID fromAccount:account];
            }
        });
    };
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    if (self.isMovingFromParentViewController) {
        signalEngine.onLogout = nil;
        signalEngine.onInviteReceived = nil;
    }
}

- (IBAction)logout:(id)sender {
    [signalEngine logout];
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
        }
        else {
            [self showCallView:nil remoteAccount:remoteAccount];
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

- (void)showCallView:(NSString* )channel remoteAccount:(NSString *)account {
    CallViewController *callVC = [self.storyboard instantiateViewControllerWithIdentifier:@"CallViewController"];
    callVC.localUID = self.localUID;
    callVC.localAccount = self.localAccount;
    callVC.remoteAccount = account;
    callVC.channel = channel;
    [self presentViewController:callVC animated:NO completion:nil];
}

- (void)refuseInvite:(NSString* )channel fromAccount:(NSString *)account {
    NSDictionary *extraDic = @{@"status": @(1)};
    [signalEngine channelInviteRefuse:channel account:account uid:0 extra:[extraDic JSONString]];
}

@end
