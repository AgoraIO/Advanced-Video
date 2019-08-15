//
//  CustomSliderCell.m
//  CustomSlider
//
//  Created by Chen Ling on 15/3/2018.
//  Copyright © 2018 Chen Ling. All rights reserved.
//

#import "CustomSliderCell.h"
#import "NSColor+Hexa.h"
#import "CustomSlider.h"

@interface CustomSliderCell()

// save leftBar's frame, and use to caculate Knob's frame
@property (assign) NSRect leftBarRect;

@end

@implementation CustomSliderCell


-(instancetype)initWithCoder:(NSCoder *)coder {
    if (self = [super initWithCoder:coder]) {
        self.sliderProgressColor = SLIDER_PROGRESS_DEFAUT_COLOR;
        self.sliderBackgroundColor = SLIDER_BACKGROUND_DEFAUT_COLOR;
        self.sliderKnobColor = SLIDER_KNOB_DEFAUT_COLOR;
        self.sliderHeight = SLIDER_DEFAUT_HEIGHT;
        self.sliderBarRadius = SLIDER_DEFAUT_BAR_RADIUS;
        self.sliderKnobWidth = SLIDER_DEFAUT_KNOB_WIDTH;
        self.sliderKnobHeight = SLIDER_DEFAUT_KNOB_HEIGHT;
    }
    return self;
}

- (void)drawBarInside:(NSRect)rect flipped:(BOOL)flipped {
    rect.size.height = self.sliderHeight;
    // Bar radius
    CGFloat barRadius = self.sliderBarRadius;
    // Knob position depending on control min/max value and current control value.
    CGFloat value = ([self doubleValue]  - [self minValue]) / ([self maxValue] - [self minValue]);
    // Final Left Part Width
    CGFloat finalWidth = value * ([[self controlView] frame].size.width - self.sliderKnobWidth);
    
    // Left Part Rect
    NSRect leftRect = rect;
    leftRect.size.width = finalWidth;
    self.leftBarRect = leftRect;
    
    // Draw Left Part
    NSBezierPath* bg = [NSBezierPath bezierPathWithRoundedRect: rect xRadius: barRadius yRadius: barRadius];
    [self.sliderBackgroundColor setFill];
    [bg fill];
    
    // Draw Right Part
    NSBezierPath* active = [NSBezierPath bezierPathWithRoundedRect: leftRect xRadius: barRadius yRadius: barRadius];
    [self.sliderProgressColor setFill];
    [active fill];
    
}

- (void)drawKnob:(NSRect)knobRect {
    
    if (((CustomSlider *)self.controlView).highlighted) {
        NSRect customKnobRect = NSMakeRect(_leftBarRect.size.width, _leftBarRect.origin.y + _leftBarRect.size.height / 2 - self.sliderKnobHeight / 2, self.sliderKnobWidth, self.sliderKnobHeight);
    
        // Draw Left Part
        NSBezierPath* bg = [NSBezierPath bezierPathWithRoundedRect: customKnobRect xRadius: self.sliderKnobWidth / 2 yRadius: self.sliderKnobHeight / 2];
        [self.sliderKnobColor  setFill];
        [bg fill];
    }

}


@end
