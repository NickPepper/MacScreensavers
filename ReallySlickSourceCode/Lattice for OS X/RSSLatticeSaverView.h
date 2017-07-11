#import <AppKit/AppKit.h>
#import <ScreenSaver/ScreenSaver.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

#include "Lattice.h"

@interface RSSLatticeSaverView : ScreenSaverView
{
	NSOpenGLView *_view;
    
    LatticeSaverSettings settings_;
    
    int mainScreenOnly_;
    
    BOOL isConfiguring_;
    BOOL preview_;
    BOOL mainScreen_;
    
    float times[10];
    int timeindex;
    
    unsigned long long tLastTime;
    
    IBOutlet id IBcameraPathSlider_;
    IBOutlet id IBcameraPathText_;
    IBOutlet id IBcomplexitySlider_;
    IBOutlet id IBcomplexityText_;
    IBOutlet id IBconfigureSheet_;
    IBOutlet id IBdensitySlider_;
    IBOutlet id IBdensityText_;
    IBOutlet id IBdepthSlider_;
    IBOutlet id IBdepthText_;
    IBOutlet id IBfovSlider_;
    IBOutlet id IBfovText_;
    IBOutlet id IBlatitudeStepper_;
    IBOutlet id IBlatitudeTextField_;
    IBOutlet id IBlongitudeStepper_;
    IBOutlet id IBlongitudeTextField_;
    IBOutlet id IBmainScreen_;
    IBOutlet id IBsettingsPopUp_;
    IBOutlet id IBspeedSlider_;
    IBOutlet id IBspeedText_;
    IBOutlet id IBtexturePopUp_;
    IBOutlet id IBthicknessSlider_;
    IBOutlet id IBthicknessText_;
    IBOutlet id IBversion_;
    
    IBOutlet id IBsmooth_;
    IBOutlet id IBfog_;
    IBOutlet id IBwidescreen_;
}

- (IBAction)closeSheet:(id)sender;

- (IBAction)setDensity:(id)sender;
- (IBAction)setDepth:(id)sender;
- (IBAction)setSpeed:(id)sender;
- (IBAction)setFov:(id)sender;
- (IBAction)setThickness:(id)sender;

- (IBAction)setCameraPath:(id)sender;
- (IBAction)setLatitudeValue:(id)sender;
- (IBAction)setLongitudeValue:(id)sender;
- (IBAction)selectSettings:(id)sender;
- (IBAction)selectTexture:(id)sender;

- (IBAction)setSmooth:(id)sender;
- (IBAction)setFog:(id)sender;

- (void) setDialogValue;

- (void) readDefaults:(ScreenSaverDefaults *) inDefaults;
- (void) writeDefaults;

@end
