#import <AppKit/AppKit.h>
#import <ScreenSaver/ScreenSaver.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

#include "Plasma.h"

@interface RSSPlasmaSaverView : ScreenSaverView
{
	NSOpenGLView *_view;
    
    PlasmaSaverSettings settings_;
    
    int mainScreenOnly_;
    
    BOOL isConfiguring_;
    BOOL preview_;
    BOOL mainScreen_;
    
    IBOutlet id IBfocusSlider_;
    IBOutlet id IBfocusText_;
    IBOutlet id IBmagnificationSlider_;
    IBOutlet id IBmagnificationText_;
    IBOutlet id IBmainScreen_;
    IBOutlet id IBresolutionSlider_;
    IBOutlet id IBresolutionText_;
    IBOutlet id IBspeedSlider_;
    IBOutlet id IBspeedText_;
    IBOutlet id IBconfigureSheet_;
    
    IBOutlet id IBversion_;
}

- (IBAction)closeSheet:(id)sender;
- (IBAction)setFocus:(id)sender;
- (IBAction)setMagnification:(id)sender;
- (IBAction)setResolution:(id)sender;
- (IBAction)setSpeed:(id)sender;

- (IBAction)reset:(id)sender;

- (void) setDialogValue;

- (void) readDefaults:(ScreenSaverDefaults *) inDefaults;
- (void) writeDefaults;

@end
