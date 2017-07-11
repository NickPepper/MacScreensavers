// Mac OS X Code
// 
// First version: 28/06/02 Stéphane Sudre

#import "RSSEuphoriaSaverView.h"
#include "Euphoria.h"
#include "resource.h"
#include <sys/time.h>

@implementation RSSEuphoriaSaverView

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
            
                settings_.wisps=NULL;
                settings_.backwisps=NULL;
                
                settings_.feedbackmap=NULL;
                
                settings_.elapsedTime=0;
                
                settings_.tex=0;
                settings_.feedbacktex=0;
                
                tObject=[defaults objectForKey:@"Standard set"];
            
                if (tObject==nil)
                {
                    settings_.standardSet=DEFAULTS1;
                    
                    setDefaults(settings_.standardSet,&settings_);
                }
                else
                {
                    [self readDefaults:defaults];
                    
                    if (settings_.standardSet==1042)
                    {
                        settings_.standardSet=DEFAULTS8;
                    }
                    else
                    if (settings_.standardSet==1047)
                    {
                        settings_.standardSet=DEFAULTS9;
                    }
                }
                
                [self setAnimationTimeInterval:0.03];
            }
        }
    }
    
    return self;
}

- (BOOL)validateMenuItem:(NSMenuItem *)anItem
{
    
    if ([anItem action]==@selector(print:))
    {
        return NO;
    }
    
    return YES;
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
            struct timeval tTime;
            unsigned long long tCurentTime;
            
            [[_view openGLContext] makeCurrentContext];
            
            gettimeofday(&tTime, NULL);
            
            tCurentTime=(tTime.tv_sec*1000+tTime.tv_usec/1000);
            
            settings_.elapsedTime=(tCurentTime-tLastTime)*0.001;
            
            tLastTime=tCurentTime;
            
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
            struct timeval tTime;
    
            [self lockFocus];
            [[_view openGLContext] makeCurrentContext];
            
            glClearColor(0.0, 0.0, 0.0, 0.0);
            glClear(GL_COLOR_BUFFER_BIT);
            
            [[_view openGLContext] flushBuffer];
            
            tSize=[_view frame].size;
            
            
            if (settings_.standardSet==DEFAULTSRANDOM)
            {
                int tRandomSettings;
                
                tRandomSettings=SSRandomIntBetween(DEFAULTS1,DEFAULTS9);
                
                setDefaults(tRandomSettings,&settings_);
            }
            
            initSaver((int) tSize.width,(int) tSize.height,&settings_);
            
            [self unlockFocus];
            
            gettimeofday(&tTime, NULL);
            
            tLastTime=(tTime.tv_sec*1000+tTime.tv_usec/1000);
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
                    
            settings_.elapsedTime=0;
                
            settings_.tex=0;
            settings_.feedbacktex=0;
        }
    }
}

- (BOOL) hasConfigureSheet
{
    return (_view!=nil);
}

- (void) readDefaults:(ScreenSaverDefaults *) inDefaults
{
    settings_.standardSet=[inDefaults integerForKey:@"Standard set"];
    
    settings_.dWisps=[inDefaults integerForKey:@"Wisps count"];
    
    settings_.dBackground=[inDefaults integerForKey:@"Background count"];
    
    settings_.dDensity=[inDefaults integerForKey:@"Density"];
    
    settings_.dFeedback=[inDefaults integerForKey:@"Feedback"];
    
    settings_.dFeedbackspeed=[inDefaults integerForKey:@"Feedback speed"];
    
    settings_.dSpeed=[inDefaults integerForKey:@"Speed"];
    
    settings_.dFeedbacksize=[inDefaults integerForKey:@"Feedback size"];
    
    settings_.dVisibility=[inDefaults integerForKey:@"Visibility"];
    
    settings_.dTexture=[inDefaults integerForKey:@"Texture"];
    
    settings_.dWireframe=[inDefaults integerForKey:@"Wireframe"];
}

- (void) writeDefaults
{
    NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults *inDefaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];
    
    mainScreenOnly_=[IBmainScreen_ state];
    
    settings_.dWireframe=([IBwireframe_ state] == NSOnState) ? 1 :0;
    
    [inDefaults setInteger:settings_.standardSet forKey:@"Standard set"];
    
    [inDefaults setInteger:settings_.dWisps forKey:@"Wisps count"];
    
    [inDefaults setInteger:settings_.dBackground forKey:@"Background count"];
    
    [inDefaults setInteger:settings_.dDensity forKey:@"Density"];
    
    [inDefaults setInteger:settings_.dFeedback forKey:@"Feedback"];
    
    [inDefaults setInteger:settings_.dFeedbackspeed forKey:@"Feedback speed"];
    
    [inDefaults setInteger:settings_.dSpeed forKey:@"Speed"];
    
    [inDefaults setInteger:settings_.dFeedbacksize forKey:@"Feedback size"];
    
    [inDefaults setInteger:settings_.dVisibility forKey:@"Visibility"];
    
    [inDefaults setInteger:settings_.dTexture forKey:@"Texture"];
    
    [inDefaults setInteger:settings_.dWireframe forKey:@"Wireframe"];
    
    [inDefaults setInteger:mainScreenOnly_ forKey:@"MainScreen Only"];
    
    [inDefaults  synchronize];
}

- (void) setDialogValue
{
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
    
    [IBwispsTextField_ setIntValue:settings_.dWisps];
    [IBwispsStepper_ setIntValue:settings_.dWisps];
    
    [IBbackgroundTextField_ setIntValue:settings_.dBackground];
    [IBbackgroundStepper_ setIntValue:settings_.dBackground];
    
    [IBdensitySlider_ setIntValue:settings_.dDensity];
    [IBdensityText_ setIntValue:settings_.dDensity];
    
    [IBfeedbackSlider_ setIntValue:settings_.dFeedback];
    [IBfeedbackText_ setIntValue:settings_.dFeedback];
    
    [IBfeedbackSpeedSlider_ setIntValue:settings_.dFeedbackspeed];
    [IBfeedbackSpeedText_ setIntValue:settings_.dFeedbackspeed];
    
    [IBspeedSlider_ setIntValue:settings_.dSpeed];
    [IBspeedText_ setIntValue:settings_.dSpeed];
    
    [IBtextureSizeSlider_ setIntValue:settings_.dFeedbacksize];
    [IBtextureSizeText_ setIntValue:settings_.dFeedbacksize];
    
    [IBvisibilitySlider_ setIntValue:settings_.dVisibility];
    [IBvisibilityText_ setIntValue:settings_.dVisibility];
    
    [IBwireframe_ setState:(settings_.dWireframe==1)? NSOnState : NSOffState];
    
    [IBtexturePopUp_ selectItemAtIndex:settings_.dTexture];
    
    [IBmainScreen_ setState:mainScreenOnly_];
}

- (NSWindow*)configureSheet
{
    NSMenuItem * tMenuItem;
    
    isConfiguring_=YES;
    
    if (IBconfigureSheet_ == nil)
    {
        
        
        [NSBundle loadNibNamed:@"ConfigureSheet" owner:self];
        
        [IBversion_ setStringValue:[[[NSBundle bundleForClass:[self class]] infoDictionary] objectForKey:@"CFBundleVersion"]];
    }
    
    tMenuItem=[IBsettingsPopUp_ itemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:0]];
        
    [tMenuItem setEnabled:NO];
    
    [self setDialogValue];
    
    return IBconfigureSheet_;
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
        
        tObject=[defaults objectForKey:@"Standard set"];
        
        if (tObject==nil)
        {
            settings_.standardSet=DEFAULTS1;
                    
            setDefaults(settings_.standardSet,&settings_);
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

- (IBAction)selectSettings:(id)sender
{
    settings_.standardSet=[[sender selectedItem] tag];
    
    if (settings_.standardSet!=0)
    {
        setDefaults(settings_.standardSet,&settings_);
    
        [self setDialogValue];
    }
}

- (IBAction)selectTexture:(id)sender
{
    settings_.dTexture=[sender indexOfSelectedItem];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setBackgroundValue:(id)sender
{
    settings_.dBackground=[sender intValue];
    
    [IBbackgroundTextField_ setIntValue:settings_.dBackground];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setDensity:(id)sender
{
    settings_.dDensity=[sender intValue];
    
    [IBdensityText_ setIntValue:settings_.dDensity];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setFeedback:(id)sender
{
    settings_.dFeedback=[sender intValue];
    
    [IBfeedbackText_ setIntValue:settings_.dFeedback];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setFeedBackSpeed:(id)sender
{
    settings_.dFeedbackspeed=[sender intValue];
    
    [IBfeedbackSpeedText_ setIntValue:settings_.dFeedbackspeed];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setSpeed:(id)sender
{
    settings_.dSpeed=[sender intValue];
    
    [IBspeedText_ setIntValue:settings_.dSpeed];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setTextureSize:(id)sender
{
    settings_.dFeedbacksize=[sender intValue];
    
    [IBtextureSizeText_ setIntValue:settings_.dFeedbacksize];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setVisibility:(id)sender
{
    settings_.dVisibility=[sender intValue];
    
    [IBvisibilityText_ setIntValue:settings_.dVisibility];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setWispsValue:(id)sender
{
    settings_.dWisps=[sender intValue];
    
    [IBwispsTextField_ setIntValue:settings_.dWisps];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}


@end
