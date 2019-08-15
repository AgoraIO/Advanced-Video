//
//  CustomSlider.m
//  CustomSlider
//
//  Created by Chen Ling on 15/3/2018.
//  Copyright © 2018 Chen Ling. All rights reserved.
//

#import "CustomSlider.h"

@interface CustomSlider()

@property (nonatomic, strong) NSTrackingArea *trackingArea;

@end

@implementation CustomSlider

- (instancetype)initWithCoder:(NSCoder *)coder {
    if (self = [super initWithCoder:coder]) {
        // init code here
    }
    return self;
}


#pragma mark - 设置mouse追踪区域
-(void)updateTrackingAreas
{
    [super updateTrackingAreas];
    if(_trackingArea != nil) {
        [self removeTrackingArea:_trackingArea];
        
    }
    
    int opts = (NSTrackingMouseEnteredAndExited | NSTrackingActiveAlways);
    
    // 将设置追踪区域为控件大小
    // 设置鼠标追踪区域，如果不设置追踪区域，mouseEntered和mouseExited会无效
    _trackingArea = [ [NSTrackingArea alloc] initWithRect:[self bounds]
                                                  options:opts
                                                    owner:self
                                                 userInfo:nil];
    [self addTrackingArea:_trackingArea];
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    // Drawing code here.
}

#pragma mark - 点击knob效果, 屏蔽就没有点下去的效果
- (BOOL)needsPanelToBecomeKey{
    [super needsPanelToBecomeKey];
    return YES;
}

//- (BOOL)becomeFirstResponder{
//    [super becomeFirstResponder];
//    return YES;
//}

#pragma mark - mouse action
- (void)mouseEntered:(NSEvent *)theEvent
{
    [super mouseEntered:theEvent];
    NSLog(@"mouseEntered");
    self.highlighted = YES;
}

- (void)mouseExited:(NSEvent *)theEvent
{
    [super mouseExited:theEvent];
    NSLog(@"mouseExited");
    self.highlighted = NO;
}


@end
