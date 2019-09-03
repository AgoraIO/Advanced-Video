//
//  MSView.h
//  MediaPlayerKitQuickstart
//
//  Created by zhanxiaochao on 2019/8/13.
//  Copyright © 2019 Agora. All rights reserved.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN
typedef enum MOUSE_EVENT_STATE{
    MOUSE_EVENT_STATE_ENTERED,
    MOUSE_EVENT_STATE_EXIT,
    MOUSE_EVENT_STATE_DOWN,
}MOUSE_EVENT_STATE;
@interface MSView : NSView
@property(nonatomic,strong)void (^mouseEventCallBack)(MOUSE_EVENT_STATE state);
@end

NS_ASSUME_NONNULL_END
