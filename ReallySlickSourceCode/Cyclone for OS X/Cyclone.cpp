/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Cyclone is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Cyclone is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


// Cyclone screen saver

#include "Cyclone.h"
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "rgbhsl.h"
#include "resource.h"
#include <stdlib.h>

// useful random functions
static inline int myRandi(int x)
{
	return((rand() * x) / RAND_MAX);
}

static inline float myRandf(float x)
{
	return(float(rand() * x) / float(RAND_MAX));
}


// useful vector functions

static float normalize(float *xyz)
{
	float length = float(sqrt(xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2]));
	if(length == 0.0)
		return(0.0);
	xyz[0] /= length;
	xyz[1] /= length;
	xyz[2] /= length;
	return(length);
}

static float dot(float *xyz1, float *xyz2){
	return(xyz1[0] * xyz2[0] + xyz1[1] * xyz2[1] + xyz1[2] * xyz2[2]);
}

static void cross(float *xyz1, float *xyz2, float *xyzOut){
	xyzOut[0] = xyz1[1] * xyz2[2] - xyz2[1] * xyz1[2];
	xyzOut[1] = xyz1[2] * xyz2[0] - xyz2[2] * xyz1[0];
	xyzOut[2] = xyz1[0] * xyz2[1] - xyz2[0] * xyz1[1];
}


// useful factorial function
static int factorial(int x){
    int returnval = 1;

    if(x == 0)
        return(1);
    else{
        do{
            returnval *= x;
            x -= 1;
        }
        while(x!=0);
    }
    return(returnval);
}



cyclone::cyclone(CycloneSaverSettings * inSettings)
{
	int i;
    
    dComplexity=inSettings->dComplexity;
    
	// Initialize position stuff
	targetxyz = new float*[dComplexity+3];
	xyz = new float*[dComplexity+3];
	oldxyz = new float*[dComplexity+3];
	for(i=0; i<int(dComplexity)+3; i++){
		targetxyz[i] = new float[3];
		xyz[i] = new float[3];
		oldxyz[i] = new float[3];
	}
	xyz[dComplexity+2][0] = myRandf(float(wide*2)) - float(wide);
	xyz[dComplexity+2][1] = float(high);
	xyz[dComplexity+2][2] = myRandf(float(wide*2)) - float(wide);
	xyz[dComplexity+1][0] = xyz[dComplexity+2][0];
	xyz[dComplexity+1][1] = myRandf(float(high / 3)) + float(high / 4);
	xyz[dComplexity+1][2] = xyz[dComplexity+2][2];
    
	for(i=dComplexity; i>1; i--)
    {
		xyz[i][0] = xyz[i+1][0] + myRandf(float(wide)) - float(wide / 2);
		xyz[i][1] = myRandf(float(high * 2)) - float(high);
		xyz[i][2] = xyz[i+1][2] + myRandf(float(wide)) - float(wide / 2);
	}
    
	xyz[1][0] = xyz[2][0] + myRandf(float(wide / 2)) - float(wide / 4);
	xyz[1][1] = -myRandf(float(high / 2)) - float(high / 4);
	xyz[1][2] = xyz[2][2] + myRandf(float(wide / 2)) - float(wide / 4);
	xyz[0][0] = xyz[1][0] + myRandf(float(wide / 8)) - float(wide / 16);
	xyz[0][1] = float(-high);
	xyz[0][2] = xyz[1][2] + myRandf(float(wide / 8)) - float(wide / 16);
	// Initialize width stuff
	targetWidth = new float[dComplexity+3];
	width = new float[dComplexity+3];
	oldWidth = new float[dComplexity+3];
	width[dComplexity+2] = myRandf(175.0f) + 75.0f;
	width[dComplexity+1] = myRandf(60.0f) + 15.0f;
	for(i=dComplexity; i>1; i--)
		width[i] = myRandf(25.0f) + 15.0f;
	width[1] = myRandf(25.0f) + 5.0f;
	width[1] = myRandf(15.0f) + 5.0f;
	// Initialize transition stuff
	xyzChange = new int*[dComplexity + 3];
	widthChange = new int*[dComplexity + 3];
    
	for(i=0; i<(dComplexity+3); i++)
    {
		xyzChange[i] = new int[2];	// 0 = step   1 = total steps
		widthChange[i] = new int[2];
		xyzChange[i][0] = 0;
		xyzChange[i][1] = 0;
		widthChange[i][0] = 0;
		widthChange[i][1] = 0;
	}
    
	// Initialize color stuff
    
	oldhsl[0] = myRandf(1.0f);
	oldhsl[1] = myRandf(1.0f);
	oldhsl[2] = 0.0f;
	targethsl[0] = myRandf(1.0f);
	targethsl[1] = myRandf(1.0f);
	targethsl[2] = 1.0f;
	hslChange[0] = 0;
	hslChange[1] = 300;
}

cyclone::~cyclone()
{
    int i;
    
    if (targetxyz!=NULL)
    {
        for(i=0;i<dComplexity;i++)
        {
            free(targetxyz[i]);
        }
        
        free(targetxyz);
    }
    
    if (xyz!=NULL)
    {
        for(i=0;i<dComplexity;i++)
        {
            free(xyz[i]);
        }
        
        free(xyz);
    }
    
    if (oldxyz!=NULL)
    {
        for(i=0;i<dComplexity;i++)
        {
            free(oldxyz[i]);
        }
        
        free(oldxyz);
    }
    
    if (targetWidth!=NULL)
    {
        free(targetWidth);
    }
    
    if (width!=NULL)
    {
        free(width);
    }
    
    if (oldWidth!=NULL)
    {
        free(oldWidth);
    }
    
    if (xyzChange!=NULL)
    {
        for(i=0;i<(dComplexity+3);i++)
        {
            free(xyzChange[i]);
        }
        
        free(xyzChange);
    }
    
    if (widthChange!=NULL)
    {
        for(i=0;i<(dComplexity+3);i++)
        {
            free(widthChange[i]);
        }
        
        free(widthChange);
    }
}

void cyclone::update(CycloneSaverSettings * inSettings){
	int i;
	int temp;
	float between;
	float diff;
	int direction;
	float point[3];
	float step;
	float blend;

	// update cyclone's path
	temp = dComplexity + 2;
	if(xyzChange[temp][0] == xyzChange[temp][1])
    {
		oldxyz[temp][0] = xyz[temp][0];
		oldxyz[temp][1] = xyz[temp][1];
		oldxyz[temp][2] = xyz[temp][2];
		targetxyz[temp][0] = myRandf(float(wide*2)) - float(wide);
		targetxyz[temp][1] = float(high);
		targetxyz[temp][2] = myRandf(float(wide*2)) - float(wide);
		xyzChange[temp][0] = 0;
		xyzChange[temp][1] = myRandi(10000 / inSettings->dSpeed) + 5000 / inSettings->dSpeed;
	}
    
	temp = dComplexity + 1;
    
	if(xyzChange[temp][0] == xyzChange[temp][1])
    {
		oldxyz[temp][0] = xyz[temp][0];
		oldxyz[temp][1] = xyz[temp][1];
		oldxyz[temp][2] = xyz[temp][2];
		targetxyz[temp][0] = xyz[temp+1][0];
		targetxyz[temp][1] = myRandf(float(high / 3)) + float(high / 4);
		targetxyz[temp][2] = xyz[temp+1][2];
		xyzChange[temp][0] = 0;
		xyzChange[temp][1] = myRandi(7000 / inSettings->dSpeed) + 5000 / inSettings->dSpeed;
	}
    
	for(i=dComplexity; i>1; i--)
    {
		if(xyzChange[i][0] == xyzChange[i][1])
        {
			oldxyz[i][0] = xyz[i][0];
			oldxyz[i][1] = xyz[i][1];
			oldxyz[i][2] = xyz[i][2];
            
			targetxyz[i][0] = targetxyz[i+1][0] + (targetxyz[i+1][0] - targetxyz[i+2][0]) / 2.0f + myRandf(float(wide / 2)) - float(wide / 4);
			targetxyz[i][1] = (targetxyz[i+1][1] + targetxyz[i-1][1]) / 2.0f + myRandf(float(high / 8)) - float(high / 16);
			targetxyz[i][2] = targetxyz[i+1][2] + (targetxyz[i+1][2] - targetxyz[i+2][2]) / 2.0f + myRandf(float(wide / 2)) - float(wide / 4);
            
			if(targetxyz[i][1] > high)
				targetxyz[i][1] = high;
			if(targetxyz[i][1] < -high)
				targetxyz[i][1] = -high;
			xyzChange[i][0] = 0;
			xyzChange[i][1] = myRandi(5000 / inSettings->dSpeed) + 3000 / inSettings->dSpeed;
		}
	}
    
	if(xyzChange[1][0] == xyzChange[1][1])
    {
		oldxyz[1][0] = xyz[1][0];
		oldxyz[1][1] = xyz[1][1];
		oldxyz[1][2] = xyz[1][2];
		targetxyz[1][0] = targetxyz[2][0] + myRandf(float(wide / 2)) - float(wide / 4);
		targetxyz[1][1] = -myRandf(float(high / 2)) - float(high / 4);
		targetxyz[1][2] = targetxyz[2][2] + myRandf(float(wide / 2)) - float(wide / 4);
		xyzChange[1][0] = 0;
		xyzChange[1][1] = myRandi(4000 / inSettings->dSpeed) + 2000 / inSettings->dSpeed;
	}
    
	if(xyzChange[0][0] == xyzChange[0][1])
    {
		oldxyz[0][0] = xyz[0][0];
		oldxyz[0][1] = xyz[0][1];
		oldxyz[0][2] = xyz[0][2];
		targetxyz[0][0] = xyz[1][0] + myRandf(float(wide / 8)) - float(wide / 16);
		targetxyz[0][1] = float(-high);
		targetxyz[0][2] = xyz[1][2] + myRandf(float(wide / 8)) - float(wide / 16);
		xyzChange[0][0] = 0;
		xyzChange[0][1] = myRandi(3000 / inSettings->dSpeed) + 1500 / inSettings->dSpeed;
	}
    
	for(i=0; i<(dComplexity+3); i++)
    {
		between = float(xyzChange[i][0]) / float(xyzChange[i][1]) * PIx2;
		between = (1.0f - float(cos(between))) / 2.0f; 
		xyz[i][0] = ((targetxyz[i][0] - oldxyz[i][0]) * between) + oldxyz[i][0];
		xyz[i][1] = ((targetxyz[i][1] - oldxyz[i][1]) * between) + oldxyz[i][1];
		xyz[i][2] = ((targetxyz[i][2] - oldxyz[i][2]) * between) + oldxyz[i][2];
		xyzChange[i][0] ++;
	}

	// Update cyclone's widths
    
	temp = dComplexity + 2;
    
	if(widthChange[temp][0] == widthChange[temp][1])
    {
		oldWidth[temp] = width[temp];
		targetWidth[temp] = myRandf(225.0f) + 75.0f;
		widthChange[temp][0] = 0;
		widthChange[temp][1] = myRandi(5000 / inSettings->dSpeed) + 5000 / inSettings->dSpeed;
	}
    
	temp = dComplexity + 1;
    
	if(widthChange[temp][0] == widthChange[temp][1])
    {
		oldWidth[temp] = width[temp];
		targetWidth[temp] = myRandf(80.0f) + 15.0f;
		widthChange[temp][0] = 0;
		widthChange[temp][1] = myRandi(5000 / inSettings->dSpeed) + 5000 / inSettings->dSpeed;
	}
    
	for(i=dComplexity; i>1; i--)
    {
		if(widthChange[i][0] == widthChange[i][1])
        {
			oldWidth[i] = width[i];
			targetWidth[i] = myRandf(25.0f) + 15.0f;
			widthChange[i][0] = 0;
			widthChange[i][1] = myRandi(5000 / inSettings->dSpeed) + 4000 / inSettings->dSpeed;
		}
	}
    
	if(widthChange[1][0] == widthChange[1][1])
    {
		oldWidth[1] = width[1];
		targetWidth[1] = myRandf(25.0f) + 5.0f;
		widthChange[1][0] = 0;
		widthChange[1][1] = myRandi(5000 / inSettings->dSpeed) + 3000 / inSettings->dSpeed;
	}
    
	if(widthChange[0][0] == widthChange[0][1]){
		oldWidth[0] = width[0];
		targetWidth[0] = myRandf(15.0f) + 5.0f;
		widthChange[0][0] = 0;
		widthChange[0][1] = myRandi(5000 / inSettings->dSpeed) + 2000 / inSettings->dSpeed;
	}
	for(i=0; i<(dComplexity+3); i++){
		between = float(widthChange[i][0]) / float(widthChange[i][1]);
		width[i] = ((targetWidth[i] - oldWidth[i]) * between) + oldWidth[i];
		widthChange[i][0] ++;
	}

	// Update cyclones color
    
	if(hslChange[0] == hslChange[1])
    {
		oldhsl[0] = hsl[0];
		oldhsl[1] = hsl[1];
		oldhsl[2] = hsl[2];
		targethsl[0] = myRandf(1.0f);
		targethsl[1] = myRandf(1.0f);
		targethsl[2] = myRandf(1.0f) + 0.5f;
		if(targethsl[2] > 1.0f)
			targethsl[2] = 1.0f;
		hslChange[0] = 0;
		hslChange[1] = myRandi(1900) + 100;
	}
    
	between = float(hslChange[0]) / float(hslChange[1]);
	diff = targethsl[0] - oldhsl[0];
	direction = 0;
	if((targethsl[0] > oldhsl[0] && diff > 0.5f) || (targethsl[0] < oldhsl[0] && diff < -0.5f))
		if(diff > 0.5f)
			direction = 1;
	hslTween(oldhsl[0], oldhsl[1], oldhsl[2],
			targethsl[0], targethsl[1], targethsl[2], between, direction,
			hsl[0], hsl[1], hsl[2]);
	hslChange[0] ++;

	if(inSettings->dShowCurves)
    {
		glDisable(GL_LIGHTING);
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINE_STRIP);
		for(step=0.0; step<1.0; step+=0.02f)
        {
			point[0] = point[1] = point[2] = 0.0f;
            
			for(i=0; i<(dComplexity+3); i++)
            {
				blend = inSettings->fact[dComplexity+2] / (inSettings->fact[i]
					* inSettings->fact[dComplexity+2-i]) * pow(step, float(i))
					* pow((1.0f - step), float(dComplexity+2-i));
				point[0] += xyz[i][0] * blend;
				point[1] += xyz[i][1] * blend;
				point[2] += xyz[i][2] * blend;
			}
			glVertex3fv(point);
		}
		glEnd();
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINE_STRIP);
		for(i=0; i<(dComplexity+3); i++)
			glVertex3fv(&xyz[i][0]);
		glEnd();
		glEnable(GL_LIGHTING);
	}
}

particle::particle(cyclone *c)
{
	cy = c;
	init();
}

void particle::init()
{
	width = myRandf(0.8f) + 0.2f;
	step = 0.0f;
	spinAngle = myRandf(360);
	hsl2rgb(cy->hsl[0], cy->hsl[1], cy->hsl[2], r, g, b);
}

void particle::update(CycloneSaverSettings * inSettings)
{
	int i;
	float scale=0, temp;
	float newStep;
	float newSpinAngle;
	float cyWidth;
	float between;
	float dir[3];
	float crossVec[3];
	float tiltAngle;
	float up[3] = {0.0f, 1.0f, 0.0f};
	float blend;
    int dComplexity=inSettings->dComplexity;
    
	lastxyz[0] = xyz[0];
	lastxyz[1] = xyz[1];
	lastxyz[2] = xyz[2];
	if(step > 1.0f)
		init();
	xyz[0] = xyz[1] = xyz[2] = 0.0f;
    
	for(i=0; i<(dComplexity+3); i++)
    {
		blend = inSettings->fact[dComplexity+2] / (inSettings->fact[i]
			* inSettings->fact[dComplexity+2-i]) * pow(step, float(i))
			* pow((1.0f - step), float(dComplexity+2-i));
		xyz[0] += cy->xyz[i][0] * blend;
		xyz[1] += cy->xyz[i][1] * blend;
		xyz[2] += cy->xyz[i][2] * blend;
	}
    
	dir[0] = dir[1] = dir[2] = 0.0f;
    
	for(i=0; i<(dComplexity+3); i++)
    {
		blend = inSettings->fact[dComplexity+2] / (inSettings->fact[i]
			* inSettings->fact[dComplexity+2-i]) * pow(step - 0.01f, float(i))
			* pow((1.0f - (step - 0.01f)), float(dComplexity+2-i));
		dir[0] += cy->xyz[i][0] * blend;
		dir[1] += cy->xyz[i][1] * blend;
		dir[2] += cy->xyz[i][2] * blend;
	}
    
	dir[0] = xyz[0] - dir[0];
	dir[1] = xyz[1] - dir[1];
	dir[2] = xyz[2] - dir[2];
	normalize(dir);
	cross(dir, up, crossVec);
	tiltAngle = -acos(dot(dir, up)) * 180.0f / PI;
	i = int(step * (float(dComplexity) + 2.0f));
	if(i >= (dComplexity + 2))
		i = dComplexity + 1;
	between = (step - (float(i) / float(dComplexity + 2))) * float(dComplexity + 2);
	cyWidth = cy->width[i] * (1.0f - between) + cy->width[i+1] * (between);
	newStep = (0.005f * float(inSettings->dSpeed)) / (width * width * cyWidth);
	step += newStep;
	newSpinAngle = (40.0f * float(inSettings->dSpeed)) / (width * cyWidth);
	spinAngle += newSpinAngle;
    
	if(inSettings->dStretch)
    {
		scale = width * cyWidth * newSpinAngle * 0.02f;
		temp = cyWidth * 2.0f / float(inSettings->dSize);
		if(scale > temp)
			scale = temp;
		if(scale < 1.0f)
			scale = 1.0f;
	}
	glColor3f(r, g, b);
	glPushMatrix();
		glLoadIdentity();
		glTranslatef(xyz[0], xyz[1], xyz[2]);
		glRotatef(tiltAngle, crossVec[0], crossVec[1], crossVec[2]);
		glRotatef(spinAngle, 0, 1, 0);
		glTranslatef(width * cyWidth, 0, 0);
		if(inSettings->dStretch)
			glScalef(1.0f, 1.0f, scale);
		glCallList(1);
	glPopMatrix();
}


__private_extern__ void draw(CycloneSaverSettings * inSettings)
{
	int i, j;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(i=0; i<inSettings->dCyclones; i++)
    {
		inSettings->cyclones[i]->update(inSettings);
		
        for(j=(i * inSettings->dParticles); j<((i+1) * inSettings->dParticles); j++)
        {
			inSettings->particles[j]->update(inSettings);
        }
    }
}


__private_extern__ void initSaver(int width, int height,CycloneSaverSettings * inSettings)
{
	int i, j;

	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand(); rand(); rand(); rand(); rand(); rand();
    
	glViewport(0,0,width,height);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0, float(width) / float(height), 50, 3000);
    
	if(!myRandi(500))
    {  // Easter egg view
		glRotatef(90, 1, 0, 0);
		glTranslatef(0.0f, -(wide * 2), 0.0f);
	}
	else  // Normal view
		glTranslatef(0.0f, 0.0f, -(wide * 2));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (glIsList(1))
    {
        glDeleteLists(1,1);
    }
    
    glNewList(1, GL_COMPILE);
		GLUquadricObj *qobj = gluNewQuadric();
		gluSphere(qobj, float(inSettings->dSize) / 4.0f, 3, 2);
		gluDeleteQuadric(qobj);
	glEndList();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float ambient[4] = {0.25f, 0.25f, 0.25f, 0.0f};
	float diffuse[4] = {1.0f, 1.0f, 1.0f, 0.0f};
	float specular[4] = {1.0f, 1.0f, 1.0f, 0.0f};
	float position[4] = {float(wide * 2), -float(high), float(wide * 2), 0.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_COLOR_MATERIAL);
	glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);
	glColorMaterial(GL_FRONT, GL_SPECULAR);
	glColor3f(0.7f, 0.7f, 0.7f);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Initialize cyclones and their particles
	for(i=0; i<13; i++)
		inSettings->fact[i] = float(factorial(i));
        
	inSettings->cyclones = new cyclone*[inSettings->dCyclones];
	inSettings->particles = new particle*[inSettings->dParticles * inSettings->dCyclones];
    
	for(i=0; i<inSettings->dCyclones; i++)
    {
		inSettings->cyclones[i] = new cyclone(inSettings);
		for(j=i*inSettings->dParticles; j<((i+1)*inSettings->dParticles); j++)
			inSettings->particles[j] = new particle(inSettings->cyclones[i]);
	}
}

__private_extern__ void setDefaults(CycloneSaverSettings * inSettings)
{
	inSettings->dCyclones = 1;
	inSettings->dParticles = 200;
	inSettings->dSize = 7;
	inSettings->dComplexity = 3;
	inSettings->dSpeed = 10;
	inSettings->dStretch = 1;
	inSettings->dShowCurves = 0;
}

__private_extern__ void cleanSettings(CycloneSaverSettings * inSettings)
{
    if (inSettings->cyclones!=NULL)
    {
        int i,j;
        
        for(i=0; i<inSettings->dCyclones; i++)
        {
            delete(inSettings->cyclones[i]);
            
            for(j=i*inSettings->dParticles; j<((i+1)*inSettings->dParticles); j++)
            {
                delete(inSettings->particles[j]);
            }
        }
        
        delete(inSettings->cyclones);
        delete(inSettings->particles);
        
        inSettings->cyclones=NULL;
        inSettings->particles=NULL;
    }
}