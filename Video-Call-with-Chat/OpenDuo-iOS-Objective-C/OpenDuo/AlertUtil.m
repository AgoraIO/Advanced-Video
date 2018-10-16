//
//  AlertUtil.m
//  OpenDuo
//
//  Created by suleyu on 2017/11/1.
//  Copyright © 2017 Agora. All rights reserved.
//

#import "AlertUtil.h"
#import <UIKit/UIKit.h>

@implementation AlertUtil

+ (void)showAlert:(NSString *)message
{
    [AlertUtil showAlert:message completion:nil];
}

+ (void)showAlert:(NSString *)message completion:(void (^)(void))completion
{
    UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil message:message preferredStyle:UIAlertControllerStyleAlert];
    UIAlertAction *okAction = [UIAlertAction actionWithTitle:@"OK" style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
        if (completion) {
            completion();
        }
    }];
    [alertController addAction:okAction];
    
    UIViewController *rootVC = [UIApplication sharedApplication].keyWindow.rootViewController;
    UIViewController *topVC = rootVC.presentedViewController ? rootVC.presentedViewController : rootVC;
    [topVC presentViewController:alertController animated:YES completion:nil];
}

@end
