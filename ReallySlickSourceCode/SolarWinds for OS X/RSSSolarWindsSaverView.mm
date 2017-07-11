// Mac OS X Code
// 
// First version: 28/06/02 Stéphane Sudre

#import "RSSSolarWindsSaverView.h"
#include "resource.h"
#include <sys/time.h>

@implementation RSSSolarWindsSaverView

- (id)initWithFrame:(NSRect)frameRect isPreview:(BOOL) preview
{
    NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];    id tObject;
    
    self = [super initWithFrame:frameRect isPreview:preview];
        
    preview_=preview;
    
    isConfiguring_=NO;
    
    dontTakeValue=NO;
    
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
                NSOpenGLPFAAlphaSize, (NSOpenGLPixelFormatAttribute)8,
                NSOpenGLPFAMinimumPolicy,
                (NSOpenGLPixelFormatAttribute)0
            };
        
            NSOpenGLPixelFormat *format = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attribs] autorelease];
            
            if (format!=nil)
            {
                _view = [[[NSOpenGLView alloc] initWithFrame:NSZeroRect pixelFormat:format] autorelease];
                [self addSubview:_view];
            
                settings_.winds=NULL;
                
                tObject=[defaults objectForKey:@"Standard set"];
            
                if (tObject==nil)
                {
                    settings_.standardSet=DEFAULTS1;
                    
                    setDefaults(settings_.standardSet,&settings_);
                }
                else
                {
                    [self readSolarDefaults:defaults];
                }
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
                
            if (settings_.standardSet==DEFAULTSRANDOM)
            {
                int tRandomSettings;
                
                tRandomSettings=(int) SSRandomFloatBetween(DEFAULTS1,DEFAULTS6);
                
                setDefaults(tRandomSettings,&settings_);
            }
    
            initSaver((int) tSize.width,(int) tSize.height,&settings_);
            
            [[_view openGLContext] flushBuffer];
            
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

- (void) readSolarDefaults:(ScreenSaverDefaults *) inDefaults
{
    settings_.standardSet=[inDefaults integerForKey:@"Standard set"];
    
    settings_.dWinds=[inDefaults integerForKey:@"Winds count"];
    
    settings_.dEmitters=[inDefaults integerForKey:@"Emiiters count"];
    
    settings_.dParticles=[inDefaults integerForKey:@"Particles count"];
    
    settings_.dGeometry=[inDefaults integerForKey:@"Geometry"];
    
    settings_.dSize=[inDefaults integerForKey:@"Particle Size"];
    
    settings_.dParticlespeed=[inDefaults integerForKey:@"Particle Speed"];
    
    settings_.dEmitterspeed=[inDefaults integerForKey:@"Emitter Speed"];
    
    settings_.dWindspeed=[inDefaults integerForKey:@"Wind Speed"];
    
    settings_.dBlur=[inDefaults integerForKey:@"Blur"];
    
    mainScreenOnly_=[inDefaults integerForKey:@"MainScreen Only"];
}

- (void) writeSolarDefaults
{
    NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults * inDefaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];
    
    mainScreenOnly_=[IBmainScreen_ state];
    
    [inDefaults setInteger:settings_.standardSet forKey:@"Standard set"];
    
    [inDefaults setInteger:settings_.dWinds forKey:@"Winds count"];
    
    [inDefaults setInteger:settings_.dEmitters forKey:@"Emiiters count"];
    
    [inDefaults setInteger:settings_.dParticles forKey:@"Particles count"];
    
    [inDefaults setInteger:settings_.dGeometry forKey:@"Geometry"];
    
    [inDefaults setInteger:settings_.dSize forKey:@"Particle Size"];
    
    [inDefaults setInteger:settings_.dParticlespeed forKey:@"Particle Speed"];
    
    [inDefaults setInteger:settings_.dEmitterspeed forKey:@"Emitter Speed"];
    
    [inDefaults setInteger:settings_.dWindspeed forKey:@"Wind Speed"];
    
    [inDefaults setInteger:settings_.dBlur forKey:@"Blur"];
    
    [inDefaults setInteger:mainScreenOnly_ forKey:@"MainScreen Only"];
    
    [inDefaults  synchronize];
}

- (void) setDialogValue
{
    [IBsettingsPopupButton_ selectItemAtIndex:[IBsettingsPopupButton_ indexOfItemWithTag:settings_.standardSet]];
    
    [IBwindsTextField_ setIntValue:settings_.dWinds];
    [IBwindsStepper_ setIntValue:settings_.dWinds];
    
    [IBemittersTextField_ setIntValue:settings_.dEmitters];
    [IBemittersStepper_ setIntValue:settings_.dEmitters];
    
    [IBparticlesStepper_ setIntValue:settings_.dParticles];
    [IBparticlesTextField_ setIntValue:settings_.dParticles];
    
    [IBgeometryPopupButton_ selectItemAtIndex:[IBgeometryPopupButton_ indexOfItemWithTag:settings_.dGeometry]];
    
    [IBparticleSizeSlider_ setIntValue:settings_.dSize];
    [IBparticleSizeLabel_ setIntValue:settings_.dSize];
    
    [IBparticleSpeedSlider_ setIntValue:settings_.dParticlespeed];
    [IBparticleSpeedLabel_ setIntValue:settings_.dParticlespeed];
    
    [IBemitterSpeedSlider_ setIntValue:settings_.dEmitterspeed];
    [IBemitterSpeedLabel_ setIntValue:settings_.dEmitterspeed];
    
    [IBwindSpeedSlider_ setIntValue:settings_.dWindspeed];
    [IBwindSpeedLabel_ setIntValue:settings_.dWindspeed];
    
    [IBmotionBlurSlider_ setIntValue:settings_.dBlur];
    [IBmotionBlurLabel_ setIntValue:settings_.dBlur];
    
    [IBmainScreen_ setState:mainScreenOnly_];
    
    [[IBgeometryPopupButton_ window] makeFirstResponder:IBwindsTextField_];
}

- (NSWindow*)configureSheet
{
    isConfiguring_=YES;
    
    if (IBconfigureSheet_ == nil)
    {
        NSMenuItem * tMenuItem;
        
        [NSBundle loadNibNamed:@"ConfigureSheet" owner:self];
        
        [IBversion_ setStringValue:[[[NSBundle bundleForClass:[self class]] infoDictionary] objectForKey:@"CFBundleVersion"]];
        
        tMenuItem=[IBsettingsPopupButton_ itemAtIndex:[IBsettingsPopupButton_ indexOfItemWithTag:0]];
        
        [tMenuItem setEnabled:NO];
    }
    
    [self setDialogValue];
    
    return IBconfigureSheet_;
}

- (IBAction)closeSheet:(id)sender
{
    if ([sender tag]==NSOKButton)
    {
        [self writeSolarDefaults];
    }
    else
    {
        NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
        ScreenSaverDefaults *defaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];    
        
        [self readSolarDefaults:defaults];
    }
    
    isConfiguring_=NO;
    
    if ([self isAnimating]==YES)
    {
        [self stopAnimation];
        [self startAnimation];
    }
    
    [NSApp endSheet:IBconfigureSheet_];
}

- (IBAction)selectGeometry:(id)sender
{
    settings_.dGeometry=[[sender selectedItem] tag];
    
    settings_.standardSet=0;
    [IBsettingsPopupButton_ selectItemAtIndex:[IBsettingsPopupButton_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)selectSolarSettings:(id)sender
{
    dontTakeValue=YES;
    
    settings_.standardSet=[[sender selectedItem] tag];
    
    if (settings_.standardSet!=0)
    {
        setDefaults(settings_.standardSet,&settings_);
        
        [self setDialogValue];
    }
}

- (IBAction)setEmitterSpeed:(id)sender
{
    settings_.dEmitterspeed=[sender intValue];
    
    [IBemitterSpeedLabel_ setIntValue:settings_.dEmitterspeed];
    settings_.standardSet=0;
    [IBsettingsPopupButton_ selectItemAtIndex:[IBsettingsPopupButton_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setEmittersValue:(id)sender
{
    int tValue;
    
    if (dontTakeValue==YES)
    {
        dontTakeValue=NO;
        return;
    }
    
    tValue=[sender intValue];
    
    if (sender==IBemittersTextField_)
    {
        if (tValue==0)
        {
            tValue=1;
            
            [IBemittersTextField_ setIntValue:tValue];
        }
        
        [IBemittersStepper_ setIntValue:tValue];
    }
    else
    {
        [IBemittersTextField_ setIntValue:tValue];
    }
    
    if (settings_.dEmitters!=tValue)
    {
        settings_.dEmitters=tValue;
    
        settings_.standardSet=0;
        [IBsettingsPopupButton_ selectItemAtIndex:[IBsettingsPopupButton_ indexOfItemWithTag:settings_.standardSet]];
    }
}

- (IBAction)setMotionBlur:(id)sender
{
    settings_.dBlur=[sender intValue];
    
    [IBmotionBlurLabel_ setIntValue:settings_.dBlur];
    settings_.standardSet=0;
    [IBsettingsPopupButton_ selectItemAtIndex:[IBsettingsPopupButton_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setParticleSize:(id)sender
{
    settings_.dSize=[sender intValue];
    
    [IBparticleSizeLabel_ setIntValue:settings_.dSize];
    settings_.standardSet=0;
    [IBsettingsPopupButton_ selectItemAtIndex:[IBsettingsPopupButton_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setParticleSpeed:(id)sender
{
    settings_.dParticlespeed=[sender intValue];
    
    [IBparticleSpeedLabel_ setIntValue:settings_.dParticlespeed];
    settings_.standardSet=0;
    [IBsettingsPopupButton_ selectItemAtIndex:[IBsettingsPopupButton_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setParticlesValue:(id)sender
{
    int tValue;
    
    if (dontTakeValue==YES)
    {
        dontTakeValue=NO;
        return;
    }
    
    tValue=[sender intValue];
    
    if (sender==IBparticlesTextField_)
    {
        [IBparticlesStepper_ setIntValue:tValue];
    }
    else
    {
        [IBparticlesTextField_ setIntValue:tValue];
    }
    
    if (settings_.dParticles!=tValue)
    {
        settings_.dParticles=tValue;
    
        settings_.standardSet=0;
        [IBsettingsPopupButton_ selectItemAtIndex:[IBsettingsPopupButton_ indexOfItemWithTag:settings_.standardSet]];
    }
}

- (IBAction)setWindSpeed:(id)sender
{
    settings_.dWindspeed=[sender intValue];
    
    [IBwindSpeedLabel_ setIntValue:settings_.dWindspeed];
    settings_.standardSet=0;
    [IBsettingsPopupButton_ selectItemAtIndex:[IBsettingsPopupButton_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setWindsValue:(id)sender
{
    int tValue;
    
    if (dontTakeValue==YES)
    {
        [IBwindsTextField_ setIntValue:settings_.dWinds];
        
        dontTakeValue=NO;
        return;
    }
    
    tValue=[sender intValue];
    
    if (sender==IBwindsTextField_)
    {
        if (tValue==0)
        {
            tValue=1;
            
            [IBwindsTextField_ setIntValue:tValue];
        }
        
        [IBwindsStepper_ setIntValue:tValue];
    }
    else
    {
        [IBwindsTextField_ setIntValue:tValue];
    }

    if (settings_.dWinds!=tValue)
    {
        settings_.dWinds=tValue;
        
        settings_.standardSet=0;
        [IBsettingsPopupButton_ selectItemAtIndex:[IBsettingsPopupButton_ indexOfItemWithTag:settings_.standardSet]];
        
    }
}

@end
