//
//  PepperCoolScreenSaverView.m
//  PepperCoolScreenSaver
//
//  Created by Nick Pershin on 01/04/17.
//  Copyright © 2017 Nick Pershin. All rights reserved.
//

#import "PepperCoolScreenSaverView.h"

@implementation ComCocoaDevCentral_PepperCoolScreenSaverView

- (instancetype)initWithFrame:(NSRect)frame isPreview:(BOOL)isPreview
{
    self = [super initWithFrame:frame isPreview:isPreview];
    if (self)
    {
        NSOpenGLPixelFormatAttribute attributes[] = {
            NSOpenGLPFAAccelerated,
            NSOpenGLPFADepthSize, 16,
            NSOpenGLPFAMinimumPolicy,
            NSOpenGLPFAClosestPolicy,
            0 };
        NSOpenGLPixelFormat *format;
        
        format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
        
        glView = [[MyOpenGLView alloc] initWithFrame:NSZeroRect pixelFormat:format];
        
        if (!glView)
        {
            NSLog( @"Couldn't initialize OpenGL view." );
            return nil;
        }
        
        [self addSubview:glView];
        [self setUpOpenGL];
        
        [self setAnimationTimeInterval:1/60.0];
    }
    return self;
}


- (void)dealloc
{
    [glView removeFromSuperview];
}


- (void)setUpOpenGL
{
    [[glView openGLContext] makeCurrentContext];
    
    glShadeModel( GL_SMOOTH );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    
    rotation = 0.0f;
}


- (void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];
    [glView setFrameSize:newSize];
    
    [[glView openGLContext] makeCurrentContext];
    
    // Reshape
    glViewport( 0, 0, (GLsizei)newSize.width, (GLsizei)newSize.height );
    glMatrixMode( GL_PROJECTION ); // select our projection matrix
    glLoadIdentity();
    gluPerspective( 45.0f, (GLfloat)newSize.width / (GLfloat)newSize.height, 0.1f, 100.0f );

    glEnable(GL_DEPTH_TEST);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    glMatrixMode( GL_MODELVIEW );
    //glLoadIdentity();
    
    [[glView openGLContext] update];
}


- (void)startAnimation
{
    [super startAnimation];
}


- (void)stopAnimation
{
    [super stopAnimation];
}


- (void)drawRect:(NSRect)rect
{
    [super drawRect:rect];
    
    [[glView openGLContext] makeCurrentContext];
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    
    glTranslatef( -1.5f, 0.0f, -6.0f );
    glRotatef( rotation, 0.0f, 1.0f, 0.0f );
    
    glBegin( GL_TRIANGLES );
    {
        glColor3f( 1.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f,  1.0f, 0.0f );
        glColor3f( 0.0f, 1.0f, 0.0f );
        glVertex3f( -1.0f, -1.0f, 1.0f );
        glColor3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( 1.0f, -1.0f, 1.0f );
        
        glColor3f( 1.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 1.0f, 0.0f );
        glColor3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( 1.0f, -1.0f, 1.0f );
        glColor3f( 0.0f, 1.0f, 0.0f );
        glVertex3f( 1.0f, -1.0f, -1.0f );
        
        glColor3f( 1.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 1.0f, 0.0f );
        glColor3f( 0.0f, 1.0f, 0.0f );
        glVertex3f( 1.0f, -1.0f, -1.0f );
        glColor3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( -1.0f, -1.0f, -1.0f );
        
        glColor3f( 1.0f, 0.0f, 0.0f );
        glVertex3f( 0.0f, 1.0f, 0.0f );
        glColor3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( -1.0f, -1.0f, -1.0f );
        glColor3f( 0.0f, 1.0f, 0.0f );
        glVertex3f( -1.0f, -1.0f, 1.0f );  
    }
    glEnd();
    
    glFlush();
}


- (void)animateOneFrame
{
    // Adjust our state
    rotation += 0.2f;
    
    // Redraw
    [self setNeedsDisplay:YES];
}


- (BOOL)hasConfigureSheet
{
    return NO;
}


- (NSWindow*)configureSheet
{
    return nil;
}

@end
