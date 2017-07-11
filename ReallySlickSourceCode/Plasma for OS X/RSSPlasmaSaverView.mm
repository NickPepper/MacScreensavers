// Mac OS X Code
// 
// First version: 28/06/02 Stéphane Sudre

#import "RSSPlasmaSaverView.h"
#include "Plasma.h"

@implementation RSSPlasmaSaverView

- (id)initWithFrame:(NSRect)frameRect isPreview:(BOOL) preview
{
    NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];
    id tObject;
    
    self = [super initWithFrame:frameRect isPreview:preview];
        
    preview_=preview;
    
    isConfiguring_=NO;
    
    if (preview_==YES)
    {
        mainScreen_=YES;
    }
    else
    {
        mainScreen_= (frameRect.origin.x==0 && frameRect.origin.y==0) ? YES : NO;
    }
    
    mainScreenOnly_=[defaults integerForKey:@"MainScreen Only"];
    
    if (self)
    {
        
        if (mainScreenOnly_!=NSOnState || mainScreen_==YES)
        {
            NSOpenGLPixelFormatAttribute attribs[] = 
            {
                NSOpenGLPFADoubleBuffer,
                NSOpenGLPFAMinimumPolicy,
                (NSOpenGLPixelFormatAttribute)0
            };
        
            NSOpenGLPixelFormat *format = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attribs] autorelease];
            
            if (format!=nil)
            {
                _view = [[[NSOpenGLView alloc] initWithFrame:NSZeroRect pixelFormat:format] autorelease];
                [self addSubview:_view];
            
                settings_.position=NULL;
                settings_.plasma=NULL;
                settings_.plasmamap=NULL;
                
                tObject=[defaults objectForKey:@"Resolution"];
            
                if (tObject==nil)
                {
                    setDefaults(&settings_);
                }
                else
                {
                    [self readDefaults:defaults];
                }
                
                [self setAnimationTimeInterval:0.04];
            }
        }
    }
    
    return self;
}

- (void) setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
    
	if (_view!=nil)
    {
        [_view setFrameSize:newSize];
    }
}

- (void) drawRect:(NSRect) inFrame
{
	[[NSColor blackColor] set];
            
    NSRectFill(inFrame);
    
    if (_view==nil)
    {    
        if (mainScreenOnly_!=NSOnState || mainScreen_==YES)
        {
            NSRect tFrame=[self frame];
            NSRect tStringFrame;
            NSDictionary * tAttributes;
            NSString * tString;
            NSMutableParagraphStyle * tParagraphStyle;
            
            tParagraphStyle=[[NSParagraphStyle defaultParagraphStyle] mutableCopy];
            [tParagraphStyle setAlignment:NSCenterTextAlignment];
            
            tAttributes = [NSDictionary dictionaryWithObjectsAndKeys:[NSFont systemFontOfSize:[NSFont systemFontSize]],NSFontAttributeName,[NSColor whiteColor],NSForegroundColorAttributeName,tParagraphStyle,NSParagraphStyleAttributeName,nil];
            
            [tParagraphStyle release];
            
            tString=NSLocalizedStringFromTableInBundle(@"Minimum OpenGL requirements\rfor this Screen Effect\rnot available\ron your graphic card.",@"Localizable",[NSBundle bundleForClass:[self class]],@"No comment");
            
            tStringFrame.origin=NSZeroPoint;
            tStringFrame.size=[tString sizeWithAttributes:tAttributes];
            
            tStringFrame=SSCenteredRectInRect(tStringFrame,tFrame);
            
            [tString drawInRect:tStringFrame withAttributes:tAttributes];
            
            return;
        }
    }
}

- (void)animateOneFrame
{
    if (isConfiguring_==NO && _view!=nil)
    {
        if (mainScreenOnly_!=NSOnState || mainScreen_==YES)
        {
            [[_view openGLContext] makeCurrentContext];
            
            draw(&settings_);
            
            [[_view openGLContext] flushBuffer];
        }
    }
}

- (void)startAnimation
{
    [super startAnimation];
    
    if (isConfiguring_==NO && _view!=nil)
    {
        if (mainScreenOnly_!=NSOnState || mainScreen_==YES)
        {
            NSSize tSize;
            
            [self lockFocus];
            [[_view openGLContext] makeCurrentContext];
            
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT);
            
            [[_view openGLContext] flushBuffer];
            
            tSize=[_view frame].size;
            
            initSaver((int) tSize.width,(int) tSize.height,&settings_);
            
            [self unlockFocus];
        }
    }
}

- (void)stopAnimation
{
    [super stopAnimation];
    
    if (_view!=nil)
    {
        if (mainScreenOnly_!=NSOnState || mainScreen_==YES)
        {
            [[_view openGLContext] makeCurrentContext];
            
            cleanSettings(&settings_);
        }
    }
}

- (BOOL) hasConfigureSheet
{
    return (_view!=nil);
}

- (void) readDefaults:(ScreenSaverDefaults *) inDefaults
{
    settings_.dZoom=[inDefaults integerForKey:@"Zoom"];
    
    settings_.dFocus=[inDefaults integerForKey:@"Focus"];
    
    settings_.dSpeed=[inDefaults integerForKey:@"Speed"];
    
    settings_.dResolution=[inDefaults integerForKey:@"Resolution"];
    
    mainScreenOnly_=[inDefaults integerForKey:@"MainScreen Only"];
}

- (void) writeDefaults
{
    NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults *inDefaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];
    
    mainScreenOnly_=[IBmainScreen_ state];
    
    [inDefaults setInteger:settings_.dZoom forKey:@"Zoom"];
    
    [inDefaults setInteger:settings_.dFocus forKey:@"Focus"];
    
    [inDefaults setInteger:settings_.dSpeed forKey:@"Speed"];
    
    [inDefaults setInteger:settings_.dResolution forKey:@"Resolution"];
    
    [inDefaults setInteger:mainScreenOnly_ forKey:@"MainScreen Only"];
    
    [inDefaults  synchronize];
}

- (void) setDialogValue
{
    [IBmagnificationSlider_ setIntValue:settings_.dZoom];
    [IBmagnificationText_ setIntValue:settings_.dZoom];
    
    [IBfocusSlider_ setIntValue:settings_.dFocus];
    [IBfocusText_ setIntValue:settings_.dFocus];
    
    [IBspeedSlider_ setIntValue:settings_.dSpeed];
    [IBspeedText_ setIntValue:settings_.dSpeed];
    
    [IBresolutionSlider_ setIntValue:settings_.dResolution];
    [IBresolutionText_ setIntValue:settings_.dResolution];
    
    [IBmainScreen_ setState:mainScreenOnly_];
}

- (NSWindow*)configureSheet
{
    isConfiguring_=YES;
    
    if (IBconfigureSheet_ == nil)
    {
        [NSBundle loadNibNamed:@"ConfigureSheet" owner:self];
        
        [IBversion_ setStringValue:[[[NSBundle bundleForClass:[self class]] infoDictionary] objectForKey:@"CFBundleVersion"]];
    }
    
    [self setDialogValue];
    
    return IBconfigureSheet_;
}

- (IBAction)reset:(id)sender
{
    setDefaults(&settings_);
    
    [self setDialogValue];
}

- (IBAction)closeSheet:(id)sender
{
    if ([sender tag]==NSOKButton)
    {
        [self writeDefaults];
    }
    else
    {
        NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
        ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];    
        id tObject;
        
        tObject=[defaults objectForKey:@"Resolution"];
        
        if (tObject==nil)
        {
            setDefaults(&settings_);
        }
        else
        {
            [self readDefaults:defaults];
        }

    }
    
    isConfiguring_=NO;
    
    if ([self isAnimating]==YES)
    {
        [self stopAnimation];
        [self startAnimation];
    }
    
    [NSApp endSheet:IBconfigureSheet_];
}


- (IBAction)setSpeed:(id)sender
{
    settings_.dSpeed=[sender intValue];
    
    [IBspeedText_ setIntValue:settings_.dSpeed];
}

- (IBAction)setFocus:(id)sender
{
    settings_.dFocus=[sender intValue];
    
    [IBfocusText_ setIntValue:settings_.dFocus];
}

- (IBAction)setMagnification:(id)sender
{
    settings_.dZoom=[sender intValue];
    
    [IBmagnificationText_ setIntValue:settings_.dZoom];
}

- (IBAction)setResolution:(id)sender
{
    settings_.dResolution=[sender intValue];
    
    [IBresolutionText_ setIntValue:settings_.dResolution];
}

@end
