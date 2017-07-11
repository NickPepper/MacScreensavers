//
//  CoolScreenSaverView.h
//  Cocoa Dev Central: Write a Screen Saver: Part 2
//

#import <ScreenSaver/ScreenSaver.h>
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import "MyOpenGLView.h"

@interface ComCocoaDevCentral_CoolScreenSaverView : ScreenSaverView 
{
	MyOpenGLView *glView;
	GLfloat rotation;
}

- (void)setUpOpenGL;

@end
