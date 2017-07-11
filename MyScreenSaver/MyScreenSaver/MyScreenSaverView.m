//
//  MyScreenSaverView.m
//  MyScreenSaver
//
//  Created by Nick Pershin on 28/03/17.
//  Copyright © 2017 Nick Pershin. All rights reserved.
//

#import "MyScreenSaverView.h"

@implementation MyScreenSaverView


- (instancetype)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    self = [super initWithFrame:frame isPreview:isPreview];
    if (self) {
        //[self setAnimationTimeInterval:1/30.0];
        [self setAnimationTimeInterval:1.0];
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
    NSBezierPath *path;
    NSRect rect;
    NSSize size;
    NSColor *color;
    float red, green, blue, alpha;
    int shapeType;
    
    size = [self bounds].size;
    
    // Calculate random width and height
    rect.size = NSMakeSize(SSRandomFloatBetween(size.width / 100.0, size.width / 10.0),
                           SSRandomFloatBetween(size.height / 100.0, size.height / 10.0));
    
    // Calculate random origin point
    rect.origin = SSRandomPointForSizeWithinRect(rect.size, [self bounds]);
    
    // Decide what kind of shape to draw
    shapeType = SSRandomIntBetween( 0, 2 );
    
    switch (shapeType)
    {
        case 0: // rect
            path = [NSBezierPath bezierPathWithRect:rect];
            break;
            
        case 1: // oval
            path = [NSBezierPath bezierPathWithOvalInRect:rect];
            break;
        
        case 2: // arc
        default:
        {
            float startAngle, endAngle, radius;
            NSPoint point;
            
            startAngle  = SSRandomFloatBetween( 0.0, 360.0 );
            endAngle    = SSRandomFloatBetween(startAngle, 360.0 + startAngle);
            
            // Use the smallest value for the radius (either width or height)
            radius = rect.size.width <= rect.size.height ? rect.size.width / 2 : rect.size.height / 2;
            
            // Calculate our center point
            point = NSMakePoint(rect.origin.x + rect.size.width  / 2,
                                rect.origin.y + rect.size.height / 2);
            
            // Construct the path
            path = [NSBezierPath bezierPath];
            
            [path appendBezierPathWithArcWithCenter: point
                                             radius: radius
                                         startAngle: startAngle
                                           endAngle: endAngle
                                          clockwise: SSRandomIntBetween( 0, 1 )];
        }
            break;
    }
    
    // Calculate a random color
    red     = SSRandomFloatBetween( 0.0, 255.0 ) / 255.0;
    green   = SSRandomFloatBetween( 0.0, 255.0 ) / 255.0;
    blue    = SSRandomFloatBetween( 0.0, 255.0 ) / 255.0;
    alpha   = SSRandomFloatBetween( 0.0, 255.0 ) / 255.0;
    
    color = [NSColor colorWithCalibratedRed:red
                                      green:green
                                       blue:blue
                                      alpha:alpha];
    
    [color set];
    
    // And finally draw it
    if (SSRandomIntBetween( 0, 1 ) == 0) {
        [path fill];
    } else {
        [path stroke];
    }
}


- (BOOL)hasConfigureSheet
{
    return NO;
}

@end
