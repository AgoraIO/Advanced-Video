//
//  MSView.m
//  MediaPlayerKitQuickstart
//
//  Created by zhanxiaochao on 2019/8/13.
//  Copyright © 2019 Agora. All rights reserved.
//

#import "MSView.h"
@interface MSView ()
@property (nonatomic, strong)NSTrackingArea *trackingArea;
@end
@implementation MSView

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    NSLog(@"frame：%@ , dirtyRect ： %@",NSStringFromRect([self frame]),NSStringFromRect(dirtyRect));
    //frame：{{100, 100}, {300, 300}} , dirtyRect ： {{0, 0}, {300, 300}}
    
    self.trackingArea = [[NSTrackingArea alloc] initWithRect:dirtyRect
                                                     options:NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved |
                         NSTrackingCursorUpdate |
                         NSTrackingActiveWhenFirstResponder |
                         NSTrackingActiveInKeyWindow |
                         NSTrackingActiveInActiveApp |
                         NSTrackingActiveAlways |
                         NSTrackingAssumeInside |
                         NSTrackingInVisibleRect |
                         NSTrackingEnabledDuringMouseDrag
                                                       owner:self
                                                    userInfo:nil];
    
    [self addTrackingArea:self.trackingArea];
    [self becomeFirstResponder];

    // Drawing code here.
}
//鼠标进入追踪区域
- (void)mouseEntered:(NSEvent *)event{
       NSLog(@"mouseEntered ========== ");
    if (self.mouseEventCallBack) {
     
        self.mouseEventCallBack(MOUSE_EVENT_STATE_ENTERED);
    }
}
//鼠标退出追踪区域
- (void)mouseExited:(NSEvent *)event{
    
       NSLog(@"mouseExited ---------- ");
    if ( self.mouseEventCallBack) {
        self.mouseEventCallBack(MOUSE_EVENT_STATE_EXIT);

    }

    
}
//鼠标右键按下
- (void)mouseDown:(NSEvent *)event{
    NSLog(@"rightMouseDown ========== ");
    if ( self.mouseEventCallBack) {
        
        self.mouseEventCallBack(MOUSE_EVENT_STATE_DOWN);
    }

}


@end
