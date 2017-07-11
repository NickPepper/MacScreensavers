// Mac OS X Code
// 
// First version: 28/06/02 Stéphane Sudre

#import "RSSLatticeSaverView.h"
#include "Lattice.h"
#include "resource.h"
#include <sys/time.h>

@implementation RSSLatticeSaverView

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
            
                settings_.elapsedTime=0;
                
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
                
                [self setAnimationTimeInterval:0.03];
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
            
            if(tCurentTime >= tLastTime)
                times[timeindex] = float(tCurentTime - tLastTime) * 0.001f;
            else  // else use elapsedTime from last frame
                times[timeindex] = settings_.elapsedTime;
            
            
            settings_.elapsedTime = 0.1f * (times[0] + times[1] + times[2] + times[3] + times[4]
            + times[5] + times[6] + times[7] + times[8] + times[9]);
        
            timeindex ++;
            if(timeindex >= 10)
                timeindex = 0;

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
            int i;
            
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
            
            settings_.dUsedTexture=settings_.dTexture;
            
            if(settings_.dUsedTexture == LATTICE_TEXTURE_RANDOM)  // Choose random texture
            {
                settings_.dUsedTexture = SSRandomIntBetween(LATTICE_TEXTURE_NONE,LATTICE_TEXTURE_DOUGHNUTS);
            }
            
            initSaver((int) tSize.width,(int) tSize.height,&settings_);
            
            for(i=0;i<10;i++)
            {
                times[i]=0.03;
            }
            
            timeindex = 0;
            
            settings_.elapsedTime=0;
            
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
    settings_.standardSet=[inDefaults integerForKey:@"Standard set"];
    
    settings_.standardSet=[inDefaults integerForKey:@"Standard set"];
    
    settings_.dLongitude=[inDefaults integerForKey:@"Longitude"];
    
    settings_.dLatitude=[inDefaults integerForKey:@"Latitude"];
    
    settings_.dThick=[inDefaults integerForKey:@"Thick"];
    
    settings_.dDensity=[inDefaults integerForKey:@"Density"];
    
    settings_.dFov=[inDefaults integerForKey:@"Fov"];
    
    settings_.dPathrand=[inDefaults integerForKey:@"Pathrand"];
    
    settings_.dSmooth=[inDefaults integerForKey:@"Smooth"];
    
    settings_.dFog=[inDefaults integerForKey:@"Fog"];
    
    settings_.dWidescreen=[inDefaults integerForKey:@"WideScreen"];
    
    settings_.dDepth=[inDefaults integerForKey:@"Depth"];
    
    settings_.dSpeed=[inDefaults integerForKey:@"Speed"];
    
    settings_.dTexture=[inDefaults integerForKey:@"Texture"];
    
    mainScreenOnly_=[inDefaults integerForKey:@"MainScreen Only"];
}

- (void) writeDefaults
{
    NSString *identifier = [[NSBundle bundleForClass:[self class]] bundleIdentifier];
    ScreenSaverDefaults *inDefaults = [ScreenSaverDefaults defaultsForModuleWithName:identifier];
    
    mainScreenOnly_=[IBmainScreen_ state];
    
    settings_.dSmooth=[IBsmooth_ state];
    settings_.dFog=[IBfog_ state];
    settings_.dWidescreen=[IBwidescreen_ state];
    
    [inDefaults setInteger:settings_.standardSet forKey:@"Standard set"];
    
    [inDefaults setInteger:settings_.dLongitude forKey:@"Longitude"];
    
    [inDefaults setInteger:settings_.dLatitude forKey:@"Latitude"];
    
    [inDefaults setInteger:settings_.dThick forKey:@"Thick"];
    
    [inDefaults setInteger:settings_.dDensity forKey:@"Density"];
    
    [inDefaults setInteger:settings_.dFov forKey:@"Fov"];
    
    [inDefaults setInteger:settings_.dPathrand forKey:@"Pathrand"];
    
    [inDefaults setInteger:settings_.dSmooth forKey:@"Smooth"];
    
    [inDefaults setInteger:settings_.dFog forKey:@"Fog"];
    
    [inDefaults setInteger:settings_.dWidescreen forKey:@"WideScreen"];
    
    [inDefaults setInteger:settings_.dSpeed forKey:@"Speed"];
    
    [inDefaults setInteger:settings_.dDepth forKey:@"Depth"];
    
    [inDefaults setInteger:settings_.dTexture forKey:@"Texture"];
    
    [inDefaults setInteger:mainScreenOnly_ forKey:@"MainScreen Only"];
    
    [inDefaults  synchronize];
}

- (void) setDialogValue
{
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
    
    [IBlongitudeTextField_ setIntValue:settings_.dLongitude];
    [IBlongitudeStepper_ setIntValue:settings_.dLongitude];
    
    [IBlatitudeTextField_ setIntValue:settings_.dLatitude];
    [IBlatitudeStepper_ setIntValue:settings_.dLatitude];
    
    [IBthicknessSlider_ setIntValue:settings_.dThick];
    [IBthicknessText_ setIntValue:settings_.dThick];
    
    [IBdensitySlider_ setIntValue:settings_.dDensity];
    [IBdensityText_ setIntValue:settings_.dDensity];
    
    [IBspeedSlider_ setIntValue:settings_.dSpeed];
    [IBspeedText_ setIntValue:settings_.dSpeed];
    
    [IBfovSlider_ setIntValue:settings_.dFov];
    [IBfovText_ setIntValue:settings_.dFov];
    
    [IBcameraPathSlider_ setIntValue:settings_.dPathrand];
    [IBcameraPathText_ setIntValue:settings_.dPathrand];
    
    [IBdepthSlider_ setIntValue:settings_.dDepth];
    [IBdepthText_ setIntValue:settings_.dDepth];
    
    [IBtexturePopUp_ selectItemAtIndex:settings_.dTexture];
    
    [IBmainScreen_ setState:mainScreenOnly_];
    
    [IBsmooth_ setState:settings_.dSmooth];
    [IBfog_ setState:settings_.dFog];
    [IBwidescreen_ setState:settings_.dWidescreen];
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


- (IBAction)setDensity:(id)sender
{
    settings_.dDensity=[sender intValue];
    
    [IBdensityText_ setIntValue:settings_.dDensity];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setFov:(id)sender
{
    settings_.dFov=[sender intValue];
    
    [IBfovText_ setIntValue:settings_.dFov];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setThickness:(id)sender;
{
    settings_.dThick=[sender intValue];
    
    [IBthicknessText_ setIntValue:settings_.dThick];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setCameraPath:(id)sender
{
    settings_.dPathrand=[sender intValue];
    
    [IBcameraPathText_ setIntValue:settings_.dPathrand];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setLongitudeValue:(id)sender
{
    settings_.dLongitude=[sender intValue];
    
    [IBlongitudeTextField_ setIntValue:settings_.dLongitude];
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setLatitudeValue:(id)sender
{
    settings_.dLatitude=[sender intValue];
    
    [IBlatitudeTextField_ setIntValue:settings_.dLatitude];
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

- (IBAction)setDepth:(id)sender
{
    settings_.dDepth=[sender intValue];
    
    [IBdepthText_ setIntValue:settings_.dDepth];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setSmooth:(id)sender
{
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

- (IBAction)setFog:(id)sender
{
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

- (IBAction)selectTexture:(id)sender
{
    settings_.dTexture=[sender indexOfSelectedItem];
    
    settings_.standardSet=0;
    [IBsettingsPopUp_ selectItemAtIndex:[IBsettingsPopUp_ indexOfItemWithTag:settings_.standardSet]];
}

@end
