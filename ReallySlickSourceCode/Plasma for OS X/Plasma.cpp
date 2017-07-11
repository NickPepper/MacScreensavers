/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Plasma is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Plasma is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


// Plasma screen saver


#include "Plasma.h"
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PIx2 6.28318530718f

// useful random functions

static float myRandf(float x)
{
    return(((double) rand() * x) / RAND_MAX);
}


// Find absolute value and truncate to 1.0

static float fabstrunc(float f)
{
	if(f >= 0.0f)
		return((f <= 1.0f) ? f : 1.0f);
	else
		return((f >= -1.0f) ? -f : 1.0f);
}


__private_extern__ void draw(PlasmaSaverSettings * inSettings)
{
	int i, j;
	float rgb[3];
	float temp;
	float focus = ((float) inSettings->dFocus)/ 50.0f + 0.3f;
	float maxdiff = 0.004f * (float) (inSettings->dSpeed);
	static int index;
    float * c=inSettings->c;
    float *** position=inSettings->position;
    register float * tPosition;
    float ***plasma=inSettings->plasma;
    float *plasmamap=inSettings->plasmamap;
    int maxj;
    int plasmasize=inSettings->plasmasize;
    
	//Update constants
	for(i=0; i<NUMCONSTS; i++)
    {
		inSettings->ct[i] += inSettings->cv[i];
		if(inSettings->ct[i] > PIx2)
			inSettings->ct[i] -= PIx2;
		c[i] = ((float) sin((double)inSettings->ct[i])) * focus;
	}

	// Update colors
    
    maxj=(int)(((float)plasmasize) / inSettings->aspectRatio);
    
	for(i=0; i<plasmasize; i++)
    {
		for(j=0; j<maxj; j++)
        {
			// Calculate vertex colors
			rgb[0] = inSettings->plasma[i][j][0];
			rgb[1] = inSettings->plasma[i][j][1];
			rgb[2] = inSettings->plasma[i][j][2];
			
            tPosition=position[i][j];
            
            plasma[i][j][0] = 0.7f
							* (c[0] * tPosition[0] + c[1] * tPosition[1]
							+ c[2] * (tPosition[0] * tPosition[0] + 1.0f)
							+ c[3] * tPosition[0] * tPosition[1]
							+ c[4] * rgb[1] + c[5] * rgb[2]);
			plasma[i][j][1] = 0.7f
							* (c[6] * tPosition[0] + c[7] * tPosition[1]
							+ c[8] * tPosition[0] * tPosition[0]
							+ c[9] * (tPosition[1] * tPosition[1] - 1.0f)
							+ c[10] * rgb[0] + c[11] * rgb[2]);
			plasma[i][j][2] = 0.7f
							* (c[12] * tPosition[0] + c[13] * tPosition[1]
							+ c[14] * (1.0f - tPosition[0] * tPosition[1])
							+ c[15] * tPosition[1] * tPosition[1]
							+ c[16] * rgb[0] + c[17] * rgb[1]);

			// Don't let the colors change too much
            
			temp = plasma[i][j][0] - rgb[0];
            
			if(temp > maxdiff)
				plasma[i][j][0] = rgb[0] + maxdiff;
            else
            {
                if(temp < -maxdiff)
                    plasma[i][j][0] = rgb[0] - maxdiff;
			}
            
            temp = plasma[i][j][1] - rgb[1];
            
			if(temp > maxdiff)
				plasma[i][j][1] = rgb[1] + maxdiff;
			else
            {
                if(temp < -maxdiff)
            
                    plasma[i][j][1] = rgb[1] - maxdiff;
			}
            
            temp = plasma[i][j][2] - rgb[2];
            
			if(temp > maxdiff)
				plasma[i][j][2] = rgb[2] + maxdiff;
			else
            {
                if(temp < -maxdiff)
                    plasma[i][j][2] = rgb[2] - maxdiff;
            }
            
			// Put colors into texture
			
            index = (i * inSettings->texsize + j) * 3;
			plasmamap[index] = fabstrunc(plasma[i][j][0]);
			plasmamap[index+1] = fabstrunc(plasma[i][j][1]);
			plasmamap[index+2] = fabstrunc(plasma[i][j][2]);
		}
	}

	// Update texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (int)(((float)plasmasize) / inSettings->aspectRatio), plasmasize,
		GL_RGB, GL_FLOAT, plasmamap);

	// Draw it
	// The "- 1" cuts off right and top edges to get rid of blending to black
	
    float texright = (((float)plasmasize) - 1) / ((float)inSettings->texsize);
	float textop = texright / inSettings->aspectRatio;
	
    glBindTexture(GL_TEXTURE_2D, 1);
    
    glBegin(GL_TRIANGLE_STRIP);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);
		glTexCoord2f(0.0f, texright);
		glVertex2f(1.0f, 0.0f);
		glTexCoord2f(textop, 0.0f);
		glVertex2f(0.0f, 1.0f);
		glTexCoord2f(textop, texright);
		glVertex2f(1.0f, 1.0f);
	glEnd();
}


__private_extern__ void initSaver(int width,int height,PlasmaSaverSettings * inSettings)
{
	int i, j;

	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();

	// Window initialization
	
	glViewport(0,0, width,height);
	inSettings->aspectRatio = ((float)width) / ((float)height);
	
    if(inSettings->aspectRatio >= 1.0f)
    {
		inSettings->wide = 30.0f / inSettings->dZoom;
		inSettings->high = inSettings->wide / inSettings->aspectRatio;
	}
	else
    {
		inSettings->high = 30.0f / inSettings->dZoom;
		inSettings->wide = inSettings->high * inSettings->aspectRatio;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f, 1.0f, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set resolution of plasma
    
	if(inSettings->aspectRatio >= 1.0f)
		inSettings->plasmasize = (inSettings->dResolution * MAXTEXSIZE) / 100;
	else
		inSettings->plasmasize = (int)(((float)inSettings->dResolution * MAXTEXSIZE) * inSettings->aspectRatio * 0.01f);
        
	// Set resolution of texture
    
	inSettings->texsize = 16;
	if(inSettings->aspectRatio >= 1.0f)
    {
		while(inSettings->plasmasize > inSettings->texsize)
			inSettings->texsize *= 2;
	}
    else
    {
		while(int(((float) inSettings->plasmasize) / inSettings->aspectRatio) > inSettings->texsize)
			inSettings->texsize *= 2;
    }
    
	// Initialize memory and positions
    
    inSettings->plasmamap = new float[inSettings->texsize*inSettings->texsize*3];
    
	for(i=0; i<inSettings->texsize*inSettings->texsize*3; i++)
    {
		inSettings->plasmamap[i] = 0.0f;
    }
    
	inSettings->plasma = new float**[inSettings->plasmasize];
	inSettings->position = new float**[inSettings->plasmasize];
    
	for(i=0; i<inSettings->plasmasize; i++)
    {
		int maxj=(int)(((float)inSettings->plasmasize) / inSettings->aspectRatio);
        
        inSettings->plasma[i] = new float*[maxj];
		inSettings->position[i] = new float*[maxj];
        
		for(j=0; j<maxj; j++)
        {
			inSettings->plasma[i][j] = new float[3];
			inSettings->position[i][j] = new float[2];
            
			inSettings->plasma[i][j][0] = 0.0f;
			inSettings->plasma[i][j][1] = 0.0f;
			inSettings->plasma[i][j][2] = 0.0f;
            
			inSettings->position[i][j][0] = (((float) i) * inSettings->wide) / (((float)inSettings->plasmasize) - 1) - (inSettings->wide * 0.5f);
			inSettings->position[i][j][1] = (((float) j) * inSettings->high) / ((((float)inSettings->plasmasize)) / inSettings->aspectRatio - 1.0f) - (inSettings->high * 0.5f);
		}
	}
	// Initialize constants
	for(i=0; i<NUMCONSTS; i++)
    {
		inSettings->ct[i] = myRandf(PIx2);
		inSettings->cv[i] = myRandf(0.005f * ((float) inSettings->dSpeed)) + 0.0001f;
	}
    
    // Make texture
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, inSettings->texsize);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, inSettings->texsize, inSettings->texsize, 0,GL_RGB, GL_FLOAT, inSettings->plasmamap);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	
}

__private_extern__ void cleanSettings(PlasmaSaverSettings * inSettings)
{
    int i, j;
    GLuint texnum=1;
    
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1,&texnum);
    
    glDisable(GL_TEXTURE_2D);
    
    if (inSettings->plasmamap!=NULL)
    {
        delete[] inSettings->plasmamap;
        inSettings->plasmamap=NULL;
    }
    
    if (inSettings->plasma!=NULL)
    {
        for(i=0; i<inSettings->plasmasize; i++)
        {
			int maxj=(int) (inSettings->plasmasize / inSettings->aspectRatio);
            
            for(j=0; j<maxj; j++)
            {
                delete[] inSettings->plasma[i][j];
                delete[] inSettings->position[i][j];
            }
            
            delete[] inSettings->plasma[i];
            delete[] inSettings->position[i];
        }
        
        delete[] inSettings->plasma;
        delete[] inSettings->position;
        
        inSettings->plasma=NULL;
        inSettings->position=NULL;
    }
}

__private_extern__ void setDefaults(PlasmaSaverSettings * inSettings)
{
	inSettings->dZoom = 10;
	inSettings->dFocus = 30;
	inSettings->dSpeed = 20;
	inSettings->dResolution = 25;
}


