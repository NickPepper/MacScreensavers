//
//  PepperCoolScreenSaverView.h
//  PepperCoolScreenSaver
//
//  Created by Nick Pershin on 01/04/17.
//  Copyright © 2017 Nick Pershin. All rights reserved.
//

#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#import <ScreenSaver/ScreenSaver.h>
#import "MyOpenGLView.h"

@interface ComCocoaDevCentral_PepperCoolScreenSaverView : ScreenSaverView
{
    MyOpenGLView *glView;
    GLfloat rotation;
}

- (void)setUpOpenGL;

@end
