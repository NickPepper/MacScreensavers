// Mac OS X Code
// 
// First version: 28/06/02 Stéphane Sudre

#import "RSSFluxSaverView.h"
#include "Flux.h"
#include "resource.h"

@implementation RSSFluxSaverView

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
                NSOpenGLPFADepthSize,(NSOpenGLPixelFormatAttribute)16,
                NSOpenGLPFAMinimumPolicy,
                (NSOpenGLPixelFormatAttribute)0
            };
        
            NSOpenGLPixelFormat *format = [[[NSOpenGLPixelFormat alloc] initWithAttributes:attribs] autorelease];
            
            if (format!=nil)
            {
                _view = [[[NSOpenGLView alloc] initWithFrame:NSZeroRect pixelFormat:format] autorelease];
                [self addSubview:_view];
            
                settings_.fluxes=NULL;
                
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
            
            if (settings_.standardSet==DEFAULTSRANDOM)
            {
                int tRandomSettings;
                
                tRandomSettings=SSRandomIntBetween(DEFAULTS1,DEFAULTS6);
                
                setDefaults(tRandomSettings,&settings_);
            }
            
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
    settings_.standardSet=[inDefaults integerForKey:@"Standard set"];
    
    settings_.dFluxes=[inDefaults integerForKey:@"Flux count"];
    
    settings_.dParticles=[inDefaults integerForKey:@"Particle count"];
    
    settings_.dTrail=[inDefaults integerForKey:@"Trail count"];
    
    settings_.dBlur=[inDefaults integerForKey:@"Blur"];
    
    settings_.dComplexity=[inDefaults integerForKey:@"Complexity"];
    
    settings_.dWind=[inDefaults integerForKey:@"Crosswind"];
    
    settings_.dExpansion=[inDefaults integerForKey:@"Expansion"];
    
    settings_.dInstability=[inDefaults integerForKey:@"Instability"];
    
    settings_.dRandomize=[inDefaults integerForKey:@"Randomize"];
    
    settings_.dRotation=[inDefaults integerForKey:@"Rotation"];
    
    settings_.dSize=[inDefaults integerForKey:@"Size"];
    
    settings_.dGeometry=[inDefaults integerForKey:@"Geometry"];
    
    mainScreenOnly_=[inDefaults integerForKey:@"MainScreen Only"];
}

- (void) writeDefaults
{
    NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults *inDefaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];
    
    mainScreenOnly_=[IBmainScreen_ state];
    
    [inDefaults setInteger:settings_.standardSet forKey:@"Standard set"];
    
    [inDefaults setInteger:settings_.dFluxes forKey:@"Flux count"];
    
    [inDefaults setInteger:settings_.dParticles forKey:@"Particle count"];
    
    [inDefaults setInteger:settings_.dTrail forKey:@"Trail count"];
    
    [inDefaults setInteger:settings_.dBlur forKey:@"Blur"];
    
    [inDefaults setInteger:settings_.dComplexity forKey:@"Complexity"];
    
    [inDefaults setInteger:settings_.dWind forKey:@"Crosswind"];
    
    [inDefaults setInteger:settings_.dExpansion forKey:@"Expansion"];
    
    [inDefaults setInteger:settings_.dInstability forKey:@"Instability"];
    
    [inDefaults setInteger:settings_.dRandomize forKey:@"Randomize"];
    
    [inDefaults setInteger:settings_.dRotation forKey:@"Rotation"];
    
    [inDefaults setInteger:settings_.dSize forKey:@"Size"];
    
    [inDefaults setInteger:settings_.dGeometry forKey:@"Geometry"];
    
    [inDefaults setInteger:mainScreenOnly_ forKey:@"MainScreen Only"];
    
    [inDefaults  synchronize];
}

- (void) setDialogValue
{
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
    
    [IBfluxTextField_ setIntValue:settings_.dFluxes];
    [IBfluxStepper_ setIntValue:settings_.dFluxes];
    
    [IBparticleTextField_ setIntValue:settings_.dParticles];
    [IBparticleStepper_ setIntValue:settings_.dParticles];
    
    [IBtrailTextField_ setIntValue:settings_.dTrail];
    [IBtrailStepper_ setIntValue:settings_.dTrail];
    
    [IBblurSlider_ setIntValue:settings_.dBlur];
    [IBblurText_ setIntValue:settings_.dBlur];
    
    [IBcomplexitySlider_ setEnabled:(settings_.dGeometry==1)];
    
    [IBcomplexitySlider_ setIntValue:settings_.dComplexity];
    [IBcomplexityText_ setIntValue:settings_.dComplexity];
    
    [IBcrosswindSlider_ setIntValue:settings_.dWind];
    [IBcrosswindText_ setIntValue:settings_.dWind];
    
    [IBexpansionSlider_ setIntValue:settings_.dExpansion];
    [IBexpansionText_ setIntValue:settings_.dExpansion];
    
    [IBinstabilitySlider_ setIntValue:settings_.dInstability];
    [IBinstabilityText_ setIntValue:settings_.dInstability];
    
    [IBrandomizationSlider_ setIntValue:settings_.dRandomize];
    [IBrandomizationText_ setIntValue:settings_.dRandomize];
    
    [IBrotationSlider_ setIntValue:settings_.dRotation];
    [IBrotationText_ setIntValue:settings_.dRotation];
    
    [IBsizeSlider_ setIntValue:settings_.dSize];
    [IBsizeText_ setIntValue:settings_.dSize];
    
    [IBgeometryType_ selectItemAtIndex:settings_.dGeometry];
    
    [IBmainScreen_ setState:mainScreenOnly_];
}

- (BOOL)validateMenuItem:(NSMenuItem *)anItem
{
    
    if ([anItem action]==@selector(print:))
    {
        return NO;
    }
    
    return YES;
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


- (IBAction)setSize:(id)sender
{
    settings_.dSize=[sender intValue];
    
    [IBsizeText_ setIntValue:settings_.dSize];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setComplexity:(id)sender
{
    settings_.dComplexity=[sender intValue];
    
    [IBcomplexityText_ setIntValue:settings_.dComplexity];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setBlur:(id)sender
{
    settings_.dBlur=[sender intValue];
    
    [IBblurText_ setIntValue:settings_.dBlur];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setCrosswind:(id)sender
{
    settings_.dWind=[sender intValue];
    
    [IBcrosswindText_ setIntValue:settings_.dWind];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setExpansion:(id)sender
{
    settings_.dExpansion=[sender intValue];
    
    [IBexpansionText_ setIntValue:settings_.dExpansion];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setRotation:(id)sender
{
    settings_.dRotation=[sender intValue];
    
    [IBrotationText_ setIntValue:settings_.dRotation];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setTrailValue:(id)sender
{
    settings_.dTrail=[sender intValue];
    
    [IBtrailTextField_ setIntValue:settings_.dTrail];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setFluxValue:(id)sender
{
    settings_.dFluxes=[sender intValue];
    
    [IBfluxTextField_ setIntValue:settings_.dFluxes];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setParticleValue:(id)sender
{
    settings_.dParticles=[sender intValue];
    
    [IBparticleTextField_ setIntValue:settings_.dParticles];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setRandomization:(id)sender
{
    settings_.dRandomize=[sender intValue];
    
    [IBrandomizationText_ setIntValue:settings_.dRandomize];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setInstability:(id)sender
{
    settings_.dInstability=[sender intValue];
    
    [IBinstabilityText_ setIntValue:settings_.dInstability];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
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

- (IBAction)selectGeometry:(id)sender
{
    settings_.dGeometry=[[sender selectedItem] tag];
    
    [IBcomplexitySlider_ setEnabled:(settings_.dGeometry==1)];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

@end
