#import <AppKit/AppKit.h>
#import <ScreenSaver/ScreenSaver.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

#include "Cyclone.h"

@interface RSSCycloneSaverView : ScreenSaverView
{
	NSOpenGLView *_view;
    
    CycloneSaverSettings settings_;
    
    int mainScreenOnly_;
    
    BOOL isConfiguring_;
    BOOL preview_;
    BOOL mainScreen_;
    
    IBOutlet id IBcomplexitySlider_;
    IBOutlet id IBcomplexityText_;
    IBOutlet id IBconfigureSheet_;
    IBOutlet id IBcycloneStepper_;
    IBOutlet id IBcycloneTextField_;
    IBOutlet id IBmainScreen_;
    IBOutlet id IBparticleStepper_;
    IBOutlet id IBparticleTextField_;
    IBOutlet id IBshowCurves_;
    IBOutlet id IBsizeSlider_;
    IBOutlet id IBsizeText_;
    IBOutlet id IBspeedSlider_;
    IBOutlet id IBspeedText_;
    IBOutlet id IBstretch_;
    
    IBOutlet id IBversion_;
}

- (IBAction)closeSheet:(id)sender;
- (IBAction)reset:(id)sender;
- (IBAction)setComplexity:(id)sender;
- (IBAction)setCyclone:(id)sender;
- (IBAction)setParticle:(id)sender;
- (IBAction)setSize:(id)sender;
- (IBAction)setSpeed:(id)sender;



- (void) readDefaults:(ScreenSaverDefaults *) inDefaults;
- (void) writeDefaults;

@end
