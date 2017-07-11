#import <AppKit/AppKit.h>
#import <ScreenSaver/ScreenSaver.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

#include "SolarWinds.h"

@interface RSSSolarWindsSaverView : ScreenSaverView
{
	NSOpenGLView *_view;
    
    SolarWindsSaverSettings settings_;
    
    int mainScreenOnly_;
    
    BOOL isConfiguring_;
    BOOL preview_;
    BOOL mainScreen_;
    
    BOOL dontTakeValue;
    
    IBOutlet id IBemitterSpeedLabel_;
    IBOutlet id IBemitterSpeedSlider_;
    IBOutlet id IBemittersStepper_;
    IBOutlet id IBemittersTextField_;
    IBOutlet id IBgeometryPopupButton_;
    IBOutlet id IBmainScreen_;
    IBOutlet id IBmotionBlurLabel_;
    IBOutlet id IBmotionBlurSlider_;
    IBOutlet id IBparticleSizeLabel_;
    IBOutlet id IBparticleSizeSlider_;
    IBOutlet id IBparticleSpeedLabel_;
    IBOutlet id IBparticleSpeedSlider_;
    IBOutlet id IBparticlesStepper_;
    IBOutlet id IBparticlesTextField_;
    IBOutlet id IBsettingsPopupButton_;
    IBOutlet id IBwindSpeedLabel_;
    IBOutlet id IBwindSpeedSlider_;
    IBOutlet id IBwindsStepper_;
    IBOutlet id IBwindsTextField_;
    
    IBOutlet id IBversion_;
    
    IBOutlet id IBconfigureSheet_;
}

- (IBAction)closeSheet:(id)sender;
- (IBAction)selectGeometry:(id)sender;
- (IBAction)selectSolarSettings:(id)sender;
- (IBAction)setEmitterSpeed:(id)sender;
- (IBAction)setEmittersValue:(id)sender;
- (IBAction)setMotionBlur:(id)sender;
- (IBAction)setParticleSize:(id)sender;
- (IBAction)setParticleSpeed:(id)sender;
- (IBAction)setParticlesValue:(id)sender;
- (IBAction)setWindSpeed:(id)sender;
- (IBAction)setWindsValue:(id)sender;

- (void) readSolarDefaults:(ScreenSaverDefaults *) inDefaults;
- (void) writeSolarDefaults;

@end
