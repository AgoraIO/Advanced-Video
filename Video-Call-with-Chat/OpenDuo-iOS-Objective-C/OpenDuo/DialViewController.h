//
//  DialViewController.h
//  OpenDuo
//
//  Created by suleyu on 2017/10/31.
//  Copyright © 2017 Agora. All rights reserved.
//

#import <UIKit/UIKit.h>

@class AgoraRtmKit;

@interface DialViewController : UIViewController
@property (copy, nonatomic) NSString *localAccount;
@property (strong, nonatomic) AgoraRtmKit *signalEngine;

@end
