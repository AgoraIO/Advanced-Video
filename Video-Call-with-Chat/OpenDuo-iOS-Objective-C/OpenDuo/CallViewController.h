//
//  CallViewController.h
//  OpenDuo
//
//  Created by suleyu on 2017/10/31.
//  Copyright © 2017 Agora. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface CallViewController : UIViewController

@property (assign, nonatomic) unsigned localUID;
@property (copy, nonatomic) NSString *localAccount;
@property (copy, nonatomic) NSString *remoteAccount;
@property (copy, nonatomic) NSString *channel;

- (void)logout;

@end
