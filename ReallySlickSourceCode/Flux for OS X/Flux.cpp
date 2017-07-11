/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Flux is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Flux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


// Flux screen saver


#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "rgbhsl.h"
#include "resource.h"
#include "Flux.h"


// Useful random number macros
// Don't forget to initialize with srand()

static int myRandi(int x)
{
	return((rand() * x) / RAND_MAX);
}

static float myRandf(float x)
{
	return(((double) rand() * x) / RAND_MAX);
}

particle::particle(FluxSaverSettings * inSettings)
{
	int i;
    
    trail=inSettings->dTrail;
    
	// Offsets are somewhat like default positions for the head of each
	// particle trail.  Offsets spread out the particle trails and keep
	// them from all overlapping.
    
	offset[0] = cos(PIx2 * float(inSettings->whichparticle) / float(inSettings->dParticles));
	offset[1] = float(inSettings->whichparticle) / float(inSettings->dParticles) - 0.5f;
	offset[2] = sin(PIx2 * float(inSettings->whichparticle) / float(inSettings->dParticles));
	
    inSettings->whichparticle++;

	// Initialize memory and set initial positions out of view of the camera
    
	vertices = new float*[trail];
    
	for(i=0; i<trail; i++)
    {
		vertices[i] = new float[5];  // 0,1,2 = position, 3 = hue, 4 = saturation
		vertices[i][0] = 0.0f;
		vertices[i][1] = 3.0f;
		vertices[i][2] = 0.0f;
		vertices[i][3] = 0.0f;
		vertices[i][4] = 0.0f;
	}

	counter = 0;
}

particle::~particle()
{
	for(int i=0; i<trail; i++)
    {
		delete[] vertices[i];
	}
    
    delete[] vertices;
}

float particle::update(float *c,FluxSaverSettings * inSettings)
{
	int i, p, growth;
	float rgb[3];
	float cx, cy, cz;  // Containment variables
	float luminosity;
	float expander = 1.0f + 0.0005f * float(inSettings->dExpansion);
	float blower = 0.001f * float(inSettings->dWind);
	float depth;

	// Record old position
	int oldc = counter;
	float oldpos[3];
	oldpos[0] = vertices[oldc][0];
	oldpos[1] = vertices[oldc][1];
	oldpos[2] = vertices[oldc][2];

	counter ++;
	if(counter >= inSettings->dTrail)
		counter = 0;

	// Here's the iterative math for calculating new vertex positions
	// first calculate limiting terms which keep vertices from constantly
	// flying off to infinity
	cx = vertices[oldc][0] * (1.0f - 1.0f / (vertices[oldc][0] * vertices[oldc][0] + 1.0f));
	cy = vertices[oldc][1] * (1.0f - 1.0f / (vertices[oldc][1] * vertices[oldc][1] + 1.0f));
	cz = vertices[oldc][2] * (1.0f - 1.0f / (vertices[oldc][2] * vertices[oldc][2] + 1.0f));
	// then calculate new positions
	vertices[counter][0] = vertices[oldc][0] + c[6] * offset[0] - cx
		+ c[2] * vertices[oldc][1]
		+ c[5] * vertices[oldc][2];
	vertices[counter][1] = vertices[oldc][1] + c[6] * offset[1] - cy
		+ c[1] * vertices[oldc][2]
		+ c[4] * vertices[oldc][0];
	vertices[counter][2] = vertices[oldc][2] + c[6] * offset[2] - cz
		+ c[0] * vertices[oldc][0]
		+ c[3] * vertices[oldc][1];

	// Pick a hue
	vertices[counter][3] = cx * cx + cy * cy + cz * cz;
	if(vertices[counter][3] > 1.0f)
		vertices[counter][3] = 1.0f;
	vertices[counter][3] += c[7];
	// Limit the hue (0 - 1)
	if(vertices[counter][3] > 1.0f)
		vertices[counter][3] -= 1.0f;
	if(vertices[counter][3] < 0.0f)
		vertices[counter][3] += 1.0f;
	// Pick a saturation
	vertices[counter][4] = c[0] + vertices[counter][3];
	// Limit the saturation (0 - 1)
	if(vertices[counter][4] < 0.0f)
		vertices[counter][4] = -vertices[counter][4];
	vertices[counter][4] -= float(int(vertices[counter][4]));
	vertices[counter][4] = 1.0f - (vertices[counter][4] * vertices[counter][4]);

	// Bring particles back if they escape
	if(!counter)
    {
		if((vertices[0][0] > 1000000000.0f) || (vertices[0][0] < -1000000000.0f)
			|| (vertices[0][1] > 1000000000.0f) || (vertices[0][1] < -1000000000.0f)
			|| (vertices[2][2] > 1000000000.0f) || (vertices[0][2] < -1000000000.0f)){
			vertices[0][0] = myRandf(2.0f) - 1.0f;
			vertices[0][1] = myRandf(2.0f) - 1.0f;
			vertices[0][2] = myRandf(2.0f) - 1.0f;
		}
	}

	// Draw every vertex in particle trail
	p = counter;
	growth = 0;
	luminosity = inSettings->lumdiff;
	for(i=0; i<inSettings->dTrail; i++)
    {
		p ++;
		if(p >= inSettings->dTrail)
			p = 0;
		growth++;

		// assign color to particle
		hsl2rgb(vertices[p][3], vertices[p][4], luminosity, rgb[0], rgb[1], rgb[2]);
		glColor3fv(rgb);

		glPushMatrix();
		if(inSettings->dGeometry == 1)  // Spheres
			glTranslatef(vertices[p][0], vertices[p][1], vertices[p][2]);
		else
        {  // Points or lights
			depth = inSettings->cosCameraAngle * vertices[p][2] - inSettings->sinCameraAngle * vertices[p][0];
			glTranslatef(inSettings->cosCameraAngle * vertices[p][0] + inSettings->sinCameraAngle
				* vertices[p][2], vertices[p][1], depth);
		}
        
		if(inSettings->dGeometry)
        {  // Spheres or lights
			switch(inSettings->dTrail - growth)
            {
			case 0:
				glScalef(0.259f, 0.259f, 0.259f);
				break;
			case 1:
				glScalef(0.5f, 0.5f, 0.5f);
				break;
			case 2:
				glScalef(0.707f, 0.707f, 0.707f);
				break;
			case 3:
				glScalef(0.866f, 0.866f, 0.866f);
				break;
			case 4:
				glScalef(0.966f, 0.966f, 0.966f);
			}
		}
        
		switch(inSettings->dGeometry)
        {
		case 0:  // Points
			switch(inSettings->dTrail - growth)
            {
			case 0:
				glPointSize(float(inSettings->dSize * (depth + 200.0f) * 0.001036f));
				break;
			case 1:
				glPointSize(float(inSettings->dSize * (depth + 200.0f) * 0.002f));
				break;
			case 2:
				glPointSize(float(inSettings->dSize * (depth + 200.0f) * 0.002828f));
				break;
			case 3:
				glPointSize(float(inSettings->dSize * (depth + 200.0f) * 0.003464f));
				break;
			case 4:
				glPointSize(float(inSettings->dSize * (depth + 200.0f) * 0.003864f));
				break;
			default:
				glPointSize(float(inSettings->dSize * (depth + 200.0f) * 0.004f));
			}
            
			glBegin(GL_POINTS);
				glVertex3f(0.0f,0.0f,0.0f);
			glEnd();
			break;
		case 1:  // Spheres
		case 2:  // Lights
			glCallList(1);
		}
        
		glPopMatrix();
		vertices[p][0] *= expander;
		vertices[p][1] *= expander;
		vertices[p][2] *= expander;
		vertices[p][2] += blower;
		luminosity += inSettings->lumdiff;
	}

	// Find distance between new position and old position and return it
	oldpos[0] -= vertices[counter][0];
	oldpos[1] -= vertices[counter][1];
	oldpos[2] -= vertices[counter][2];
	return(float(sqrt(oldpos[0] * oldpos[0] + oldpos[1] * oldpos[1] + oldpos[2] * oldpos[2])));
}


// This class is a set of particle trails and constants that enter
// into their equations of motion.

flux::flux(FluxSaverSettings *inSettings)
{
	int i;

	inSettings->whichparticle = 0;

	particles = new particle[inSettings->dParticles](inSettings);
	randomize = 1;
    
	for(i=0; i<NUMCONSTS; i++)
    {
		c[i] = myRandf(2.0f) - 1.0f;
		cv[i] = myRandf(0.000005f * float(inSettings->dInstability) * float(inSettings->dInstability))
			+ 0.000001f * float(inSettings->dInstability) * float(inSettings->dInstability);
	}

	currentSmartConstant = 0;
	oldDistance = 0.0f;
}

flux::~flux()
{
	delete[] particles;
}

void flux::update(FluxSaverSettings * inSettings)
{
	int i;

	// randomize constants
	if(inSettings->dRandomize)
    {
		randomize --;
        
		if(randomize <= 0)
        {
			for(i=0; i<NUMCONSTS; i++)
				c[i] = myRandf(2.0f) - 1.0f;
                
			int temp = 101 - inSettings->dRandomize;
			temp = temp * temp;
			randomize = temp + myRandi(temp);
		}
	}

	// update constants
	for(i=0; i<NUMCONSTS; i++)
    {
		c[i] += cv[i];
		if(c[i] >= 1.0f){
			c[i] = 1.0f;
			cv[i] = -cv[i];
		}
		if(c[i] <= -1.0f){
			c[i] = -1.0f;
			cv[i] = -cv[i];
		}
	}

	// update all particles in this flux field
	float dist;
	for(i=0; i<inSettings->dParticles; i++)
		dist = particles[i].update(c,inSettings);

	// use dist from last particle to activate smart constants
    
	inSettings->dSmart = 0;
    
	if(inSettings->dSmart)
    {
		const float upper = 0.4f;
		const float lower = 0.2f;
		int beSmart = 0;
        
		if(dist > upper && dist > oldDistance)
			beSmart = 1;
            
		if(dist < lower && dist < oldDistance)
			beSmart = 1;
            
		if(beSmart)
        {
			cv[currentSmartConstant] = -cv[currentSmartConstant];
			currentSmartConstant ++;
			if(currentSmartConstant >= inSettings->dSmart)
				currentSmartConstant = 0;
		}
        
		oldDistance = dist;
	}
}


__private_extern__ void draw(FluxSaverSettings * inSettings)
{
	int i;

	// clear the screen
	glLoadIdentity();
	if(inSettings->dBlur)
    {  // partially
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glColor4f(0.0f, 0.0f, 0.0f, 0.5f - (float(sqrt(sqrt(double(inSettings->dBlur)))) * 0.15495f));
		glBegin(GL_TRIANGLE_STRIP);
			glVertex3f(-5.0f, -4.0f, 0.0f);
			glVertex3f(5.0f, -4.0f, 0.0f);
			glVertex3f(-5.0f, 4.0f, 0.0f);
			glVertex3f(5.0f, 4.0f, 0.0f);
		glEnd();
	}
	else  // completely
		glClear(GL_COLOR_BUFFER_BIT);

	inSettings->cameraAngle += 0.01f * float(inSettings->dRotation);
	if(inSettings->cameraAngle >= 360.0f)
		inSettings->cameraAngle -= 360.0f;
        
	if(inSettings->dGeometry == 1)  // Only rotate for spheres
		glRotatef(inSettings->cameraAngle, 0.0f, 1.0f, 0.0f);
	else
    {
		inSettings->cosCameraAngle = cos(inSettings->cameraAngle * DEG2RAD);
		inSettings->sinCameraAngle = sin(inSettings->cameraAngle * DEG2RAD);
	}

	// set up blend modes for rendering particles
	switch(inSettings->dGeometry)
    {
        case 0:  // Blending for points
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            glEnable(GL_BLEND);
            glEnable(GL_POINT_SMOOTH);
            glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
            break;
        case 1:  // No blending for spheres, but we need z-buffering
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            glClear(GL_DEPTH_BUFFER_BIT);
            break;
        case 2:  // Blending for lights
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_BLEND);
	}

	// Update particles
	for(i=0; i<inSettings->dFluxes; i++)
		inSettings->fluxes[i].update(inSettings);
}

__private_extern__ void initSaver(int width,int height,FluxSaverSettings * inSettings)
{
	int i, j;
	float x, y, temp;

	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();

	// Window initialization
	
	glViewport(0,0, width,height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100.0, width / height, 0.01, 200);
	glTranslatef(0.0, 0.0, -2.5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	inSettings->cameraAngle = 0.0f;
    
    if(inSettings->dGeometry == 0)
    {
		glEnable(GL_POINT_SMOOTH);
        if (glIsList(1)) glDeleteLists(1, 1);
		//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	}
    else
    {
        glDisable(GL_POINT_SMOOTH);
    }

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(inSettings->dGeometry == 1)
    {  // Spheres and their lighting
		glNewList(1, GL_COMPILE);
			GLUquadricObj *qobj = gluNewQuadric();
			gluSphere(qobj, 0.005f * float(inSettings->dSize), inSettings->dComplexity + 2, inSettings->dComplexity + 1);
			gluDeleteQuadric(qobj);
		glEndList();

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		float ambient[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		float diffuse[4] = {1.0f, 1.0f, 1.0f, 0.0f};
		float specular[4] = {1.0f, 1.0f, 1.0f, 0.0f};
		float position[4] = {500.0f, 500.0f, 500.0f, 0.0f};
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	}
    else
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
        
        if (glIsList(1)) glDeleteLists(1, 1);
    }

	if(inSettings->dGeometry == 2)
    {  // Init lights
		for(i=0; i<LIGHTSIZE; i++)
        {
			for(j=0; j<LIGHTSIZE; j++)
            {
				x = float(i - LIGHTSIZE / 2) / float(LIGHTSIZE / 2);
				y = float(j - LIGHTSIZE / 2) / float(LIGHTSIZE / 2);
				
                temp = 1.0f - float(sqrt((x * x) + (y * y)));
				
                if(temp > 1.0f)
                {
					temp = 1.0f;
				}
                else
                {
                    if(temp < 0.0f)
                        temp = 0.0f;
				}
                
                inSettings->lightTexture[i][j] = ((unsigned char) 255.0f * temp * temp);
			}
		}
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 1);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, 1, LIGHTSIZE, LIGHTSIZE, 0,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, inSettings->lightTexture);

		temp = float(inSettings->dSize) * 0.005f;
		glNewList(1, GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D, 1);
			glBegin(GL_TRIANGLES);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-temp, -temp, 0.0f);
				glTexCoord2f(1.0f, 0.0f);
				glVertex3f(temp, -temp, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(temp, temp, 0.0f);
				glTexCoord2f(0.0f, 0.0f);
				glVertex3f(-temp, -temp, 0.0f);
				glTexCoord2f(1.0f, 1.0f);
				glVertex3f(temp, temp, 0.0f);
				glTexCoord2f(0.0f, 1.0f);
				glVertex3f(-temp, temp, 0.0f);
			glEnd();
		glEndList();
	}
    else
    {
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

	// Initialize luminosity difference
	inSettings->lumdiff = 1.0f / float(inSettings->dTrail);

	// Initialize flux fields
	inSettings->fluxes = new flux[inSettings->dFluxes](inSettings);
}

__private_extern__ void cleanSettings(FluxSaverSettings * inSettings)
{
	// Free memory
	delete[] inSettings->fluxes;
    
    // A COMPLETER (callList etc...)
}


__private_extern__ void setDefaults(int which,FluxSaverSettings * inSettings)
{
	switch(which)
    {
        case DEFAULTS1:  // Regular
            inSettings->dFluxes = 1;
            inSettings->dParticles = 20;
            inSettings->dTrail = 40;
            inSettings->dGeometry = 2;
            inSettings->dSize = 15;
            inSettings->dRandomize = 0;
            inSettings->dExpansion = 40;
            inSettings->dRotation = 30;
            inSettings->dWind = 20;
            inSettings->dInstability = 20;
            inSettings->dBlur = 0;
            break;
        case DEFAULTS2:  // Hypnotic
            inSettings->dFluxes = 2;
            inSettings->dParticles = 10;
            inSettings->dTrail = 40;
            inSettings->dGeometry = 2;
            inSettings->dSize = 15;
            inSettings->dRandomize = 80;
            inSettings->dExpansion = 20;
            inSettings->dRotation = 0;
            inSettings->dWind = 40;
            inSettings->dInstability = 10;
            inSettings->dBlur = 30;
            break;
        case DEFAULTS3:  // Insane
            inSettings->dFluxes = 4;
            inSettings->dParticles = 30;
            inSettings->dTrail = 8;
            inSettings->dGeometry = 2;
            inSettings->dSize = 25;
            inSettings->dRandomize = 0;
            inSettings->dExpansion = 80;
            inSettings->dRotation = 60;
            inSettings->dWind = 40;
            inSettings->dInstability = 100;
            inSettings->dBlur = 10;
            break;
        case DEFAULTS4:  // Sparklers
            inSettings->dFluxes = 3;
            inSettings->dParticles = 20;
            inSettings->dTrail = 6;
            inSettings->dGeometry = 1;
            inSettings->dSize = 20;
            inSettings->dComplexity = 3;
            inSettings->dRandomize = 85;
            inSettings->dExpansion = 60;
            inSettings->dRotation = 30;
            inSettings->dWind = 20;
            inSettings->dInstability = 30;
            inSettings->dBlur = 0;
            break;
        case DEFAULTS5:  // Paradigm
            inSettings->dFluxes = 1;
            inSettings->dParticles = 40;
            inSettings->dTrail = 40;
            inSettings->dGeometry = 2;
            inSettings->dSize = 5;
            inSettings->dRandomize = 90;
            inSettings->dExpansion = 30;
            inSettings->dRotation = 20;
            inSettings->dWind = 10;
            inSettings->dInstability = 5;
            inSettings->dBlur = 10;
            break;
        case DEFAULTS6:  // Galactic
            inSettings->dFluxes = 1;
            inSettings->dParticles = 2;
            inSettings->dTrail = 1500;
            inSettings->dGeometry = 2;
            inSettings->dSize = 10;
            inSettings->dRandomize = 0;
            inSettings->dExpansion = 5;
            inSettings->dRotation = 25;
            inSettings->dWind = 0;
            inSettings->dInstability = 5;
            inSettings->dBlur = 0;
	}
}
