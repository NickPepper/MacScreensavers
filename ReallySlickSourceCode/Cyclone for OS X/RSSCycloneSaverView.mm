// Mac OS X Code
// 
// First version: 07/07/02 Stéphane Sudre

#import "RSSCycloneSaverView.h"

@implementation RSSCycloneSaverView

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
            
                settings_.cyclones=NULL;
                settings_.particles=NULL;
                
                tObject=[defaults objectForKey:@"Cyclones"];
            
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

- (void) setFrameSize:(NSSize)newSize
{
	[super setFrameSize:newSize];
    
    if (_view!=nil)
    {
        [_view setFrameSize:newSize];
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
            glFlush();
            
            [[_view openGLContext] flushBuffer];
            
            tSize=[_view frame].size;
            
            cleanSettings(&settings_);
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
    settings_.dCyclones=[inDefaults integerForKey:@"Cyclones"];
    
    settings_.dParticles=[inDefaults integerForKey:@"Particles"];
    
    settings_.dSize=[inDefaults integerForKey:@"Size"];
    
    settings_.dComplexity=[inDefaults integerForKey:@"Complexity"];
    
    settings_.dSpeed=[inDefaults integerForKey:@"Speed"];
    
    settings_.dStretch=[inDefaults integerForKey:@"Stretch"];
    
    settings_.dShowCurves=[inDefaults integerForKey:@"Curve"];
}

- (void) writeDefaults
{
    NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults *inDefaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];
    
    mainScreenOnly_=[IBmainScreen_ state];
    
    settings_.dStretch=[IBstretch_ state];
    
    settings_.dShowCurves=[IBshowCurves_ state];
    
    [inDefaults setInteger:settings_.dCyclones forKey:@"Cyclones"];
    
    [inDefaults setInteger:settings_.dParticles forKey:@"Particles"];
    
    [inDefaults setInteger:settings_.dSize forKey:@"Size"];
    
    [inDefaults setInteger:settings_.dComplexity forKey:@"Complexity"];
    
    [inDefaults setInteger:settings_.dSpeed forKey:@"Speed"];
    
    [inDefaults setInteger:settings_.dStretch forKey:@"Stretch"];
    
    [inDefaults setInteger:settings_.dShowCurves forKey:@"Curve"];
    
    [inDefaults setInteger:mainScreenOnly_ forKey:@"MainScreen Only"];
    
    [inDefaults  synchronize];
}

- (void) setDialogValue
{
    [IBcycloneStepper_ setIntValue:settings_.dCyclones];
    [IBcycloneTextField_ setIntValue:settings_.dCyclones];
    
    [IBparticleStepper_ setIntValue:settings_.dParticles];
    [IBparticleTextField_ setIntValue:settings_.dParticles];
    
    [IBcomplexitySlider_ setIntValue:settings_.dComplexity];
    [IBcomplexityText_ setIntValue:settings_.dComplexity];
    
    [IBsizeSlider_ setIntValue:settings_.dSize];
    [IBsizeText_ setIntValue:settings_.dSize];
    
    [IBspeedSlider_ setIntValue:settings_.dSpeed];
    [IBspeedText_ setIntValue:settings_.dSpeed];
    
    [IBmainScreen_ setState:mainScreenOnly_];
    
    [IBstretch_ setState:settings_.dStretch];
    
    [IBshowCurves_ setState:settings_.dShowCurves];
}

- (NSWindow*)configureSheet
{
    isConfiguring_=YES;
    
    if (IBconfigureSheet_ == nil)
    {
        [NSBundle loadNibNamed:@"ConfigureSheet" owner:self];
        
        [IBversion_ setStringValue:[[[NSBundle bundleForClass:[self class]] infoDictionary] objectForKey:@"CFBundleVersion"]];
        
        [self setDialogValue];
    }
    
    return IBconfigureSheet_;
}

- (IBAction)closeSheet:(id)sender
{
    [self writeDefaults];
    
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

- (IBAction)reset:(id)sender
{
    setDefaults(&settings_);
    
    [self setDialogValue];
}

- (IBAction)setComplexity:(id)sender
{
    settings_.dComplexity=[sender intValue];
    
    [IBcomplexityText_ setIntValue:settings_.dComplexity];
}

- (IBAction)setCyclone:(id)sender
{
    settings_.dCyclones=[sender intValue];
    
    [IBcycloneTextField_ setIntValue:settings_.dCyclones];
}

- (IBAction)setParticle:(id)sender
{
    settings_.dParticles=[sender intValue];
    
    [IBparticleTextField_ setIntValue:settings_.dParticles];
}

- (IBAction)setSize:(id)sender
{
    settings_.dSize=[sender intValue];
    
    [IBsizeText_ setIntValue:settings_.dSize];
}

@end
