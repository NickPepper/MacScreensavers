/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Flocks is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Flocks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


// Flocks screensaver

#include <stdio.h>
#include <math.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <time.h>
#include "rgbhsl.h"
#include <stdlib.h>
#include "Flocks.h"

#define R2D 57.2957795131f

// useful random functions
static int myRandi(int x)
{
    return ((rand() * x) / RAND_MAX);
}

static float myRandf(float x)
{
    return ((rand() *x*((double) 1.0)) / RAND_MAX);
}

bug::bug()
{
}

bug::~bug()
{
}

void bug::initLeader(FlocksSaverSettings * inSettings)
{
	type = 0;
	h = myRandf(1.0);
	s = 1.0f;
	l = 1.0f;
	x = myRandf(inSettings->wide * 2) - inSettings->wide;
	y = myRandf(inSettings->high * 2) - inSettings->high;
	z = myRandf(inSettings->wide * 2) + inSettings->wide * 2;
	right = up = forward = 1;
	xSpeed = ySpeed = zSpeed = 0.0f;
	maxSpeed = 8.0f * inSettings->dSpeed;
	accel = 13.0f * inSettings->dSpeed;
	craziness = myRandf(4.0f) + 0.05f;
	nextChange = 1.0f;
}

void bug::initFollower(FlocksSaverSettings * inSettings)
{
	type = 1;
	h = myRandf(1.0);
	s = 1.0f;
	l = 1.0f;
	x = myRandf((inSettings->wide * 2)) - inSettings->wide;
	y = myRandf((inSettings->high * 2)) - inSettings->high;
	z = myRandf((inSettings->wide * 5)) + inSettings->wide * 2;
	right = up = forward = 0;
	xSpeed = ySpeed = zSpeed = 0.0f;
	maxSpeed = (myRandf(6.0f) + 4.0f) * float(inSettings->dSpeed);
	accel = (myRandf(4.0f) + 9.0f) * float(inSettings->dSpeed);
	leader = 0;
}

void bug::update(bug *bugs,FlocksSaverSettings * inSettings)
{
	int i;
	float scale[4];
    float elapsedTime=inSettings->elapsedTime;
    
	if(!type)
    {  // leader
		nextChange -= elapsedTime;
        
		if(nextChange <= 0.0f)
        {
			if(myRandi(2))
				right ++;
			if(myRandi(2))
				up ++;
			if(myRandi(2))
				forward ++;
			if(right >= 2)
				right = 0;
			if(up >= 2)
				up = 0;
			if(forward >= 2)
				forward = 0;
			nextChange = myRandf(craziness);
		}
        
		if(right)
			xSpeed += accel * elapsedTime;
		else
			xSpeed -= accel * elapsedTime;
		if(up)
			ySpeed += accel * elapsedTime;
		else
			ySpeed -= accel * elapsedTime;
		if(forward)
			zSpeed -= accel * elapsedTime;
		else
			zSpeed += accel * elapsedTime;
		if(x < (-inSettings->wide))
			right = 1;
		if(x > inSettings->wide)
			right = 0;
		if(y < (-inSettings->high))
			up = 1;
		if(y > inSettings->high)
			up = 0;
		if(z < (-inSettings->deep))
			forward = 0;
		if(z > inSettings->deep)
			forward = 1;
		// Even leaders change color from Chromatek 3D
		if(inSettings->dChromatek)
        {
			h = 0.666667f * ((inSettings->wide - z) / (inSettings->wide + inSettings->wide));
			if(h > 0.666667f)
				h = 0.666667f;
			if(h < 0.0f)
				h = 0.0f;
		}
	}
	else
    {  // follower
		if(!myRandi(10))
        {
			float oldDistance = 10000000.0f, newDistance;
            bug tBug;
            
			for(i=0; i<inSettings->dLeaders; i++)
            {
				tBug=bugs[i];
                
                newDistance = ((tBug.x - x) * (tBug.x - x)
							+ (tBug.y - y) * (tBug.y - y)
							+ (tBug.z - z) * (tBug.z - z));
                            
				if(newDistance < oldDistance)
                {
					oldDistance = newDistance;
					leader = i;
				}
			}
		}
		if((bugs[leader].x - x) > 0.0f)
			xSpeed += accel * elapsedTime;
		else
			xSpeed -= accel * elapsedTime;
            
		if((bugs[leader].y - y) > 0.0f)
			ySpeed += accel * elapsedTime;
		else
			ySpeed -= accel * elapsedTime;
            
		if((bugs[leader].z - z) > 0.0f)
			zSpeed += accel * elapsedTime;
		else
			zSpeed -= accel * elapsedTime;
            
		if(inSettings->dChromatek)
        {
			h = 0.666667f * ((inSettings->wide - z) / (inSettings->wide + inSettings->wide));
            
			if(h > 0.666667f)
            {
				h = 0.666667f;
			}
            else
            {
                if(h < 0.0f)
                    h = 0.0f;
            }
        }
		else
        {
			if(fabs(h - bugs[leader].h) < (inSettings->colorFade * elapsedTime))
				h = bugs[leader].h;
			else
            {
				if(fabs(h - bugs[leader].h) < 0.5f)
                {
					if(h > bugs[leader].h)
						h -= inSettings->colorFade * elapsedTime;
					else
						h += inSettings->colorFade * elapsedTime;
				}
				else
                {
					if(h > bugs[leader].h)
						h += inSettings->colorFade * elapsedTime;
					else
						h -= inSettings->colorFade * elapsedTime;
					if(h > 1.0f)
						h -= 1.0f;
					else
                    {
                        if(h < 0.0f)
                            h += 1.0f;
                    }
                }
			}
		}
	}

	if(xSpeed > maxSpeed)
		xSpeed = maxSpeed;
    else
	{
        if(xSpeed < -maxSpeed)
            xSpeed = -maxSpeed;
	}
    if(ySpeed > maxSpeed)
		ySpeed = maxSpeed;
	else
    {
        if(ySpeed < -maxSpeed)
            ySpeed = -maxSpeed;
	}
    
    if(zSpeed > maxSpeed)
		zSpeed = maxSpeed;
	else
    {
        if(zSpeed < -maxSpeed)
            zSpeed = -maxSpeed;
	}
    
	x += xSpeed * elapsedTime;
	y += ySpeed * elapsedTime;
	z += zSpeed * elapsedTime;
    
	if(inSettings->dStretch)
    {
		scale[0] = xSpeed * 0.04f;
		scale[1] = ySpeed * 0.04f;
		scale[2] = zSpeed * 0.04f;
		scale[3] = scale[0] * scale[0] + scale[1] * scale[1] + scale[2] * scale[2];
        
		if(scale[3] > 0.0f)
        {
			scale[3] = sqrt(scale[3]);
			scale[0] /= scale[3];
			scale[1] /= scale[3];
			scale[2] /= scale[3];
		}
	}
	hsl2rgb(h, s, l, r, g, b);
	halfr = r * 0.5f;
	halfg = g * 0.5f;
	halfb = b * 0.5f;
    
	glColor3f(r, g, b);
    
	if(inSettings->dGeometry)
    {  // Draw blobs
		glPushMatrix();
			glTranslatef(x, y, z);
			if(inSettings->dStretch)
            {
				scale[3] *= inSettings->dStretch * 0.05f;
				if(scale[3] < 1.0f)
					scale[3] = 1.0f;
				glRotatef(atan2(-scale[0], -scale[2]) * R2D, 0.0f, 1.0f, 0.0f);
				glRotatef(asin(scale[1]) * R2D, 1.0f, 0.0f, 0.0f);
				glScalef(1.0f, 1.0f, scale[3]);
			}
			glCallList(1);
		glPopMatrix();
	}
	else
    {  // Draw dots
		if(inSettings->dStretch)
        {
			glLineWidth(inSettings->dSize * (700 - z) * 0.0002f);
			scale[0] *= inSettings->dStretch;
			scale[1] *= inSettings->dStretch;
			scale[2] *= inSettings->dStretch;
			glBegin(GL_LINES);
				glVertex3f(x - scale[0], y - scale[1], z - scale[2]);
				glVertex3f(x + scale[0], y + scale[1], z + scale[2]);
			glEnd();
		}
		else
        {
			glPointSize(inSettings->dSize * (700 - z) * 0.001f);
			glBegin(GL_POINTS);
				glVertex3f(x, y, z);
			glEnd();
		}
	}

	if(inSettings->dConnections && type)
    {  // draw connections
		glLineWidth(1.0f);
		glBegin(GL_LINES);
			glColor3f(halfr, halfg, halfb);
			glVertex3f(x, y, z);
			glColor3f(bugs[leader].halfr, bugs[leader].halfg, bugs[leader].halfb);
			glVertex3f(bugs[leader].x, bugs[leader].y, bugs[leader].z);
		glEnd();
	}
}


__private_extern__ void draw(PlasmaSaverSettings * inSettings)
{
	int i;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0, inSettings->aspectRatio, 50, 2000);
	glTranslatef(0.0, 0.0, -inSettings->wide * 2.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update and draw leaders
	for(i=0; i<inSettings->dLeaders; i++)
		inSettings->lBugs[i].update(inSettings->lBugs,inSettings);
        
	// Update and draw followers
	for(i=0; i<inSettings->dFollowers; i++)
		inSettings->fBugs[i].update(inSettings->lBugs,inSettings);
}


__private_extern__ void initSaver(int width,int height,PlasmaSaverSettings * inSettings)
{
	int i;

	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();

	// Window initialization

	inSettings->aspectRatio = (1.0*width) / height;
	glViewport(0, 0, width, height);
	
	// calculate boundaries
	if (width > height)
    {
		inSettings->high = inSettings->deep = 160;
		inSettings->wide = (inSettings->high * width) / height;
	}
	else
    {
		inSettings->wide = inSettings->deep = 160;
		inSettings->high = (inSettings->wide * height) / width;
	}

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	if(inSettings->dGeometry)
    {  // Setup lights and build blobs
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		float ambient[4] = {0.25f, 0.25f, 0.25f, 0.0f};
		float diffuse[4] = {1.0f, 1.0f, 1.0f, 0.0f};
		float specular[4] = {1.0f, 1.0f, 1.0f, 0.0f};
		float position[4] = {500.0f, 500.0f, 500.0f, 0.0f};
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glEnable(GL_COLOR_MATERIAL);
		glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);
		glColorMaterial(GL_FRONT, GL_SPECULAR);
		glColor3f(0.7f, 0.7f, 0.7f);
		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

		glNewList(1, GL_COMPILE);
			GLUquadricObj *qobj = gluNewQuadric();
			gluSphere(qobj, inSettings->dSize * 0.5f, inSettings->dComplexity + 2, inSettings->dComplexity + 1);
			gluDeleteQuadric(qobj);
		glEndList();
	}
    else
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
        
        if (glIsList(1)) glDeleteLists(1, 1);
    }

	inSettings->lBugs = new bug[inSettings->dLeaders];
	inSettings->fBugs = new bug[inSettings->dFollowers];
    
	for(i=0; i<inSettings->dLeaders; i++)
		inSettings->lBugs[i].initLeader(inSettings);
        
	for(i=0; i<inSettings->dFollowers; i++)
		inSettings->fBugs[i].initFollower(inSettings);

	inSettings->colorFade = inSettings->dColorfadespeed * 0.01f;
}

__private_extern__ void cleanSettings(PlasmaSaverSettings * inSettings)
{
    if (inSettings->lBugs!=NULL)
    {
        delete[] inSettings->lBugs;
        
        inSettings->lBugs=NULL;
    }
    
    if (inSettings->fBugs!=NULL)
    {
        delete[] inSettings->fBugs;
        
        inSettings->fBugs=NULL;
    }
}

__private_extern__ void setDefaults(FlocksSaverSettings * inSettings)
{
	inSettings->dLeaders = 4;
	inSettings->dFollowers = 400;
	inSettings->dGeometry = 1;
	inSettings->dSize = 10;
	inSettings->dComplexity = 1;
	inSettings->dSpeed = 15;
	inSettings->dStretch = 20;
	inSettings->dColorfadespeed = 15;
	inSettings->dChromatek = (1==0);
	inSettings->dConnections = (1==0);
}