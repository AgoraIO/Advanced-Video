//
//  AlertUtil.h
//  OpenDuo
//
//  Created by suleyu on 2017/11/1.
//  Copyright © 2017 Agora. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface AlertUtil : NSObject

+ (void)showAlert:(NSString *)message;

+ (void)showAlert:(NSString *)message completion:(void (^)(void))completion;

@end
