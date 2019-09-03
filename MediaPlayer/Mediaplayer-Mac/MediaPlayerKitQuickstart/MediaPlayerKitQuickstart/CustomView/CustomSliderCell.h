//
//  CustomSliderCell.h
//  CustomSlider
//
//  Created by Chen Ling on 15/3/2018.
//  Copyright © 2018 Chen Ling. All rights reserved.

#import <Cocoa/Cocoa.h>


#define ColorWithRGB(colorCode)     [NSColor colorWithDeviceRed:((colorCode>>16)&0xFF)/255.0 green:((colorCode>>8)&0xFF)/255.0 blue:((colorCode)&0xFF)/255.0 alpha:1.0];

#define SLIDER_PROGRESS_DEFAUT_COLOR       ColorWithRGB(0x3399FF)
#define SLIDER_BACKGROUND_DEFAUT_COLOR     ColorWithRGB(0x969696)
#define SLIDER_KNOB_DEFAUT_COLOR           ColorWithRGB(0x3399FF)
#define SLIDER_DEFAUT_HEIGHT               8.0
#define SLIDER_DEFAUT_BAR_RADIUS           0.0
#define SLIDER_DEFAUT_KNOB_WIDTH           8.0
#define SLIDER_DEFAUT_KNOB_HEIGHT          8.0


IB_DESIGNABLE
@interface CustomSliderCell : NSSliderCell

@property (nonatomic, strong) IBInspectable NSColor *sliderProgressColor;
@property (nonatomic, strong) IBInspectable NSColor *sliderBackgroundColor;
@property (nonatomic, strong) IBInspectable NSColor *sliderKnobColor;
@property (nonatomic, assign) IBInspectable CGFloat sliderHeight;
@property (nonatomic, assign) IBInspectable CGFloat sliderBarRadius;
@property (nonatomic, assign) IBInspectable CGFloat sliderKnobWidth;
@property (nonatomic, assign) IBInspectable CGFloat sliderKnobHeight;
@end
