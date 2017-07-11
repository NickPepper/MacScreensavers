#import <AppKit/AppKit.h>
#import <ScreenSaver/ScreenSaver.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

#include "Flux.h"

@interface RSSFluxSaverView : ScreenSaverView
{
	NSOpenGLView *_view;
    
    FluxSaverSettings settings_;
    
    int mainScreenOnly_;
    
    BOOL isConfiguring_;
    BOOL preview_;
    BOOL mainScreen_;
    
    IBOutlet id IBblurSlider_;
    IBOutlet id IBblurText_;
    IBOutlet id IBcomplexitySlider_;
    IBOutlet id IBcomplexityText_;
    IBOutlet id IBconfigureSheet_;
    IBOutlet id IBcrosswindSlider_;
    IBOutlet id IBcrosswindText_;
    IBOutlet id IBexpansionSlider_;
    IBOutlet id IBexpansionText_;
    IBOutlet id IBfluxStepper_;
    IBOutlet id IBfluxTextField_;
    IBOutlet id IBgeometryType_;
    IBOutlet id IBinstabilitySlider_;
    IBOutlet id IBinstabilityText_;
    IBOutlet id IBmainScreen_;
    IBOutlet id IBparticleStepper_;
    IBOutlet id IBparticleTextField_;
    IBOutlet id IBrandomizationSlider_;
    IBOutlet id IBrandomizationText_;
    IBOutlet id IBrotationSlider_;
    IBOutlet id IBrotationText_;
    IBOutlet id IBsettingsPopUp_;
    IBOutlet id IBsizeSlider_;
    IBOutlet id IBsizeText_;
    IBOutlet id IBtrailStepper_;
    IBOutlet id IBtrailTextField_;
    IBOutlet id IBversion_;
}

- (IBAction)closeSheet:(id)sender;

- (IBAction)setComplexity:(id)sender;
- (IBAction)setBlur:(id)sender;
- (IBAction)setCrosswind:(id)sender;
- (IBAction)setExpansion:(id)sender;
- (IBAction)setRotation:(id)sender;
- (IBAction)setSize:(id)sender;
- (IBAction)setTrailValue:(id)sender;
- (IBAction)setFluxValue:(id)sender;
- (IBAction)setParticleValue:(id)sender;
- (IBAction)setRandomization:(id)sender;
- (IBAction)setInstability:(id)sender;
- (IBAction)selectSettings:(id)sender;
- (IBAction)selectGeometry:(id)sender;

- (void) setDialogValue;

- (void) readDefaults:(ScreenSaverDefaults *) inDefaults;
- (void) writeDefaults;

@end
