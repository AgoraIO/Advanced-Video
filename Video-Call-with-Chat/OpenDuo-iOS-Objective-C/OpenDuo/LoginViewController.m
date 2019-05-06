//
//  ViewController.m
//  OpenDuo
//
//  Created by suleyu on 2017/10/31.
//  Copyright © 2017 Agora. All rights reserved.
//

#import "LoginViewController.h"
#import "DialViewController.h"
#import "AlertUtil.h"
#import "KeyCenter.h"
#import <AgoraRtmKit/AgoraRtmKit.h>

@interface LoginViewController ()
@property (weak, nonatomic) IBOutlet UITextField *accountTextField;
@property (strong, nonatomic) AgoraRtmKit *signalEngine;
@end

@implementation LoginViewController
- (AgoraRtmKit *)signalEngine {
    if (!_signalEngine) {
        _signalEngine = [[AgoraRtmKit alloc] initWithAppId:[KeyCenter appId] delegate:nil];
    }
    return _signalEngine;
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    if ([segue.identifier isEqualToString:@"ShowDialView"]) {
        DialViewController *dialVC = segue.destinationViewController;
        dialVC.localAccount = self.accountTextField.text;
        dialVC.signalEngine = self.signalEngine;
    }
}

- (IBAction)loginButtonClicked:(id)sender {
    NSString *account = self.accountTextField.text;
    if (!account.length) {
        return;
    }
    
    [self.accountTextField resignFirstResponder];
    
    __weak typeof(self) weakSelf = self;
    [self.signalEngine loginByToken:nil user:account completion:^(AgoraRtmLoginErrorCode errorCode) {
        NSLog(@"Login completion code: %ld", (long)errorCode);
        if (errorCode == AgoraRtmLoginErrorOk) {
            [weakSelf performSegueWithIdentifier:@"ShowDialView" sender:nil];
        } else {
            [AlertUtil showAlert:@"Login failed"];
        }
    }];
}
@end
