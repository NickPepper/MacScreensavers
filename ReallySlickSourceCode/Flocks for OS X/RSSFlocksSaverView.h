#import <AppKit/AppKit.h>
#import <ScreenSaver/ScreenSaver.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

#include "Flocks.h"

@interface RSSFlocksSaverView : ScreenSaverView
{
	NSOpenGLView *_view;
    
    FlocksSaverSettings settings_;
    
    int mainScreenOnly_;
    
    BOOL isConfiguring_;
    BOOL preview_;
    BOOL mainScreen_;
    
    IBOutlet id IBchromaDepthGlasses_;
    IBOutlet id IBcolorFadeSpeedSlider_;
    IBOutlet id IBcolorFadeSpeedText_;
    IBOutlet id IBcomplexitySlider_;
    IBOutlet id IBcomplexityText_;
    IBOutlet id IBconfigureSheet_;
    IBOutlet id IBfollowersStepper_;
    IBOutlet id IBfollowersTextField_;
    IBOutlet id IBgeometryType_;
    IBOutlet id IBleadersStepper_;
    IBOutlet id IBleadersTextField_;
    IBOutlet id IBmainScreen_;
    IBOutlet id IBshowConnections_;
    IBOutlet id IBsizeSlider_;
    IBOutlet id IBsizeText_;
    IBOutlet id IBspeedSlider_;
    IBOutlet id IBspeedText_;
    IBOutlet id IBstretchSlider_;
    IBOutlet id IBstretchText_;
    IBOutlet id IBversion_;
    
    unsigned long long tLastTime;
}

- (IBAction)closeSheet:(id)sender;
- (IBAction)setColorFadeSpeed:(id)sender;
- (IBAction)setComplexity:(id)sender;
- (IBAction)setFollowers:(id)sender;
- (IBAction)setGeometryType:(id)sender;
- (IBAction)setLeaders:(id)sender;
- (IBAction)setSize:(id)sender;
- (IBAction)setSpeed:(id)sender;
- (IBAction)setStretch:(id)sender;




- (void) readDefaults:(ScreenSaverDefaults *) inDefaults;
- (void) writeDefaults;

@end
