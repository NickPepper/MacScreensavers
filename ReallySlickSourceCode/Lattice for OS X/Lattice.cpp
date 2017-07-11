/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Lattice is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Lattice is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


// Lattice screensaver

#include "Lattice.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "texture.h"

#include "camera.h"
#include "resource.h"

// Globals



static int transitions[120] = {
        1, 2, 12, 4, 14, 8,
        0, 3, 15, 7, 7, 7,
        3, 4, 14, 0, 7, 16,
        2, 1, 15, 7, 7, 7,
        5, 10, 12, 17, 17, 17,
        4, 3, 13, 11, 9, 17,
        12, 4, 10, 17, 17, 17,
        2, 0, 14, 8, 16, 19,
        1, 3, 15, 7, 7, 7,
        4, 10, 12, 17, 17, 17,
        11, 4, 12, 17, 17, 17,
        10, 5, 15, 13, 17, 18,
        13, 10, 4, 17, 17, 17,
        12, 1, 11, 5, 6, 17,
        15, 2, 12, 0, 7, 19,
        14, 3, 1, 7, 7, 7,
        3, 1, 15, 7, 7, 7,
        5, 11, 13, 6, 9, 18,
        10, 4, 12, 17, 17, 17,
        15, 1, 3, 7, 7, 7};


static int segments;
//float cullVec[5][3];  // represent planes for culling geometry

// Modulus function for picking the correct element of lattice array
__private_extern__ int myMod(int x)
{
	while(x < 0)
		x += LATSIZE;
	return(x % LATSIZE);
}


// start point, start slope, end point, end slope, position (0.0 - 1.0)
// returns point somewhere along a smooth curve between the start point
// and end point

__private_extern__ float interpolate(float a, float b, float c, float d, float where)
{
	float q = 2.0f * (a - c) + b + d;
	float r = 3.0f * (c - a) - 2.0f * b - d;
    
	return((where * where * where * q) + (where * where * r) + (where * b) + a);
}


__private_extern__ void initTextures(LatticeSaverSettings * inSettings)
{
	switch(inSettings->dUsedTexture)
    {
        case LATTICE_TEXTURE_INDUSTRIAL:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXSIZE, TEXSIZE, GL_RGB,
                GL_UNSIGNED_BYTE, indtex1);
    
            glBindTexture(GL_TEXTURE_2D, 2);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXSIZE, TEXSIZE, GL_RGB,
                GL_UNSIGNED_BYTE, indtex2);
            break;
        case LATTICE_TEXTURE_CRYSTAL:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXSIZE, TEXSIZE, GL_RGB,
                GL_UNSIGNED_BYTE, crystex);
            break;
        case LATTICE_TEXTURE_CHROME:
        	glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXSIZE, TEXSIZE, GL_RGB,
                GL_UNSIGNED_BYTE, chrometex);
            break;
        case LATTICE_TEXTURE_BRASS:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TEXSIZE, TEXSIZE, GL_RGB,
                GL_UNSIGNED_BYTE, brasstex);
            break;
		case LATTICE_TEXTURE_SHINY:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 4, TEXSIZE, TEXSIZE, GL_RGBA,
                GL_UNSIGNED_BYTE, shinytex);
            break;
        case LATTICE_TEXTURE_GHOSTLY:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_ALPHA, TEXSIZE, TEXSIZE, GL_ALPHA, 
                GL_UNSIGNED_BYTE, ghostlytex);
            break;
        case LATTICE_TEXTURE_CIRCUITS:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, GL_ALPHA, TEXSIZE, TEXSIZE, GL_ALPHA, 
                GL_UNSIGNED_BYTE, circuittex);
            break;
        case LATTICE_TEXTURE_DOUGHNUTS:
            glBindTexture(GL_TEXTURE_2D, 1);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gluBuild2DMipmaps(GL_TEXTURE_2D, 4, TEXSIZE, TEXSIZE, GL_RGBA,
                GL_UNSIGNED_BYTE, doughtex);
            break;
    }
}


__private_extern__ void makeTorus(int smooth, int longitude, int latitude, float centerradius, float thickradius)
{
	int i, j;
	float r, rr;  // Radius
	float z, zz;  // Depth
	float cosa, sina;  // Longitudinal positions
	float cosn, cosnn, sinn, sinnn;  // Normals for shading
	float ncosa, nsina;  // Longitudinal positions for shading
	float u, v1, v2, ustep, vstep;
	float temp;
	float oldcosa, oldsina, oldncosa, oldnsina, oldcosn, oldcosnn, oldsinn, oldsinnn;

	// Smooth shading?
	if(smooth)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	// Initialize texture stuff
	vstep = 1.0f / float(latitude);
	ustep = float(int((centerradius / thickradius) + 0.5f)) / float(longitude);
	v2 = 0.0f;

	for(i=0; i<latitude; i++){
		temp = PIx2 * float(i) / float(latitude);
		cosn = cos(temp);
		sinn = sin(temp);
		temp = PIx2 * float(i+1) / float(latitude);
		cosnn = cos(temp);
		sinnn = sin(temp);
		r = centerradius + thickradius * cosn;
		rr = centerradius + thickradius * cosnn;
		z = thickradius * sinn;
		zz = thickradius * sinnn;
		if(!smooth){  // Redefine normals for flat shaded model
			temp = PIx2 * (float(i) + 0.5f) / float(latitude);
			cosn = cosnn = cos(temp);
			sinn = sinnn = sin(temp);
		}
		v1 = v2;
		v2 += vstep;
		u = 0.0f;
		glBegin(GL_TRIANGLE_STRIP);
			for(j=0; j<longitude; j++){
				temp = PIx2 * float(j) / float(longitude);
				cosa = cos(temp);
				sina = sin(temp);
				if(smooth){
					ncosa = cosa;
					nsina = sina;
				}
				else{  // Redefine longitudinal component of normal for flat shading
					temp = PIx2 * (float(j) - 0.5f) / float(longitude);
					ncosa = cos(temp);
					nsina = sin(temp);
				}
				if(j==0){  // Save first values for end of circular tri-strip
					oldcosa = cosa;
					oldsina = sina;
					oldncosa = ncosa;
					oldnsina = nsina;
					oldcosn = cosn;
					oldcosnn = cosnn;
					oldsinn = sinn;
					oldsinnn = sinnn;
				}
				glNormal3f(cosnn * ncosa, cosnn * nsina, sinnn);
				glTexCoord2f(u, v2);
				glVertex3f(cosa * rr, sina * rr, zz);
				glNormal3f(cosn * ncosa, cosn * nsina, sinn);
				glTexCoord2f(u, v1);
				glVertex3f(cosa * r, sina * r, z);
				u += ustep;  // update u texture coordinate
			}
			//  Finish off circular tri-strip with saved first values
			glNormal3f(oldcosnn * oldncosa, oldcosnn * oldnsina, oldsinnn);
			glTexCoord2f(u, v2);
			glVertex3f(oldcosa * rr, oldsina * rr, zz);
			glNormal3f(oldcosn * oldncosa, oldcosn * oldnsina, oldsinn);
			glTexCoord2f(u, v1);
			glVertex3f(oldcosa * r, oldsina * r, z);
		glEnd();
	}
}


//  Build the lattice display lists
__private_extern__ void makeLatticeObjects(LatticeSaverSettings * inSettings)
{
	int i, d = 0;
	float thick = float(inSettings->dThick) * 0.001f;

	if(inSettings->dUsedTexture > LATTICE_TEXTURE_NONE && inSettings->dUsedTexture < LATTICE_TEXTURE_DOUGHNUTS)
		glColor3f(1.0f, 1.0f, 1.0f);

	for(i=1; i<=NUMOBJECTS; i++)
    {
		glNewList(i, GL_COMPILE);
			if(inSettings->dUsedTexture >= LATTICE_TEXTURE_CRYSTAL)
				glBindTexture(GL_TEXTURE_2D, 1);
                
			if(d < inSettings->dDensity)
            {
				glPushMatrix();
					if(inSettings->dUsedTexture == LATTICE_TEXTURE_NONE || inSettings->dUsedTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(myRandf(1.0f), myRandf(1.0f), myRandf(1.0f));
					else
                    if(inSettings->dUsedTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, myRandi(2)+1);
					glTranslatef(-0.25f, -0.25f, -0.25f);
					if(myRandi(2))
						glRotatef(180.0f, 1, 0, 0);
					makeTorus(inSettings->dSmooth, inSettings->dLongitude, inSettings->dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
			d = (d + 37) % 100;
			if(d < inSettings->dDensity)
            {
				glPushMatrix();
					if(inSettings->dUsedTexture == LATTICE_TEXTURE_NONE || inSettings->dUsedTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(myRandf(1.0f), myRandf(1.0f), myRandf(1.0f));
					else
                    if(inSettings->dUsedTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, myRandi(2)+1);
					glTranslatef(0.25f, -0.25f, -0.25f);
					if(myRandi(2))
						glRotatef(90.0f, 1, 0, 0);
					else
						glRotatef(-90.0f, 1, 0, 0);
					makeTorus(inSettings->dSmooth, inSettings->dLongitude, inSettings->dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
			d = (d + 37) % 100;
			if(d < inSettings->dDensity)
            {
				glPushMatrix();
					if(inSettings->dUsedTexture == LATTICE_TEXTURE_NONE || inSettings->dUsedTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(myRandf(1.0f), myRandf(1.0f), myRandf(1.0f));
					else
                    if(inSettings->dUsedTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, myRandi(2)+1);
					glTranslatef(0.25f, -0.25f, 0.25f);
					if(myRandi(2))
						glRotatef(90.0f, 0, 1, 0);
					else
						glRotatef(-90.0f, 0, 1, 0);
					makeTorus(inSettings->dSmooth, inSettings->dLongitude, inSettings->dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
			d = (d + 37) % 100;
			if(d < inSettings->dDensity)
            {
				glPushMatrix();
					if(inSettings->dUsedTexture == LATTICE_TEXTURE_NONE || inSettings->dUsedTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(myRandf(1.0f), myRandf(1.0f), myRandf(1.0f));
					else
                    if(inSettings->dUsedTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, myRandi(2)+1);
					glTranslatef(0.25f, 0.25f, 0.25f);
					if(myRandi(2))
						glRotatef(180.0f, 1, 0, 0);
					makeTorus(inSettings->dSmooth, inSettings->dLongitude, inSettings->dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
			d = (d + 37) % 100;
			if(d < inSettings->dDensity)
            {
				glPushMatrix();
					if(inSettings->dUsedTexture == LATTICE_TEXTURE_NONE || inSettings->dUsedTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(myRandf(1.0f), myRandf(1.0f), myRandf(1.0f));
					else
                    if(inSettings->dUsedTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, myRandi(2)+1);
					glTranslatef(-0.25f, 0.25f, 0.25f);
					if(myRandi(2))
						glRotatef(90.0f, 1, 0, 0);
					else
						glRotatef(-90.0f, 1, 0, 0);
					makeTorus(inSettings->dSmooth, inSettings->dLongitude, inSettings->dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
			d = (d + 37) % 100;
			if(d < inSettings->dDensity)
            {
				glPushMatrix();
					if(inSettings->dUsedTexture == LATTICE_TEXTURE_NONE || inSettings->dUsedTexture >= LATTICE_TEXTURE_SHINY)
						glColor3f(myRandf(1.0f), myRandf(1.0f), myRandf(1.0f));
					else
                    if(inSettings->dUsedTexture == LATTICE_TEXTURE_INDUSTRIAL)
						glBindTexture(GL_TEXTURE_2D, myRandi(2)+1);
					glTranslatef(-0.25f, 0.25f, -0.25f);
					if(myRandi(2))
						glRotatef(90.0f, 0, 1, 0);
					else
						glRotatef(-90.0f, 0, 1, 0);
					makeTorus(inSettings->dSmooth, inSettings->dLongitude, inSettings->dLatitude, 0.36f - thick, thick);
				glPopMatrix();
			}
		glEndList();
		d = (d + 37) % 100;
	}
}

__private_extern__ void reconfigure(LatticeSaverSettings * inSettings)
{
	int i, j;
	int newBorder, positive;
    float **path=inSettings->path;
    int * globalxyz=inSettings->globalxyz;
    
	// End of old path = start of new path
	for(i=0; i<6; i++)
		path[0][i] = path[segments][i];

	// determine if direction of motion is positive or negative
	// update global position
	if(inSettings->lastBorder < 6)
    {
		if((path[0][3] + path[0][4] + path[0][5]) > 0.0f)
        {
			positive = 1;
			globalxyz[inSettings->lastBorder / 2] ++;
		}
		else
        {
			positive = 0;
			globalxyz[inSettings->lastBorder / 2] --;
		}
	}
	else
    {
		if(path[0][3] > 0.0f)
        {
			positive = 1;
			globalxyz[0] ++;
		}
		else
        {
			positive = 0;
			globalxyz[0] --;
		}
        
		if(path[0][4] > 0.0f)
			globalxyz[1] ++;
		else
			globalxyz[1] --;
		if(path[0][5] > 0.0f)
			globalxyz[2] ++;
		else
			globalxyz[2] --;
	}

	if(!myRandi(11 - inSettings->dPathrand))
    {  // Change directions
		if(!positive)
			inSettings->lastBorder += 10;
		newBorder = transitions[inSettings->lastBorder*6+myRandi(6)];
		positive = 0;
		if(newBorder < 10)
			positive = 1;
		else
			newBorder -= 10;
		for(i=0; i<6; i++)  // set the new border point
			path[1][i] = inSettings->bPnt[newBorder][i];
		if(!positive){  // flip everything if direction is negative
			if(newBorder < 6)
				path[1][newBorder/2] *= -1.0f;
			else
				for(i=0; i<3; i++)
					path[1][i] *= -1.0f;
			for(i=3; i<6; i++)	
				path[1][i] *= -1.0f;
		}
		for(i=0; i<3; i++)  // reposition the new border
			path[1][i] += globalxyz[i];
		inSettings->lastBorder = newBorder;
		segments = 1;
	}
	else
    {  // Just keep going straight
		newBorder = inSettings->lastBorder;
		for(i=0; i<6; i++)
			inSettings->path[1][i] = inSettings->bPnt[newBorder][i];
		i = newBorder / 2;
		if(!positive)
        {
			if(newBorder < 6)
				inSettings->path[1][i] *= -1.0f;
			else{
				inSettings->path[1][0] *= -1.0f;
				inSettings->path[1][1] *= -1.0f;
				inSettings->path[1][2] *= -1.0f;
			}
			inSettings->path[1][3] *= -1.0f;
			inSettings->path[1][4] *= -1.0f;
			inSettings->path[1][5] *= -1.0f;
		}
		for(j=0; j<3; j++)
        {
			inSettings->path[1][j] += inSettings->globalxyz[j];
			if((newBorder < 6) && (j != 1))
				path[1][j] += myRandf(0.15f) - 0.075f;
		}
		if(newBorder >= 6)
			path[1][0] += myRandf(0.1f) - 0.05f;
		segments = 1;
	}
}


__private_extern__ void draw(LatticeSaverSettings * inSettings)
{
	int i, j, k;
	// Time from one frame to the next
	//static float elapsedTime = 0.0f;
	//static DWORD thisTime = timeGetTime(), lastTime;
	int indexx, indexy, indexz;
	rsVec xyz, dir, angvel, tempVec;
	float angle, distance;
	float rotationInertia = 0.007f * float(inSettings->dSpeed);
//static float rotationInertia = 0.00014f * float(dSpeed);
	 float maxSpin = 0.0025f * float(inSettings->dSpeed);
//static float maxSpin = 0.00005f * float(dSpeed);
	float rotMat[16];
	rsQuat newQuat;
	//float normScale;
    float elapsedTime=inSettings->elapsedTime;
	 
	//rsQuat cullQuat;
	float cullMat[16];
	float transMat[16];
    float ** path=inSettings->path;
	//rsVec cull[5];  // storage for transformed culling vectors
	//static int drawDepth = (1.0f>sinf(float(dFov)*0.5f*D2R)/cosf(float(dFov)*0.5f*D2R))
	//	?dDepth
	//	:int(float(dDepth)*(sinf(float(dFov)*0.5f*D2R)/cosf(float(dFov)*0.5f*D2R))+0.5f);
	 int drawDepth = inSettings->dDepth + 2;

	inSettings->where += float(inSettings->dSpeed) * 0.05f * elapsedTime;
//where += float(dSpeed) * 0.001f * elapsedTime;
	if(inSettings->where >= 1.0f)
    {
		inSettings->where -= 1.0f;
		inSettings->seg++;
	}
    
	if(inSettings->seg >= segments)
    {
		inSettings->seg = 0;
		reconfigure(inSettings);
	}
    
    int seg=inSettings->seg;
    
	// Calculate position
	xyz[0] = interpolate(path[seg][0], path[seg][3], path[seg+1][0],
		path[seg+1][3], inSettings->where);
	xyz[1] = interpolate(path[seg][1], path[inSettings->seg][4], path[seg+1][1],
		path[seg+1][4], inSettings->where);
	xyz[2] = interpolate(path[seg][2], path[seg][5], path[seg+1][2],
		path[seg+1][5], inSettings->where);

	// Do rotation stuff
	dir = xyz - inSettings->oldxyz;  // Direction of motion
	dir.normalize();
	angvel.cross(dir, inSettings->oldDir);  // Desired axis of rotation
	angle = acos(inSettings->oldDir.dot(dir));  // Desired turn angle
	if(angle > maxSpin)  // Cap the spin
		angle = maxSpin;
	if(angle < -maxSpin)
		angle = -maxSpin;
	angvel.scale(angle);  // Desired angular velocity
	tempVec = angvel - inSettings->oldAngvel;  // Change in angular velocity
	distance = tempVec.length();  // Don't let angular velocity change too much
	if(distance > rotationInertia * elapsedTime)
    {
		tempVec.scale((rotationInertia * elapsedTime) / distance);
		angvel = inSettings->oldAngvel + tempVec;
	}
	/*if(!flymodeChange)  // transition from one fly mode to the other?
		if(!myRandi(1000 - 7 * dSpeed))
			flymodeChange = 1;
	if(flymodeChange){
		normScale = float(((200 - dSpeed) / 2) - flymodeChange) / float((200 - dSpeed) / 2);
		angvel.scale(normScale);
		if(flymodeChange == ((200 - dSpeed) / 2))
			flymode = myRandi(3);
		flymodeChange++;
		if(flymodeChange >= (200 - dSpeed))
			flymodeChange = 0;
	}*/
    
	inSettings->flymodeChange -= elapsedTime;
    
	if(inSettings->flymodeChange <= 1.0f)  // prepare to transition
		angvel.scale(inSettings->flymodeChange);
        
	if(inSettings->flymodeChange <= 0.0f)
    {  // transition from one fly mode to the other?
		inSettings->flymode = myRandi(4);
		inSettings->flymodeChange = myRandf(float(150 - inSettings->dSpeed)) + 5.0f;
	}
    
	tempVec = angvel;  // Recompute desired rotation
	angle = tempVec.normalize();
	newQuat.make(angle, tempVec[0], tempVec[1], tempVec[2]);  // Use rotation	
	if(inSettings->flymode)  // fly normal (straight)
		inSettings->quat.preMult(newQuat);
	else  // don't fly normal (go backwards and stuff)
		inSettings->quat.postMult(newQuat);

	// Roll
	
	inSettings->rollChange -= elapsedTime;
    
	if(inSettings->rollChange <= 0.0f)
    {
		inSettings->rollAcc = myRandf(0.02f * float(inSettings->dSpeed)) - (0.01f * float(inSettings->dSpeed));
//rollAcc = myRandf(0.0004f * float(dSpeed)) - (0.0002f * float(dSpeed));
		inSettings->rollChange = myRandf(10.0f) + 2.0f;
	}
    
	inSettings->rollVel += inSettings->rollAcc * elapsedTime;
    
	if(inSettings->rollVel > (0.04f * float(inSettings->dSpeed)) && inSettings->rollAcc > 0.0f)
//if(rollVel > (0.0008f * float(dSpeed)) && rollAcc > 0.0f)
		inSettings->rollAcc = 0.0f;
	if(inSettings->rollVel < (-0.04f * float(inSettings->dSpeed)) && inSettings->rollAcc < 0.0f)
//if(rollVel < (-0.0008f * float(dSpeed)) && rollAcc < 0.0f)
		inSettings->rollAcc = 0.0f;
    
    newQuat.make(inSettings->rollVel * elapsedTime, inSettings->oldDir[0], inSettings->oldDir[1], inSettings->oldDir[2]);
	inSettings->quat.preMult(newQuat);

	//quat.normalize();
	inSettings->quat.toMat(rotMat);

	// Save old stuff
	inSettings->oldxyz = xyz;
	inSettings->oldDir[0] = -rotMat[2];
	inSettings->oldDir[1] = -rotMat[6];
	inSettings->oldDir[2] = -rotMat[10];
	inSettings->oldAngvel = angvel;

	// Apply transformations
	glLoadMatrixf(rotMat);
	//glLoadIdentity();
	glGetFloatv(GL_MODELVIEW, cullMat);
	glTranslatef(-xyz[0], -xyz[1], -xyz[2]);

	// Transform culling planes
	/*cullQuat.copy(quat);
	cullQuat[3] = -cullQuat[3];
	cullQuat.toMat(cullMat);
	for(i=0; i<5; i++){
		for(j=0; j<3; j++){
			cull[i][j] = cullMat[j] * cullVec[i][0]
				+ cullMat[4+j] * cullVec[i][1]
				+ cullMat[8+j] * cullVec[i][2]
				+ cullMat[12+j];
		}
	}*/
	glGetFloatv(GL_MODELVIEW, transMat);

	// Render everything
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	for(i=inSettings->globalxyz[0]-drawDepth; i<=inSettings->globalxyz[0]+drawDepth; i++)
    {
		for(j=inSettings->globalxyz[1]-drawDepth; j<=inSettings->globalxyz[1]+drawDepth; j++)
        {
			for(k=inSettings->globalxyz[2]-drawDepth; k<=inSettings->globalxyz[2]+drawDepth; k++)
            {
				// check 4 culling planes
				/*tempVec[0] = float(i) - xyz[0];
				tempVec[1] = float(j) - xyz[1];
				tempVec[2] = float(k) - xyz[2];
				if(tempVec.dot(cull[0]) > -0.9f){
					if(tempVec.dot(cull[1]) > -0.9f){
						if(tempVec.dot(cull[2]) > -0.9f){
							if(tempVec.dot(cull[3]) > -0.9f){
								if(tempVec.dot(cull[4]) > -(float(dDepth) + 0.9f)){
									// if object isn't culled, calculate indices
									indexx = myMod(i);  
									indexy = myMod(j);
									indexz = myMod(k);
									// draw it
									glPushMatrix();
										glTranslatef(float(i), float(j), float(k));
										glCallList(lattice[indexx][indexy][indexz]);
									glPopMatrix();
								}
							}
						}
					}
				}*/
                
				tempVec[0] = i - xyz[0];
				tempVec[1] = j - xyz[1];
				tempVec[2] = k - xyz[2];
                
				float tpos[3];  // transformed position
				//tpos[0] = tempVec[0]*transMat[0] + tempVec[1]*transMat[4] + tempVec[2]*transMat[8] + transMat[12];
				//tpos[1] = tempVec[0]*transMat[1] + tempVec[1]*transMat[5] + tempVec[2]*transMat[9] + transMat[13];
				//tpos[2] = tempVec[0]*transMat[2] + tempVec[1]*transMat[6] + tempVec[2]*transMat[10] + transMat[14];
				tpos[0] = tempVec[0]*rotMat[0] + tempVec[1]*rotMat[4] + tempVec[2]*rotMat[8];// + rotMat[12];
				tpos[1] = tempVec[0]*rotMat[1] + tempVec[1]*rotMat[5] + tempVec[2]*rotMat[9];// + rotMat[13];
				tpos[2] = tempVec[0]*rotMat[2] + tempVec[1]*rotMat[6] + tempVec[2]*rotMat[10];// + rotMat[14];
				
                if(inSettings->theCamera->inViewVolume(tpos, 0.9f))
                {
					indexx = myMod(i);
					indexy = myMod(j);
					indexz = myMod(k);
					// draw it
					glPushMatrix();
						glTranslatef((float)i, (float)j, (float)k);
						glCallList(inSettings->lattice[indexx][indexy][indexz]);
					glPopMatrix();
				}
			}
		}
	}
}

__private_extern__ void initSaver(int width,int height,LatticeSaverSettings * inSettings)
{
	
	int i, j, k;
	float ** bPnt;
    float ** path;
    
    path=(float **) malloc(7*sizeof(float **));
    
    for(i=0;i<7;i++)
    {
        path[i]=(float *) malloc(6*sizeof(float));
    }
    
    inSettings->path=path;
    
    bPnt=(float **) malloc(10*sizeof(float **));
    
    for(i=0;i<10;i++)
    {
        bPnt[i]=(float *) malloc(6*sizeof(float));
    }
    
    inSettings->bPnt=bPnt;
    
	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();

	// Window initialization
	
    inSettings->flymode = 1;
    
    inSettings->rollVel = 0.0f;
    inSettings->rollAcc = 0.0f;
    
    inSettings->rollChange = myRandf(10.0f) + 2.0f;
    
    inSettings->oldxyz=rsVec(0.0f, 0.0f, 0.0f);
	inSettings->oldDir=rsVec(0.0f, 0.0f, -1.0f);
	inSettings->oldAngvel=rsVec(0.0f, 0.0f, 0.0f);
    
    inSettings->quat=rsQuat();
    
    inSettings->where=0.0;
    inSettings->seg=0;
    
    inSettings->flymodeChange = 20.0f;
    
    if(inSettings->dWidescreen)
		glViewport(0, height/2-width/4, width, width/2);
	else
		glViewport(0,0, width,height);

	if(inSettings->dUsedTexture != LATTICE_TEXTURE_CRYSTAL && inSettings->dUsedTexture != LATTICE_TEXTURE_GHOSTLY)  // No z-buffering for crystal or ghostly
	{
    	glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float mat[16] = {cos(float(inSettings->dFov) * 0.5f * D2R) / sin(float(inSettings->dFov) * 0.5f * D2R), 0.0f, 0.0f, 0.0f,
		0.0f, 0.0, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f - 0.02f / float(inSettings->dDepth), -1.0f,
		0.0f, 0.0f, -(0.02f + 0.0002f / float(inSettings->dDepth)), 0.0f};
	if(inSettings->dWidescreen)
		//gluPerspective(asinf(sinf(float(dFov)*0.5f*D2R)*0.5f)*2.0f*R2D, 2.0f, 0.01f, float(dDepth));
		//gluPerspective(float(dFov), 2.0f, 0.01f, float(dDepth));
		mat[5] = mat[0] * 2.0f;
	else
		//gluPerspective(asinf(sinf(float(dFov)*0.5f*D2R)*0.75f)*2.0f*R2D, 1.333f, 0.01f, float(dDepth));
		//gluPerspective(float(dFov), 1.333f, 0.01f, float(dDepth));
		//mat[5] = mat[0] * 1.333f;
		mat[5] = mat[0] * (float(width) / float(height));
	glLoadMatrixf(mat);
	inSettings->theCamera = new camera;
	inSettings->theCamera->init(mat, float(inSettings->dDepth));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if(inSettings->dUsedTexture != LATTICE_TEXTURE_CHROME && inSettings->dUsedTexture != LATTICE_TEXTURE_BRASS && inSettings->dUsedTexture != LATTICE_TEXTURE_GHOSTLY)
    {  // No lighting for chrome, brass, or ghostly
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
		float ambient[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		float diffuse[4] = {1.0f, 1.0f, 1.0f, 0.0f};
		float specular[4] = {1.0f, 1.0f, 1.0f, 0.0f};
		float position[4] = {400.0f, 300.0f, 500.0f, 0.0f};
		//float position[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
	}
    else
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }
    
	glEnable(GL_COLOR_MATERIAL);
    
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
	if(inSettings->dUsedTexture == LATTICE_TEXTURE_NONE  ||
       inSettings->dUsedTexture == LATTICE_TEXTURE_SHINY ||
       inSettings->dUsedTexture >= LATTICE_TEXTURE_CIRCUITS)
    {
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
		glColorMaterial(GL_FRONT, GL_SPECULAR);
	}
    
	if(inSettings->dUsedTexture == LATTICE_TEXTURE_CRYSTAL)
    {
		glMaterialf(GL_FRONT, GL_SHININESS, 10.0f);
		glColorMaterial(GL_FRONT, GL_SPECULAR);
	}
    
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	if(inSettings->dFog)
    {
		glEnable(GL_FOG);
		float fog_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
		glFogfv(GL_FOG_COLOR, fog_color);
		glFogf(GL_FOG_MODE, GL_LINEAR);
		glFogf(GL_FOG_START, float(inSettings->dDepth) * 0.3f);
		glFogf(GL_FOG_END, float(inSettings->dDepth) - 0.1f);
	}
    else
    {
        glDisable(GL_FOG);
    }

	glDisable(GL_BLEND);
    
    if(inSettings->dUsedTexture == LATTICE_TEXTURE_CRYSTAL || inSettings->dUsedTexture == LATTICE_TEXTURE_GHOSTLY)
    {  // Use blending for crystal and ghostly
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_BLEND);
	}

	if(inSettings->dUsedTexture == LATTICE_TEXTURE_CIRCUITS)
    {  // Use blending for circuits
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
	}

	glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
        
    if(inSettings->dUsedTexture)
    {
		glEnable(GL_TEXTURE_2D);
		initTextures(inSettings);
		// Environment mapping for crystal, chrome, brass, shiny, and ghostly
		if(inSettings->dUsedTexture >= LATTICE_TEXTURE_CRYSTAL && inSettings->dUsedTexture <= LATTICE_TEXTURE_GHOSTLY)
        {
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
		}
	}

	// Initialize lattice objects and their positions in the lattice array
	makeLatticeObjects(inSettings);
	for(i=0; i<LATSIZE; i++)
    {
		for(j=0; j<LATSIZE; j++)
        {
			for(k=0; k<LATSIZE; k++)
            {
				inSettings->lattice[i][j][k] = myRandi(NUMOBJECTS) + 1;
			}
		}
	}

	// Initialize border points
	// horizontal border points
	for(i=0; i<6; i++)
    {
		for(j=0; j<6; j++)
        {
			bPnt[i][j] = 0.0f;
		}
	}
    
	bPnt[0][0] = 0.5f;    bPnt[0][1] = -0.25f;  bPnt[0][2] = 0.25f;  bPnt[0][3] = 1.0f; // right
	bPnt[1][0] = 0.5f;    bPnt[1][1] = 0.25f;   bPnt[1][2] = -0.25f; bPnt[1][3] = 1.0f; // right
	bPnt[2][0] = -0.25f;  bPnt[2][1] = 0.5f;    bPnt[2][2] = 0.25f;  bPnt[2][4] = 1.0f; // top
	bPnt[3][0] = 0.25f;   bPnt[3][1] = 0.5f;    bPnt[3][2] = -0.25f; bPnt[3][4] = 1.0f; // top
	bPnt[4][0] = -0.25f;  bPnt[4][1] = -0.25f;  bPnt[4][2] = 0.5f;   bPnt[4][5] = 1.0f; // front
	bPnt[5][0] = 0.25f;   bPnt[5][1] = 0.25f;   bPnt[5][2] = 0.5f;   bPnt[5][5] = 1.0f; // front
	// diagonal border points
	bPnt[6][0] = 0.5f;    bPnt[6][1] = -0.5f;   bPnt[6][2] = -0.5f;  bPnt[6][3] = 1.0f; bPnt[6][4] = -1.0f; bPnt[6][5] = -1.0f;
	bPnt[7][0] = 0.5f;    bPnt[7][1] = 0.5f;    bPnt[7][2] = -0.5f;  bPnt[7][3] = 1.0f; bPnt[7][4] = 1.0f;  bPnt[7][5] = -1.0f;
	bPnt[8][0] = 0.5f;    bPnt[8][1] = -0.5f;   bPnt[8][2] = 0.5f;   bPnt[8][3] = 1.0f; bPnt[8][4] = -1.0f; bPnt[8][5] = 1.0f;
	bPnt[9][0] = 0.5f;    bPnt[9][1] = 0.5f;    bPnt[9][2] = 0.5f;   bPnt[9][3] = 1.0f; bPnt[9][4] = 1.0f;  bPnt[9][5] = 1.0f;

	inSettings->globalxyz[0] = 0;
	inSettings->globalxyz[1] = 0;
	inSettings->globalxyz[2] = 0;

	// Set up first path section
	path[0][0] = 0.0f;
	path[0][1] = 0.0f;
	path[0][2] = 0.0f;
	path[0][3] = 0.0f;
	path[0][4] = 0.0f;
	path[0][5] = 0.0f;
	j = myRandi(12);
	k = j % 6;
	for(i=0; i<6; i++)
		path[1][i] = bPnt[k][i];
	if(j > 5)
    {  // If we want to head in a negative direction
		i = k / 2;  // then we need to flip along the appropriate axis
		path[1][i] *= -1.0f;
		path[1][i+3] *= -1.0f;
	}
	inSettings->lastBorder = k;
	segments = 1;

	// Set vectors perpendicular to culling planes
	// left
/*	cullVec[0][0] = cosf(dFov * 0.5f * D2R);
	cullVec[0][1] = 0.0f;
	cullVec[0][2] = -sinf(dFov * 0.5f * D2R);
	// right
	cullVec[1][0] = -cullVec[0][0];
	cullVec[1][1] = 0.0f;
	cullVec[1][2] = cullVec[0][2];
	// bottom
	cullVec[2][0] = 0.0f;
	//cullVec[2][1] = cosf(dFov * 0.5f * 0.75f * D2R);
	//cullVec[2][2] = -sinf(dFov * 0.5f * 0.75f * D2R);
	cullVec[2][1] = cosf(dFov * 0.5f * (float(top) / float(right)) * D2R);
	cullVec[2][2] = -sinf(dFov * 0.5f * (float(top) / float(right)) * D2R);
	// top
	cullVec[3][0] = 0.0f;
	cullVec[3][1] = -cullVec[2][1];
	cullVec[3][2] = cullVec[2][2];
	// far
	cullVec[4][0] = 0.0f;
	cullVec[4][1] = 0.0f;
	cullVec[4][2] = 1.0f;*/
}



__private_extern__ void cleanSettings(LatticeSaverSettings * inSettings)
{
	int i;
    
    for(i=0;i<7;i++)
    {
        free(inSettings->path[i]);
    }
    
    free(inSettings->path);
    
    for(i=0;i<10;i++)
    {
        free(inSettings->bPnt[i]);
    }
    
    free(inSettings->bPnt);
}


__private_extern__ void setDefaults(int which,LatticeSaverSettings * inSettings)
{
	switch(which)
    {
        case DEFAULTS1:  // Regular
            inSettings->dLongitude = 16;
            inSettings->dLatitude = 8;
            inSettings->dThick = 50;
            inSettings->dDensity = 50;
            inSettings->dDepth = 3;
            inSettings->dFov = 90;
            inSettings->dPathrand = 7;
            inSettings->dSpeed = 10;
            inSettings->dTexture = LATTICE_TEXTURE_NONE;
            inSettings->dSmooth = FALSE;
            inSettings->dFog = TRUE;
            break;
        case DEFAULTS2:  // Chainmail
            inSettings->dLongitude = 24;
            inSettings->dLatitude = 12;
            inSettings->dThick = 50;
            inSettings->dDensity = 80;
            inSettings->dDepth = 2;
            inSettings->dFov = 90;
            inSettings->dPathrand = 7;
            inSettings->dSpeed = 10;
            inSettings->dTexture = LATTICE_TEXTURE_CHROME;
            inSettings->dSmooth = TRUE;
            inSettings->dFog = TRUE;
            break;
        case DEFAULTS3:  // Brass Mesh
            inSettings->dLongitude = 4;
            inSettings->dLatitude = 4;
            inSettings->dThick = 40;
            inSettings->dDensity = 50;
            inSettings->dDepth = 3;
            inSettings->dFov = 90;
            inSettings->dPathrand = 7;
            inSettings->dSpeed = 10;
            inSettings->dTexture = LATTICE_TEXTURE_BRASS;
            inSettings->dSmooth = FALSE;
            inSettings->dFog = TRUE;
            break;
        case DEFAULTS4:  // Computer
            inSettings->dLongitude = 4;
            inSettings->dLatitude = 6;
            inSettings->dThick = 70;
            inSettings->dDensity = 90;
            inSettings->dDepth = 3;
            inSettings->dFov = 90;
            inSettings->dPathrand = 7;
            inSettings->dSpeed = 10;
            inSettings->dTexture = LATTICE_TEXTURE_CIRCUITS;
            inSettings->dSmooth = FALSE;
            inSettings->dFog = TRUE;
            break;
        case DEFAULTS5:  // Slick
            inSettings->dLongitude = 24;
            inSettings->dLatitude = 12;
            inSettings->dThick = 100;
            inSettings->dDensity = 30;
            inSettings->dDepth = 3;
            inSettings->dFov = 90;
            inSettings->dPathrand = 7;
            inSettings->dSpeed = 10;
            inSettings->dTexture = LATTICE_TEXTURE_SHINY;
            inSettings->dSmooth = TRUE;
            inSettings->dFog = TRUE;
            break;
        case DEFAULTS6:  // Tasty
            inSettings->dLongitude = 24;
            inSettings->dLatitude = 12;
            inSettings->dThick = 100;
            inSettings->dDensity = 25;
            inSettings->dDepth = 3;
            inSettings->dFov = 90;
            inSettings->dPathrand = 7;
            inSettings->dSpeed = 10;
            inSettings->dTexture = LATTICE_TEXTURE_DOUGHNUTS;
            inSettings->dSmooth = TRUE;
            inSettings->dFog = TRUE;
            break;
	}
}