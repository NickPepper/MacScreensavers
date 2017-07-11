//
//  PepperPhrasesSSView.m
//  PepperPhrasesSS
//
//  Created by Nick Pershin on 01/04/17.
//  Copyright © 2017 Nick Pershin. All rights reserved.
//

#import "PepperPhrasesSSView.h"

@implementation PepperPhrasesSSView


- (instancetype)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    self = [super initWithFrame:frame isPreview:isPreview];
    if (self) {
        //[self setAnimationTimeInterval:1/30.0];
        [self setAnimationTimeInterval:1];
    }
    return self;
}


- (void)startAnimation
{
    [super startAnimation];
}


- (void)stopAnimation
{
    [super stopAnimation];
}


- (void)drawRect:(NSRect)rect
{
    [super drawRect:rect];
}


- (void)animateOneFrame
{
    NSRect rect;
    NSColor *randomColor;
    NSMutableParagraphStyle* textStyle;
    NSDictionary* textFontAttributes;
    NSString *text;
    //NSSize size = [self bounds].size;
    float red, green, blue, alpha;
    int randomFontSize, randomString;
    
    rect.size = NSMakeSize(330.0, 100.0);
    
    // Calculate random origin point
    rect.origin = SSRandomPointForSizeWithinRect(rect.size, [self bounds]);
    
    textStyle = NSMutableParagraphStyle.defaultParagraphStyle.mutableCopy;
    textStyle.alignment = NSTextAlignmentLeft;
    
    // Calculate random color
    red     = SSRandomFloatBetween( 0.0, 255.0 ) / 255.0;
    green   = SSRandomFloatBetween( 0.0, 255.0 ) / 255.0;
    blue    = SSRandomFloatBetween( 0.0, 255.0 ) / 255.0;
    alpha   = 1.0;//SSRandomFloatBetween( 0.0, 255.0 ) / 255.0;
    
    randomColor = [NSColor colorWithCalibratedRed:red
                                            green:green
                                             blue:blue
                                            alpha:alpha];
    //[randomColor set];

    // Calculate random font size
    randomFontSize = SSRandomIntBetween( 8, 32 );
    
    textFontAttributes = @{NSFontAttributeName: [NSFont fontWithName: @"Helvetica" size: randomFontSize], NSForegroundColorAttributeName: randomColor, NSParagraphStyleAttributeName: textStyle};
    
    // Set random string
    randomString = SSRandomIntBetween( 0, 3 );
    
    switch (randomString)
    {
        case 0:
            text = @"Жди меня, и я вернусь!";
            break;
            
        case 1:
            text = @"От работы кони дохнут";
            break;
        
        case 2:
            text = @"Работа - не волк";
            break;
            
        case 3:
            text = @"Курить - здоровью вредить";
            break;
        
        default:
            text = @"Он работал как можно быстрее, и его депрессия всё нарастала. © Алан Дин Фостер, \"Програмерзость\"";
            break;
    }
    
    [text drawInRect: rect withAttributes: textFontAttributes];
}


- (BOOL)hasConfigureSheet
{
    return NO;
}


- (NSWindow*)configureSheet
{
    return nil;
}

@end
