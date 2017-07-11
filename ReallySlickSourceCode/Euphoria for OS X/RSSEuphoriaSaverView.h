#import <AppKit/AppKit.h>
#import <ScreenSaver/ScreenSaver.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

#include "Euphoria.h"

@interface RSSEuphoriaSaverView : ScreenSaverView
{
	NSOpenGLView *_view;
    
    EuphoriaSaverSettings settings_;
    
    int mainScreenOnly_;
    
    BOOL isConfiguring_;
    BOOL preview_;
    BOOL mainScreen_;
    
    IBOutlet id IBmainScreen_;
    IBOutlet id IBbackgroundStepper_;
    IBOutlet id IBbackgroundTextField_;
    IBOutlet id IBconfigureSheet_;
    IBOutlet id IBdensitySlider_;
    IBOutlet id IBdensityText_;
    IBOutlet id IBfeedbackSlider_;
    IBOutlet id IBfeedbackSpeedSlider_;
    IBOutlet id IBfeedbackSpeedText_;
    IBOutlet id IBfeedbackText_;
    IBOutlet id IBsettingsPopUp_;
    IBOutlet id IBspeedSlider_;
    IBOutlet id IBspeedText_;
    IBOutlet id IBtexturePopUp_;
    IBOutlet id IBtextureSizeSlider_;
    IBOutlet id IBtextureSizeText_;
    IBOutlet id IBvisibilitySlider_;
    IBOutlet id IBvisibilityText_;
    IBOutlet id IBwireframe_;
    IBOutlet id IBwispsStepper_;
    IBOutlet id IBwispsTextField_;
    
    IBOutlet id IBversion_;
    
    unsigned long long tLastTime;
}

- (IBAction)closeSheet:(id)sender;
- (IBAction)selectSettings:(id)sender;
- (IBAction)selectTexture:(id)sender;
- (IBAction)setBackgroundValue:(id)sender;
- (IBAction)setDensity:(id)sender;
- (IBAction)setFeedback:(id)sender;
- (IBAction)setFeedBackSpeed:(id)sender;
- (IBAction)setSpeed:(id)sender;
- (IBAction)setTextureSize:(id)sender;
- (IBAction)setVisibility:(id)sender;
- (IBAction)setWispsValue:(id)sender;

- (void) readDefaults:(ScreenSaverDefaults *) inDefaults;
- (void) writeDefaults;

@end
