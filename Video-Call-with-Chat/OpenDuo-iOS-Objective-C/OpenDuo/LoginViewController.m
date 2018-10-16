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
#import <AgoraSigKit/AgoraSigKit.h>

@interface LoginViewController ()
{
    AgoraAPI *signalEngine;
}

@property (weak, nonatomic) IBOutlet UITextField *accountTextField;

@end

@implementation LoginViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    signalEngine = [AgoraAPI getInstanceWithoutMedia:[KeyCenter appId]];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    __weak typeof(self) weakSelf = self;
    
    signalEngine.onLoginSuccess = ^(uint32_t uid, int fd) {
        NSLog(@"Login successfully, uid: %u", uid);
        dispatch_async(dispatch_get_main_queue(), ^{
            [weakSelf performSegueWithIdentifier:@"ShowDialView" sender:@(uid)];
        });
    };
    
    signalEngine.onLoginFailed = ^(AgoraEcode ecode) {
        NSLog(@"Login failed, error: %lu", (unsigned long)ecode);
        dispatch_async(dispatch_get_main_queue(), ^{
            [AlertUtil showAlert:@"Login failed"];
        });
    };
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    signalEngine.onLoginSuccess = nil;
    signalEngine.onLoginFailed = nil;
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    if ([segue.identifier isEqualToString:@"ShowDialView"]) {
        DialViewController *dialVC = segue.destinationViewController;
        dialVC.localUID = [sender unsignedIntValue];
        dialVC.localAccount = self.accountTextField.text;
    }
}

- (IBAction)loginButtonClicked:(id)sender {
    NSString *account = self.accountTextField.text;
    if (account.length > 0) {
        [self.accountTextField resignFirstResponder];
        
        NSString *sigToken = @"_no_need_token";
        [signalEngine login:[KeyCenter appId] account:account token:sigToken uid:0 deviceID:nil];
    }
}

@end
