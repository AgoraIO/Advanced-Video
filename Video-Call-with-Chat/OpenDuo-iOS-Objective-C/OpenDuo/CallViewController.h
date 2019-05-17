//
//  CallViewController.h
//  OpenDuo
//
//  Created by suleyu on 2017/10/31.
//  Copyright © 2017 Agora. All rights reserved.
//

#import <UIKit/UIKit.h>

@class AgoraRtmKit;
@class AgoraRtmRemoteInvitation;

@interface CallViewController : UIViewController
@property (copy, nonatomic) NSString *localAccount;
@property (copy, nonatomic) NSString *remoteAccount;
@property (strong, nonatomic) AgoraRtmKit *signalEngine;
@property (strong, nonatomic) AgoraRtmRemoteInvitation *remoteInvitation;

- (void)logout;

@end
