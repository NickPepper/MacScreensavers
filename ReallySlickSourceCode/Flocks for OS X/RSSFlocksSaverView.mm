// Mac OS X Code
// 
// First version: 07/07/02 Stéphane Sudre

#import "RSSFlocksSaverView.h"
#include <sys/time.h>

@implementation RSSFlocksSaverView

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
            
                settings_.elapsedTime=0;
                
                settings_.lBugs=NULL;
                settings_.fBugs=NULL;
                
                tObject=[defaults objectForKey:@"Leaders"];
            
                if (tObject==nil)
                {
                    setDefaults(&settings_);
                }
                else
                {
                    [self readDefaults:defaults];
                }
                
                [self setAnimationTimeInterval:0.03333];
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
        }
    }
}

- (BOOL) hasConfigureSheet
{
    return (_view!=nil);
}

- (void) readDefaults:(ScreenSaverDefaults *) inDefaults
{
    settings_.dLeaders=[inDefaults integerForKey:@"Leaders"];
    
    settings_.dFollowers=[inDefaults integerForKey:@"Followers"];
    
    settings_.dGeometry=[inDefaults integerForKey:@"Geometry"];
    
    settings_.dSize=[inDefaults integerForKey:@"Size"];
    
    settings_.dComplexity=[inDefaults integerForKey:@"Complexity"];
    
    settings_.dSpeed=[inDefaults integerForKey:@"Speed"];
    
    settings_.dStretch=[inDefaults integerForKey:@"Stretch"];
    
    settings_.dColorfadespeed=[inDefaults integerForKey:@"ColorFadeSpeed"];
    
    settings_.dChromatek=[inDefaults integerForKey:@"Chromatek"];
    
    settings_.dConnections=[inDefaults integerForKey:@"Connections"];
}

- (void) writeDefaults
{
    NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults *inDefaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];
    
    mainScreenOnly_=[IBmainScreen_ state];
    
    settings_.dChromatek=[IBchromaDepthGlasses_ state];
    
    settings_.dConnections=[IBshowConnections_ state];
    
    [inDefaults setInteger:settings_.dLeaders forKey:@"Leaders"];
    
    [inDefaults setInteger:settings_.dFollowers forKey:@"Followers"];
    
    [inDefaults setInteger:settings_.dGeometry forKey:@"Geometry"];
    
    [inDefaults setInteger:settings_.dSize forKey:@"Size"];
    
    [inDefaults setInteger:settings_.dComplexity forKey:@"Complexity"];
    
    [inDefaults setInteger:settings_.dSpeed forKey:@"Speed"];
    
    [inDefaults setInteger:settings_.dStretch forKey:@"Stretch"];
    
    [inDefaults setInteger:settings_.dColorfadespeed forKey:@"ColorFadeSpeed"];
    
    [inDefaults setInteger:settings_.dChromatek forKey:@"Chromatek"];
    
    [inDefaults setInteger:settings_.dConnections forKey:@"Connections"];
    
    [inDefaults setInteger:mainScreenOnly_ forKey:@"MainScreen Only"];
    
    [inDefaults  synchronize];
}

- (void) setDialogValue
{
    [IBleadersStepper_ setIntValue:settings_.dLeaders];
    [IBleadersTextField_ setIntValue:settings_.dLeaders];
    
    [IBfollowersStepper_ setIntValue:settings_.dFollowers];
    [IBfollowersTextField_ setIntValue:settings_.dFollowers];
    
    [IBgeometryType_ selectItemAtIndex:[IBgeometryType_ indexOfItemWithTag:settings_.dGeometry]];
    
    [IBsizeSlider_ setIntValue:settings_.dSize];
    [IBsizeText_ setIntValue:settings_.dSize];
    
    [IBcomplexitySlider_ setEnabled:(settings_.dGeometry==1)];
    
    [IBcomplexitySlider_ setIntValue:settings_.dComplexity];
    [IBcomplexityText_ setIntValue:settings_.dComplexity];
    
    [IBspeedSlider_ setIntValue:settings_.dSpeed];
    [IBspeedText_ setIntValue:settings_.dSpeed];
    
    [IBstretchSlider_ setIntValue:settings_.dStretch];
    [IBstretchText_ setIntValue:settings_.dStretch];
    
    [IBcolorFadeSpeedSlider_ setIntValue:settings_.dColorfadespeed];
    [IBcolorFadeSpeedText_ setIntValue:settings_.dColorfadespeed];
    
    [IBmainScreen_ setState:mainScreenOnly_];
    
    [IBchromaDepthGlasses_ setState:settings_.dChromatek];
    
    [IBshowConnections_ setState:settings_.dConnections];
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
        
        tObject=[defaults objectForKey:@"Leaders"];
            
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

- (IBAction)setLeaders:(id)sender
{
	settings_.dLeaders=[sender intValue];
    
    [IBleadersTextField_ setIntValue:settings_.dLeaders];
}

- (IBAction)setFollowers:(id)sender
{
    settings_.dFollowers=[sender intValue];
    
    [IBfollowersTextField_ setIntValue:settings_.dFollowers];
}

- (IBAction)setGeometryType:(id)sender
{
	settings_.dGeometry=[[sender selectedItem] tag];
    
    [IBcomplexitySlider_ setEnabled:(settings_.dGeometry==1)];

}

- (IBAction)setSize:(id)sender
{
    settings_.dSize=[sender intValue];
    
    [IBsizeText_ setIntValue:settings_.dSize];
}

- (IBAction)setComplexity:(id)sender
{
    settings_.dComplexity=[sender intValue];
    
    [IBcomplexityText_ setIntValue:settings_.dComplexity];
}

- (IBAction)setSpeed:(id)sender
{
    settings_.dSpeed=[sender intValue];
    
    [IBspeedText_ setIntValue:settings_.dSpeed];
}

- (IBAction)setStretch:(id)sender
{
    settings_.dStretch=[sender intValue];
    
    [IBstretchText_ setIntValue:settings_.dStretch];
}

- (IBAction)setColorFadeSpeed:(id)sender
{
    settings_.dColorfadespeed=[sender intValue];
    
    [IBcolorFadeSpeedText_ setIntValue:settings_.dColorfadespeed];
}

@end
