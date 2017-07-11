/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Euphoria is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Euphoria is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#include "Euphoria.h"

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "rsMath.h"
#include "rgbhsl.h"
#include "resource.h"
#include "texture.h"


#define PIx2 6.28318530718f

wisp::wisp(EuphoriaSaverSettings * inSettings)
{
	int i, j;
	float recHalfDens = 1.0f / (inSettings->dDensity * 0.5f);
    register float * pointer;
    
    dDensity=inSettings->dDensity;
    
    
	vertices = new float**[dDensity+1];
	for(i=0; i<=dDensity; i++)
    {
		vertices[i] = new float*[dDensity+1];
        
		for(j=0; j<=dDensity; j++)
        {
			pointer=vertices[i][j] = new float[7];
			pointer[3] = float(i) * recHalfDens - 1.0f;  // x position on grid
			pointer[4] = float(j) * recHalfDens - 1.0f;  // y position on grid
			// distance squared from the center
			pointer[5] = pointer[3] * pointer[3]
				+ pointer[4] * pointer[4];
			pointer[6] = 0.0f;  // intensity
		}
	}

	// initialize constants
	for(i=0; i<NUMCONSTS; i++)
    {
		c[i] = myRandf(2.0f) - 1.0f;
		cr[i] = myRandf(PIx2);
		cv[i] = myRandf(inSettings->dSpeed * 0.03f) + (inSettings->dSpeed * 0.001f);
	}

	// pick color
	hsl[0] = myRandf(1.0f);
	hsl[1] = 0.1f + myRandf(0.9f);
	hsl[2] = 1.0f;
	hueSpeed = myRandf(0.1f) - 0.05f;
	saturationSpeed = myRandf(0.04f) + 0.001f;
}

wisp::~wisp()
{
	int i, j;

	for(i=0; i<=dDensity; i++)
    {
		for(j=0; j<=dDensity; j++)
        {
			delete[] vertices[i][j];
		}
        
		delete[] vertices[i];
	}
    
	delete[] vertices;
}


void wisp::update(EuphoriaSaverSettings * inSettings)
{
	int i, j;
	rsVec up, right, crossvec;
	// visibility constants
	float viscon1 = inSettings->dVisibility * 0.01f;
	float viscon2 = 1.0f / viscon1;
    register float * pointer;
    
	// update constants
	for(i=0; i<NUMCONSTS; i++)
    {
		cr[i] += cv[i] * inSettings->elapsedTime;
		if(cr[i] > PIx2)
			cr[i] -= PIx2;

		c[i] = cos((double) cr[i]);
	}

	// update vertex positions
	for(i=0; i<=dDensity; i++)
    {
		for(j=0; j<=dDensity; j++)
        {
			pointer=vertices[i][j];
            
            pointer[0] = pointer[3] * pointer[3] * pointer[4] * c[0]
				+ pointer[5] * c[1] + 0.5f * c[2];
			pointer[1] = pointer[4] * pointer[4] * pointer[5] * c[3]
				+ pointer[3] * c[4] + 0.5f * c[5];
			pointer[2] = pointer[5] * pointer[5] * pointer[3] * c[6]
				+ pointer[4] * c[7] + c[8];
		}
	}

	// update vertex normals for most of mesh
	for(i=1; i<dDensity; i++)
    {
		for(j=1; j<dDensity; j++)
        {
			up.set(vertices[i][j+1][0] - vertices[i][j-1][0],
				vertices[i][j+1][1] - vertices[i][j-1][1],
				vertices[i][j+1][2] - vertices[i][j-1][2]);
			right.set(vertices[i+1][j][0] - vertices[i-1][j][0],
				vertices[i+1][j][1] - vertices[i-1][j][1],
				vertices[i+1][j][2] - vertices[i-1][j][2]);
                
			up.normalize();
			right.normalize();
			crossvec.cross(right, up);
            
			// Use depth component of normal to compute intensity
			// This way only edges of wisp are bright
            
			if(crossvec[2] < 0.0f)
				crossvec[2] *= -1.0f;
			
            vertices[i][j][6] = viscon2 * (viscon1 - crossvec[2]);
            
			if(vertices[i][j][6] > 1.0f)
            {
				vertices[i][j][6] = 1.0f;
			}
            else
            {
                if(vertices[i][j][6] < 0.0f)
                {
                    vertices[i][j][6] = 0.0f;
                }
            }
        }
	}

	// update color
	hsl[0] += hueSpeed * inSettings->elapsedTime;
    
	if(hsl[0] < 0.0f)
    {
		hsl[0] += 1.0f;
	}
    else
    {
        if(hsl[0] > 1.0f)
        {
            hsl[0] -= 1.0f;
        }
    }
    
	hsl[1] += saturationSpeed * inSettings->elapsedTime;
    
	if(hsl[1] <= 0.1f)
    {
		hsl[1] = 0.1f;
		saturationSpeed = -saturationSpeed;
	}
    
	if(hsl[1] >= 1.0f)
    {
		hsl[1] = 1.0f;
		saturationSpeed = -saturationSpeed;
	}
    
	hsl2rgb(hsl[0], hsl[1], hsl[2], rgb[0], rgb[1], rgb[2]);
}

void wisp::draw(EuphoriaSaverSettings * inSettings)
{
	int i, j;
    register float * pointer;
    
	glPushMatrix();

	if(inSettings->dWireframe)
    {
		for(i=1; i<dDensity; i++)
        {
			glBegin(GL_LINE_STRIP);
            
			for(j=0; j<=dDensity; j++)
            {
                pointer=vertices[i][j];
                
				glColor3f(rgb[0] + pointer[6] - 1.0f, rgb[1] + pointer[6] - 1.0f, rgb[2] + pointer[6] - 1.0f);
				glTexCoord2d(pointer[3] - pointer[0], pointer[4] - pointer[1]);
				glVertex3fv(pointer);
			}
			glEnd();
		}
        
		for(j=1; j<dDensity; j++)
        {
			glBegin(GL_LINE_STRIP);
            
			for(i=0; i<=dDensity; i++)
            {
                pointer=vertices[i][j];
                
				glColor3f(rgb[0] + pointer[6] - 1.0f, rgb[1] + pointer[6] - 1.0f, rgb[2] + pointer[6] - 1.0f);
				glTexCoord2d(pointer[3] - pointer[0], pointer[4] - pointer[1]);
				glVertex3fv(pointer);
			}
			glEnd();
		}
	}
	else
    {
		for(i=0; i<dDensity; i++)
        {
			glBegin(GL_TRIANGLE_STRIP);
            
				for(j=0; j<=dDensity; j++)
                {
					pointer=vertices[i][j];
                    
                    glColor3f(rgb[0] + vertices[i+1][j][6] - 1.0f, rgb[1] + vertices[i+1][j][6] - 1.0f, rgb[2] + vertices[i+1][j][6] - 1.0f);
					glTexCoord2d(vertices[i+1][j][3] - vertices[i+1][j][0], vertices[i+1][j][4] - vertices[i+1][j][1]);
					glVertex3fv(vertices[i+1][j]);
					glColor3f(rgb[0] + pointer[6] - 1.0f, rgb[1] + pointer[6] - 1.0f, rgb[2] + pointer[6] - 1.0f);
					glTexCoord2d(pointer[3] - pointer[0], pointer[4] - pointer[1]);
					glVertex3fv(pointer);
				}
                
			glEnd();
		}
	}

	glPopMatrix();
}


void wisp::drawAsBackground(EuphoriaSaverSettings * inSettings){
	int i, j;
	register float * pointer;
    
	glPushMatrix();
	glTranslatef(c[0] * 0.2f, c[1] * 0.2f, 1.6f);

	if(inSettings->dWireframe)
    {
		for(i=1; i<dDensity; i++)
        {
			glBegin(GL_LINE_STRIP);
            
			for(j=0; j<=dDensity; j++)
            {
				pointer=vertices[i][j];
                
                glColor3f(rgb[0] + pointer[6] - 1.0f, rgb[1] + pointer[6] - 1.0f, rgb[2] + pointer[6] - 1.0f);
				glTexCoord2d(pointer[3] - pointer[0], pointer[4] - pointer[1]);
				glVertex3f(pointer[3], pointer[4], pointer[6]);
			}
			glEnd();
		}
		for(j=1; j<dDensity; j++)
        {
			glBegin(GL_LINE_STRIP);
            
			for(i=0; i<=dDensity; i++)
            {
                pointer=vertices[i][j];
                
				glColor3f(rgb[0] + pointer[6] - 1.0f, rgb[1] + pointer[6] - 1.0f, rgb[2] + pointer[6] - 1.0f);
				glTexCoord2d(pointer[3] - pointer[0], pointer[4] - pointer[1]);
				glVertex3f(pointer[3], pointer[4], pointer[6]);
			}
			glEnd();
		}
	}
	else{
		for(i=0; i<dDensity; i++)
        {
			glBegin(GL_TRIANGLE_STRIP);
            
				for(j=0; j<=dDensity; j++)
                {
                    pointer=vertices[i][j];
                    
					glColor3f(rgb[0] + vertices[i+1][j][6] - 1.0f, rgb[1] + vertices[i+1][j][6] - 1.0f, rgb[2] + vertices[i+1][j][6] - 1.0f);
					glTexCoord2d(vertices[i+1][j][3] - vertices[i+1][j][0], vertices[i+1][j][4] - vertices[i+1][j][1]);
					glVertex3f(vertices[i+1][j][3], vertices[i+1][j][4], vertices[i+1][j][6]);
					glColor3f(rgb[0] + pointer[6] - 1.0f, rgb[1] + pointer[6] - 1.0f, rgb[2] + pointer[6] - 1.0f);
					glTexCoord2d(pointer[3] - pointer[0], pointer[4] - pointer[1]);
					glVertex3f(pointer[3], pointer[4], pointer[6]);
				}
                
			glEnd();
		}
	}

	glPopMatrix();
}

__private_extern__ void draw(EuphoriaSaverSettings * inSettings)
{
	int i;

	// Update wisps
	for(i=0; i<inSettings->dWisps; i++)
		inSettings->wisps[i].update(inSettings);
	for(i=0; i<inSettings->dBackground; i++)
		inSettings->backwisps[i].update(inSettings);

	if(inSettings->dFeedback)
    {
		float feedbackIntensity = inSettings->dFeedback / 101.0f;

		// update feedback variables
		for(i=0; i<4; i++)
        {
			inSettings->fr[i] += inSettings->elapsedTime * inSettings->fv[i];
			if(inSettings->fr[i] > PIx2)
				inSettings->fr[i] -= PIx2;
		}

		inSettings->f[0] = 30.0f * cos((double)inSettings->fr[0]);
		inSettings->f[1] = 0.2f * cos((double)inSettings->fr[1]);
		inSettings->f[2] = 0.2f * cos((double)inSettings->fr[2]);
		inSettings->f[3] = 0.8f * cos((double)inSettings->fr[3]);

		for(i=0; i<3; i++)
        {
			inSettings->lr[i] += inSettings->elapsedTime * inSettings->lv[i];
			if(inSettings->lr[i] > PIx2)
				inSettings->lr[i] -= PIx2;

			inSettings->l[i] = cos(inSettings->lr[i]);

			inSettings->l[i] = inSettings->l[i] * inSettings->l[i];
		}

		// Create drawing area for feedback texture
		glViewport(0, 0, inSettings->feedbacktexsize, inSettings->feedbacktexsize);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(30.0, inSettings->aspectRatio, 0.01f, 20.0f);
		glMatrixMode(GL_MODELVIEW);

		// Draw
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(feedbackIntensity, feedbackIntensity, feedbackIntensity);
		glBindTexture(GL_TEXTURE_2D, inSettings->feedbacktex);
		glPushMatrix();
		glTranslatef(inSettings->f[1] * inSettings->l[1], inSettings->f[2] * inSettings->l[1], inSettings->f[3] * inSettings->l[2]);
		glRotatef(inSettings->f[0] * inSettings->l[0], 0, 0, 1);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(-0.5f, -0.5f);
			glVertex3f(-inSettings->aspectRatio*2.0f, -2.0f, 1.25f);
			glTexCoord2f(1.5f, -0.5f);
			glVertex3f(inSettings->aspectRatio*2.0f, -2.0f, 1.25f);
			glTexCoord2f(-0.5f, 1.5f);
			glVertex3f(-inSettings->aspectRatio*2.0f, 2.0f, 1.25f);
			glTexCoord2f(1.5f, 1.5f);
			glVertex3f(inSettings->aspectRatio*2.0f, 2.0f, 1.25f);
		glEnd();
		glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, inSettings->tex);
		for(i=0; i<inSettings->dBackground; i++)
			inSettings->backwisps[i].drawAsBackground(inSettings);
		for(i=0; i<inSettings->dWisps; i++)
			inSettings->wisps[i].draw(inSettings);

		// readback feedback texture
		glReadBuffer(GL_BACK);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, inSettings->feedbacktexsize);
		glBindTexture(GL_TEXTURE_2D, inSettings->feedbacktex);
		glReadPixels(0, 0, inSettings->feedbacktexsize, inSettings->feedbacktexsize, GL_RGB, GL_UNSIGNED_BYTE, inSettings->feedbackmap);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, inSettings->feedbacktexsize, inSettings->feedbacktexsize, GL_RGB, GL_UNSIGNED_BYTE, inSettings->feedbackmap);

		// create regular drawing area
		glViewport(inSettings->viewport[0], inSettings->viewport[1], inSettings->viewport[2], inSettings->viewport[3]);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(20.0, inSettings->aspectRatio, 0.01f, 20.0f);
		glMatrixMode(GL_MODELVIEW);

		// Draw again
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(feedbackIntensity, feedbackIntensity, feedbackIntensity);
		glPushMatrix();
		glTranslatef(inSettings->f[1] * inSettings->l[1], inSettings->f[2] * inSettings->l[1], inSettings->f[3] * inSettings->l[2]);
		glRotatef(inSettings->f[0] * inSettings->l[0], 0, 0, 1);
		glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(-0.5f, -0.5f);
			glVertex3f(-inSettings->aspectRatio*2.0f, -2.0f, 1.25f);
			glTexCoord2f(1.5f, -0.5f);
			glVertex3f(inSettings->aspectRatio*2.0f, -2.0f, 1.25f);
			glTexCoord2f(-0.5f, 1.5f);
			glVertex3f(-inSettings->aspectRatio*2.0f, 2.0f, 1.25f);
			glTexCoord2f(1.5f, 1.5f);
			glVertex3f(inSettings->aspectRatio*2.0f, 2.0f, 1.25f);
		glEnd();
		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D, inSettings->tex);
	}
	else
	{
        glClear(GL_COLOR_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, inSettings->tex);
    }
    
	// 
	for(i=0; i<inSettings->dBackground; i++)
		inSettings->backwisps[i].drawAsBackground(inSettings);
	for(i=0; i<inSettings->dWisps; i++)
		inSettings->wisps[i].draw(inSettings);
}

__private_extern__  void initSaver(int width,int height,EuphoriaSaverSettings * inSettings)
{
	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
    
    inSettings->fr[4] = 0.0f;
    inSettings->fr[1] = 0.0f;
    inSettings->fr[2] = 0.0f;
    inSettings->fr[3] = 0.0f;
    
    
	inSettings->lr[0] = 0.0f;
    inSettings->lr[1] = 0.0f;
    inSettings->lr[2] = 0.0f;

	// Window initialization
    
    inSettings->viewport[0] = 0;
	inSettings->viewport[1] = 0;
	inSettings->viewport[2] = width;
	inSettings->viewport[3] = height;

    
    glViewport(0,0, width,height);
	inSettings->aspectRatio = width / height;

	// setup regular drawing area just in case feedback isn't used
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(20.0, inSettings->aspectRatio, 0.01, 20);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glLineWidth(2.0f);
	// Commented out because smooth lines and textures don't mix on my TNT.
	// It's like it rendering in software mode
	//glEnable(GL_LINE_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    if (glIsTexture(inSettings->tex))
    {
        glDeleteTextures(1, (GLuint *) &inSettings->tex);
        inSettings->tex=0;
    }
    
    if (inSettings->dTexture || inSettings->dFeedback)
    {
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    if(inSettings->dTexture>0)
    {
		int whichtex = inSettings->dTexture;
		if(whichtex == 4)  // random texture
			whichtex = myRandi(3) + 1;

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		// Initialize texture
		glGenTextures(1, (GLuint *) &inSettings->tex);
		glBindTexture(GL_TEXTURE_2D, inSettings->tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        switch(whichtex)
        {
            case 1:
                gluBuild2DMipmaps(GL_TEXTURE_2D, 1, TEXSIZE, TEXSIZE, GL_LUMINANCE, GL_UNSIGNED_BYTE, plasmamap);
                break;
            case 2:
                gluBuild2DMipmaps(GL_TEXTURE_2D, 1, TEXSIZE, TEXSIZE, GL_LUMINANCE, GL_UNSIGNED_BYTE, stringymap);
                break;
            case 3:
                gluBuild2DMipmaps(GL_TEXTURE_2D, 1, TEXSIZE, TEXSIZE, GL_LUMINANCE, GL_UNSIGNED_BYTE, linesmap);
		}
	}

	
    
    if (inSettings->dFeedback>0)
    {
		inSettings->feedbacktexsize = int(pow(2, inSettings->dFeedbacksize));
        
		while(inSettings->feedbacktexsize > inSettings->viewport[2] || inSettings->feedbacktexsize > inSettings->viewport[3])
        {
			inSettings->feedbacktexsize=inSettings->feedbacktexsize/2;
		}

		// feedback texture setup

		inSettings->feedbackmap = new unsigned char[inSettings->feedbacktexsize*inSettings->feedbacktexsize*3];
		
        if (glIsTexture(inSettings->feedbacktex))
        {
            glDeleteTextures(1, (GLuint *) &inSettings->feedbacktex);
            inSettings->feedbacktex=0;
        }
        
        glGenTextures(1, (GLuint *) &inSettings->feedbacktex);
		glBindTexture(GL_TEXTURE_2D, inSettings->feedbacktex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, inSettings->feedbacktexsize, inSettings->feedbacktexsize, 0, GL_RGB, GL_UNSIGNED_BYTE, inSettings->feedbackmap);

		// feedback velocity variable setup
		inSettings->fv[0] = float(inSettings->dFeedbackspeed) * (myRandf(0.025f) + 0.025f);
		inSettings->fv[1] = float(inSettings->dFeedbackspeed) * (myRandf(0.05f) + 0.05f);
		inSettings->fv[2] = float(inSettings->dFeedbackspeed) * (myRandf(0.05f) + 0.05f);
		inSettings->fv[3] = float(inSettings->dFeedbackspeed) * (myRandf(0.1f) + 0.1f);
		inSettings->lv[0] = float(inSettings->dFeedbackspeed) * (myRandf(0.0025f) + 0.0025f);
		inSettings->lv[1] = float(inSettings->dFeedbackspeed) * (myRandf(0.0025f) + 0.0025f);
		inSettings->lv[2] = float(inSettings->dFeedbackspeed) * (myRandf(0.0025f) + 0.0025f);
	}

	// Initialize wisps
	inSettings->wisps = new wisp[inSettings->dWisps](inSettings);
	
    if (inSettings->dBackground>0)
    {
        inSettings->backwisps = new wisp[inSettings->dBackground](inSettings);
    }
    else
    {
        inSettings->backwisps=NULL;
    }
    
}

__private_extern__ void setDefaults(int which,EuphoriaSaverSettings * inSettings)
{
    switch(which)
    {
        case DEFAULTS1:  // Regular
            inSettings->dWisps = 5;
            inSettings->dBackground = 0;
            inSettings->dDensity = 25;
            inSettings->dVisibility = 35;
            inSettings->dSpeed = 15;
            inSettings->dFeedback = 0;
            inSettings->dFeedbackspeed = 1;
            inSettings->dFeedbacksize = 8;
            inSettings->dWireframe = 0;
            inSettings->dTexture = 2;
            break;
        case DEFAULTS2:  // Grid
            inSettings->dWisps = 4;
            inSettings->dBackground = 1;
            inSettings->dDensity = 25;
            inSettings->dVisibility = 70;
            inSettings->dSpeed = 15;
            inSettings->dFeedback = 0;
            inSettings->dFeedbackspeed = 1;
            inSettings->dFeedbacksize = 8;
            inSettings->dWireframe = 1;
            inSettings->dTexture = 0;
            break;
        case DEFAULTS3:  // Cubism
            inSettings->dWisps = 15;
            inSettings->dBackground = 0;
            inSettings->dDensity = 4;
            inSettings->dVisibility = 15;
            inSettings->dSpeed = 10;
            inSettings->dFeedback = 0;
            inSettings->dFeedbackspeed = 1;
            inSettings->dFeedbacksize = 8;
            inSettings->dWireframe = 0;
            inSettings->dTexture = 0;
            break;
        case DEFAULTS4:  // Bad math
            inSettings->dWisps = 2;
            inSettings->dBackground = 2;
            inSettings->dDensity = 20;
            inSettings->dVisibility = 40;
            inSettings->dSpeed = 30;
            inSettings->dFeedback = 40;
            inSettings->dFeedbackspeed = 5;
            inSettings->dFeedbacksize = 8;
            inSettings->dWireframe = 1;
            inSettings->dTexture = 2;
            break;
        case DEFAULTS5:  // M-Theory
            inSettings->dWisps = 3;
            inSettings->dBackground = 0;
            inSettings->dDensity = 25;
            inSettings->dVisibility = 15;
            inSettings->dSpeed = 20;
            inSettings->dFeedback = 40;
            inSettings->dFeedbackspeed = 20;
            inSettings->dFeedbacksize = 8;
            inSettings->dWireframe = 0;
            inSettings->dTexture = 0;
            break;
        case DEFAULTS6:  // ultra high frequency tunneling electron microscope
            inSettings->dWisps = 0;
            inSettings->dBackground = 3;
            inSettings->dDensity = 35;
            inSettings->dVisibility = 5;
            inSettings->dSpeed = 50;
            inSettings->dFeedback = 0;
            inSettings->dFeedbackspeed = 1;
            inSettings->dFeedbacksize = 8;
            inSettings->dWireframe = 0;
            inSettings->dTexture = 0;
            break;
        case DEFAULTS7:  // Nowhere
            inSettings->dWisps = 0;
            inSettings->dBackground = 3;
            inSettings->dDensity = 30;
            inSettings->dVisibility = 40;
            inSettings->dSpeed = 20;
            inSettings->dFeedback = 80;
            inSettings->dFeedbackspeed = 10;
            inSettings->dFeedbacksize = 8;
            inSettings->dWireframe = 1;
            inSettings->dTexture = 3;
            break;
        case DEFAULTS8:  // Echo
            inSettings->dWisps = 3;
            inSettings->dBackground = 0;
            inSettings->dDensity = 25;
            inSettings->dVisibility = 30;
            inSettings->dSpeed = 20;
            inSettings->dFeedback = 85;
            inSettings->dFeedbackspeed = 30;
            inSettings->dFeedbacksize = 8;
            inSettings->dWireframe = 0;
            inSettings->dTexture = 1;
            break;
        case DEFAULTS9:  // Kaleidoscope
            inSettings->dWisps = 3;
            inSettings->dBackground = 0;
            inSettings->dDensity = 25;
            inSettings->dVisibility = 40;
            inSettings->dSpeed = 15;
            inSettings->dFeedback = 90;
            inSettings->dFeedbackspeed = 3;
            inSettings->dFeedbacksize = 8;
            inSettings->dWireframe = 0;
            inSettings->dTexture = 0;
            break;
	}
}

__private_extern__ void cleanSettings(EuphoriaSaverSettings * inSettings)
{
    if (inSettings->feedbackmap!=NULL)
    {
        delete inSettings->feedbackmap;
        inSettings->feedbackmap=NULL;
    }
    
    if (inSettings->wisps!=NULL)
    {
        delete[] inSettings->wisps;
        inSettings->wisps=NULL;
    }
        
    if (inSettings->backwisps!=NULL)
    {
        delete[] inSettings->backwisps;
        inSettings->backwisps=NULL;
    }
}