/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Skyrocket is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Skyrocket is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


// Skyrocket screen saver


#include <windows.h>
#include <stdio.h>
#include "../Savergl/Savergl.h"
#include <math.h>
#include <time.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <regstr.h>
#include <commctrl.h>
#include "resource.h"
#include "../../rsMath/rsMath.h"
#include "particle.h"
//#include "overlay.h"



// Global variables
LPCTSTR registryPath = ("Software\\Really Slick\\Skyrocket");
HDC hdc;
HGLRC hglrc;
int readyToDraw = 0;
// list of particles
std::list<particle> particles;
// Time from one frame to the next
float elapsedTime = 0.0f;
// Window variables
int xsize, ysize, centerx, centery;
float aspectRatio;
// Camera variables
static rsVec lookFrom[3];  // 3 = position, target position, last position
static rsVec lookAt[3]  // 3 = position, target position, last position
	= {rsVec(0.0f, 1200.0f, 0.0f), 
	rsVec(0.0f, 1200.0f, 0.0f), 
	rsVec(0.0f, 1200.0f, 0.0f)};
rsVec cameraPos;  // used for positioning sounds (same as lookFrom[0])
rsVec cameraVel;  // used for doppler shift
// Mouse variables
float mouseIdleTime;
int mouseButtons, mousex, mousey;
float mouseSpeed;

int numRockets = 0;
#define MAXFLARES 110    // Required 100 and 10 extra for good measure
float allflares[MAXFLARES][7];  // type, x, y, r, g, b, alpha
int numFlares = 0;
// Parameters edited in the dialog box
int dMaxrockets;
int dSmoke;
int dExplosionsmoke;
int dWind;
int dAmbient;
int dStardensity;
int dFlare;
int dMoonglow;
int dMoon;
int dClouds;
int dEarth;
int dIllumination;
int dSound;
int dPriority;
// Commands given from keyboard
int kAction = 1;
int kCamera = 1;  // 0 = paused, 1 = autonomous, 2 = mouse control
int kNewCamera = 0;
int kSlowMotion = 0;
int userDefinedExplosion = -1;



std::list<particle>::iterator currentParticle = particles.begin();
#define MINDEPTH -1000000.0f  // particle depth for inactive particles


particle* addParticle(){
	static particle* tempPart;

	if(currentParticle != particles.end()){
		currentParticle->depth = 10000;
		tempPart = currentParticle->thisParticle();
		currentParticle ++;
		return(tempPart);
	}
	else{
		particles.push_back(particle());
		tempPart = &(particles.back());
		tempPart->depth = 10000;
		currentParticle = particles.end();
		return(tempPart);
	}
}


void removeParticle(std::list<particle>::iterator curpart){
	curpart->depth = MINDEPTH;
}


void sortParticles(){
	particles.sort();
	currentParticle = particles.begin();
	while(currentParticle->depth > MINDEPTH && currentParticle != particles.end())
		currentParticle ++;
}


// Rockets and explosions illuminate smoke
// Only explosions illuminate clouds
void illuminate(particle* ill){
	float temp;
	// desaturate illumination colors
	rsVec newrgb(ill->rgb[0] * 0.6f + 0.4f, ill->rgb[1] * 0.6f + 0.4f, ill->rgb[2] * 0.6f + 0.4f);

	// Smoke illumination
	if((ill->type == ROCKET) || (ill->type == FOUNTAIN)){
		float distsquared;
		std::list<particle>::iterator smk = particles.begin();
		while(smk != particles.end()){
			if(smk->depth > MINDEPTH && smk->type == SMOKE){
				distsquared = (ill->xyz[0] - smk->xyz[0]) * (ill->xyz[0] - smk->xyz[0])
					+ (ill->xyz[1] - smk->xyz[1]) * (ill->xyz[1] - smk->xyz[1])
					+ (ill->xyz[2] - smk->xyz[2]) * (ill->xyz[2] - smk->xyz[2]);
				if(distsquared < 40000.0f){
					temp = (40000.0f - distsquared) * 0.000025f;
					temp = temp * temp * ill->bright;
					smk->rgb[0] += temp * newrgb[0];
					if(smk->rgb[0] > 1.0f)
						smk->rgb[0] = 1.0f;
					smk->rgb[1] += temp * newrgb[1];
					if(smk->rgb[1] > 1.0f)
						smk->rgb[1] = 1.0f;
					smk->rgb[2] += temp * newrgb[2];
					if(smk->rgb[2] > 1.0f)
						smk->rgb[2] = 1.0f;
				}
			}
			smk++;
		}
	}
	if(ill->type == EXPLOSION){
		float distsquared;
		std::list<particle>::iterator smk = particles.begin();
		while(smk != particles.end()){
			if(smk->depth > MINDEPTH && smk->type == SMOKE){
				distsquared = (ill->xyz[0] - smk->xyz[0]) * (ill->xyz[0] - smk->xyz[0])
					+ (ill->xyz[1] - smk->xyz[1]) * (ill->xyz[1] - smk->xyz[1])
					+ (ill->xyz[2] - smk->xyz[2]) * (ill->xyz[2] - smk->xyz[2]);
				if(distsquared < 640000.0f){
					temp = (640000.0f - distsquared) * 0.0000015625f;
					temp = temp * temp * ill->bright;
					smk->rgb[0] += temp * newrgb[0];
					if(smk->rgb[0] > 1.0f)
						smk->rgb[0] = 1.0f;
					smk->rgb[1] += temp * newrgb[1];
					if(smk->rgb[1] > 1.0f)
						smk->rgb[1] = 1.0f;
					smk->rgb[2] += temp * newrgb[2];
					if(smk->rgb[2] > 1.0f)
						smk->rgb[2] = 1.0f;
				}
			}
			smk++;
		}
	}

	// cloud illumination
	if(ill->type == EXPLOSION && dClouds){
		int north, south, west, east;  // limits of cloud indices to inspect
		int halfmesh = CLOUDMESH / 2;
		float distsquared;
		// remember clouds have 20000-foot radius from world.h, hence 0.00005
		// Hardcoded values like this are evil, but oh well
		south = int((ill->xyz[2] - 1600.0f) * 0.00005f * float(halfmesh)) + halfmesh;
		north = int((ill->xyz[2] + 1600.0f) * 0.00005f * float(halfmesh) + 0.5f) + halfmesh;
		west = int((ill->xyz[0] - 1600.0f) * 0.00005f * float(halfmesh)) + halfmesh;
		east = int((ill->xyz[0] + 1600.0f) * 0.00005f * float(halfmesh) + 0.5f) + halfmesh;
		for(int i=west; i<=east; i++){
			for(int j=south; j<=north; j++){
				distsquared = (clouds[i][j][0] - ill->xyz[0]) * (clouds[i][j][0] - ill->xyz[0])
					+ (clouds[i][j][1] - ill->xyz[1]) * (clouds[i][j][1] - ill->xyz[1])
					+ (clouds[i][j][2] - ill->xyz[2]) * (clouds[i][j][2] - ill->xyz[2]);
				if(distsquared < 2560000.0f){
					temp = (2560000.0f - distsquared) * 0.000000390625f;
					temp = temp * temp * ill->bright;
					clouds[i][j][6] += temp * newrgb[0];
					if(clouds[i][j][6] > 1.0f)
						clouds[i][j][6] = 1.0f;
					clouds[i][j][7] += temp * newrgb[1];
					if(clouds[i][j][7] > 1.0f)
						clouds[i][j][7] = 1.0f;
					clouds[i][j][8] += temp * newrgb[2];
					if(clouds[i][j][8] > 1.0f)
						clouds[i][j][8] = 1.0f;
				}
			}
		}
	}
}


// pulling of other particles
void pulling(particle* suck){
	rsVec diff;
	float pulldistsquared;
	float pullconst = (1.0f - suck->life) * 0.01f * elapsedTime;

	std::list<particle>::iterator puller = particles.begin();
	while(puller != particles.end()){
		if(puller->depth > MINDEPTH){
			diff = suck->xyz - puller->xyz;
			pulldistsquared = diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2];
			if(pulldistsquared < 250000.0f && pulldistsquared != 0.0f && puller->type != SUCKER){
				diff.normalize();
				puller->vel += diff * ((250000.0f - pulldistsquared) * pullconst);
			}
		}
		puller ++;
	}
}


// pushing of other particles
void pushing(particle* shock){
	rsVec diff;
	float pushdistsquared;
	float pushconst = (1.0f - shock->life) * 0.002f * elapsedTime;

	std::list<particle>::iterator pusher = particles.begin();
	while(pusher != particles.end()){
		if(pusher->depth > MINDEPTH){
			diff = pusher->xyz - shock->xyz;
			pushdistsquared = diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2];
			if(pushdistsquared < 640000.0f && pushdistsquared != 0.0f && pusher->type != SHOCKWAVE){
				diff.normalize();
				pusher->vel += diff * ((640000.0f - pushdistsquared) * pushconst);
			}
		}
		pusher ++;
	}
}


// vertical stretching of other particles (x, z sucking; y pushing)
void stretching(particle* stretch){
	rsVec diff;
	float stretchdistsquared, temp;
	float stretchconst = (1.0f - stretch->life) * 0.002f * elapsedTime;

	std::list<particle>::iterator stretcher = particles.begin();
	while(stretcher != particles.end()){
		if(stretcher->depth > MINDEPTH){
			diff = stretch->xyz - stretcher->xyz;
			stretchdistsquared = diff[0]*diff[0] + diff[1]*diff[1] + diff[2]*diff[2];
			if(stretchdistsquared < 640000.0f && stretchdistsquared != 0.0f && stretcher->type != STRETCHER){
				diff.normalize();
				temp = (640000.0f - stretchdistsquared) * stretchconst;
				stretcher->vel[0] += diff[0] * temp * 5.0f;
				stretcher->vel[1] -= diff[1] * temp;
				stretcher->vel[2] += diff[2] * temp * 5.0f;
			}
		}
		stretcher ++;
	}
}


// Makes list of lens flares.  Must be a called even when action is paused
// because camera might still be moving.
void makeFlareList(){
	rsVec cameraDir, partDir;

	cameraDir = lookAt[0] - lookFrom[0];
	cameraDir.normalize();
	std::list<particle>::iterator curlight = particles.begin();
	while(curlight->depth > MINDEPTH && curlight != particles.end() && numFlares < MAXFLARES){
		if(curlight->type == EXPLOSION){
			double winx, winy, winz;
			gluProject(curlight->xyz[0], curlight->xyz[1], curlight->xyz[2],
				modelMat, projMat, viewport,
				&winx, &winy, &winz);
			partDir = curlight->xyz - cameraPos;
			if(partDir.dot(cameraDir) > 1.0f){  // is light source in front of camera?
				allflares[numFlares][0] = 0;
				allflares[numFlares][1] = (float(winx) / float(xsize)) * aspectRatio;
				allflares[numFlares][2] = float(winy) / float(ysize);
				allflares[numFlares][3] = curlight->rgb[0];
				allflares[numFlares][4] = curlight->rgb[1];
				allflares[numFlares][5] = curlight->rgb[2];
				rsVec vector = curlight->xyz - cameraPos;  // find distance attenuation factor
				float distatten = (10000.0f - vector.length()) * 0.0001f;
				if(distatten < 0.0f)
					distatten = 0.0f;
				allflares[numFlares][6] = curlight->bright * float(dFlare) * 0.01f * distatten;
				numFlares++;
			}
		}
		if(curlight->type == SUCKER || curlight->type == SHOCKWAVE
			|| curlight->type == STRETCHER || curlight->type == BIGMAMA){
			double winx, winy, winz;
			float temp;
			gluProject(curlight->xyz[0], curlight->xyz[1], curlight->xyz[2],
				modelMat, projMat, viewport,
				&winx, &winy, &winz);
			partDir = curlight->xyz - cameraPos;
			if(partDir.dot(cameraDir) > 1.0f){  // is light source in front of camera?
				allflares[numFlares][0] = 1;  // superFlare
				allflares[numFlares][1] = (float(winx) / float(xsize)) * aspectRatio;
				allflares[numFlares][2] = float(winy) / float(ysize);
				allflares[numFlares][3] = curlight->rgb[0];
				allflares[numFlares][4] = curlight->rgb[1];
				allflares[numFlares][5] = curlight->rgb[2];
				rsVec vector = curlight->xyz - cameraPos;  // find distance attenuation factor
				float distatten = (20000.0f - vector.length()) * 0.00005f;
				if(distatten < 0.0f)
					distatten = 0.0f;
				temp = 1.0f - (curlight->bright - 0.5f) * float(dFlare) * 0.02f;
				allflares[numFlares][6] = (1.0f - temp * temp * temp * temp) * distatten;
				numFlares++;
			}
		}
		curlight++;
	}
}


void draw(){
	int i, j;
	static float cameraAngle = 0.0f;
	static const float firstHeading = myRandf(2.0f * PIx2);
	static const float firstRadius = myRandf(2000.0f);
	static int lastCameraMode = kCamera;
	static float cameraTime[3]  // time, elapsed time, step (1.0 - 0.0)
		= {20.0f, 0.0f, 0.0f};

	// super fast easter egg
	static int superFast = myRandi(2000);
	if(!superFast)
		elapsedTime *= 5.0f;

	// build viewing matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, aspectRatio, 1.0f, 40000.0f);
	glGetDoublev(GL_PROJECTION_MATRIX, projMat);

	// update camera
	static int first = 1;
	if(first){
		lookFrom[1] = rsVec(myRandf(3000.0f) - 1500.0f, 400.0f, myRandf(3000.0f) - 1500.0f);
		lookFrom[2] = rsVec(myRandf(1000.0f) + 5000.0f, 5.0f, myRandf(4000.0f) - 2000.0f);
		first = 0;
	}
	if(kNewCamera){  // Make new random camera view
		cameraTime[0] = myRandf(25.0f) + 5.0f;
		cameraTime[1] = 0.0f;
		cameraTime[2] = 0.0f;
		// choose new positions
		lookFrom[1][0] = myRandf(6000.0f) - 3000.0f;  // new target position
		lookFrom[1][1] = myRandf(1700.0f) + 5.0f;
		lookFrom[1][2] = myRandf(6000.0f) - 3000.0f;
		lookAt[1][0] = myRandf(1000.0f) - 500.0f;  // new target position
		lookAt[1][1] = myRandf(1100.0f) + 200.0f;
		lookAt[1][2] = myRandf(1000.0f) - 500.0f;
		// cut to a new view
		lookFrom[2][0] = myRandf(6000.0f) - 3000.0f;  // new last position
		lookFrom[2][1] = myRandf(1700.0f) + 5.0f;
		lookFrom[2][2] = myRandf(6000.0f) - 3000.0f;
		lookAt[2][0] = myRandf(1000.0f) - 500.0f;  // new last position
		lookAt[2][1] = myRandf(1100.0f) + 200.0f;
		lookAt[2][2] = myRandf(1000.0f) - 500.0f;
		kNewCamera = 0;
	}
	if(kCamera == 1){  // if the camera is active
		if(lastCameraMode == 2){  // camera was controlled by mouse last frame
			cameraTime[0] = 10.0f;
			cameraTime[1] = 0.0f;
			cameraTime[2] = 0.0f;
			lookFrom[2] = lookFrom[0];
			lookFrom[1][0] = myRandf(6000.0f) - 3000.0f;  // new target position
			lookFrom[1][1] = myRandf(1700.0f) + 5.0f;
			lookFrom[1][2] = myRandf(6000.0f) - 3000.0f;
			lookAt[2] = lookAt[0];
			lookAt[1][0] = myRandf(1000.0f) - 500.0f;  // new target position
			lookAt[1][1] = myRandf(1100.0f) + 200.0f;
			lookAt[1][2] = myRandf(1000.0f) - 500.0f;
		}
		cameraTime[1] += elapsedTime;
		cameraTime[2] = cameraTime[1] / cameraTime[0];
		if(cameraTime[2] >= 1.0f){  // reset camera sequence
			// reset timer
			cameraTime[0] = myRandf(25.0f) + 5.0f;
			cameraTime[1] = 0.0f;
			cameraTime[2] = 0.0f;
			// choose new positions
			lookFrom[2] = lookFrom[1];  // last = target
			lookFrom[1][0] = myRandf(6000.0f) - 3000.0f;  // new target position
			lookFrom[1][1] = myRandf(1700.0f) + 5.0f;
			lookFrom[1][2] = myRandf(6000.0f) - 3000.0f;
			lookAt[2] = lookAt[1];  // last = target
			lookAt[1][0] = myRandf(1000.0f) - 500.0f;  // new target position
			lookAt[1][1] = myRandf(1100.0f) + 200.0f;
			lookAt[1][2] = myRandf(1000.0f) - 500.0f;
			if(!myRandi(3)){  // cut to a new view
				lookFrom[2][0] = myRandf(6000.0f) - 3000.0f;  // new last position
				lookFrom[2][1] = myRandf(1700.0f) + 5.0f;
				lookFrom[2][2] = myRandf(6000.0f) - 3000.0f;
				lookAt[2][0] = myRandf(1000.0f) - 500.0f;  // new last position
				lookAt[2][1] = myRandf(1100.0f) + 200.0f;
				lookAt[2][2] = myRandf(1000.0f) - 500.0f;
			}
		}
		// change camera position and angle
		float cameraStep = 0.5f * (1.0f - cosf(cameraTime[2] * PI));
		lookFrom[0] = lookFrom[2] + ((lookFrom[1] - lookFrom[2]) * cameraStep);
		lookAt[0] = lookAt[2] + ((lookAt[1] - lookAt[2]) * cameraStep);
		// update variables used for sound and lens flares
		cameraVel = lookFrom[0] - cameraPos;
		cameraPos = lookFrom[0];
	}

	// Build modelview matrix
	// Don't use gluLookAt() because it's easier to find the billboard matrix
	// if we know the heading and pitch
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	static float heading, pitch;
	// Control camera with the mouse
	if(kCamera == 2){
		heading += 100.0f * elapsedTime * aspectRatio * float(centerx - mousex) / float(xsize);
		pitch += 100.0f * elapsedTime * float(centery - mousey) / float(ysize);
		if(heading > 180.0f)
			heading -= 360.0f;
		if(heading < -180.0f)
			heading += 360.0f;
		if(pitch > 90.0f)
			pitch = 90.0f;
		if(pitch < -90.0f)
			pitch = -90.0f;
		if(mouseButtons & MK_LBUTTON)
			mouseSpeed += 400.0f * elapsedTime;
		if(mouseButtons & MK_RBUTTON)
			mouseSpeed -= 400.0f * elapsedTime;
		if((mouseButtons & MK_MBUTTON) || ((mouseButtons & MK_LBUTTON) && (mouseButtons & MK_RBUTTON)))
			mouseSpeed = 0.0f;
		if(mouseSpeed > 4000.0f)
			mouseSpeed = 4000.0f;
		if(mouseSpeed < -4000.0f)
			mouseSpeed = -4000.0f;
		float ch = cosf(D2R * heading);
		float sh = sinf(D2R * heading);
		float cp = cosf(D2R * pitch);
		float sp = sinf(D2R * pitch);
		lookFrom[0][0] -= mouseSpeed * sh * cp * elapsedTime;
		lookFrom[0][1] += mouseSpeed * sp * elapsedTime;
		lookFrom[0][2] -= mouseSpeed * ch * cp * elapsedTime;
		cameraPos = lookFrom[0];
		// Calculate new lookAt position so that lens flares will be computed correctly
		// and so that transition back to autonomous camera mode is smooth
		lookAt[0][0] = lookFrom[0][0] - 500.0f * sh * cp;
		lookAt[0][1] = lookFrom[0][1] + 500.0f * sp;
		lookAt[0][2] = lookFrom[0][2] - 500.0f * ch * cp;
	}
	else{
		float radius = sqrtf((lookAt[0][0] - lookFrom[0][0]) * (lookAt[0][0] - lookFrom[0][0])
			+ (lookAt[0][2] - lookFrom[0][2]) * (lookAt[0][2] - lookFrom[0][2]));
		pitch = R2D * atan2f(lookAt[0][1] - lookFrom[0][1], radius);
		heading = R2D * atan2f(lookFrom[0][0] - lookAt[0][0], lookFrom[0][2] - lookAt[0][2]);
	}
	glRotatef(-pitch, 1, 0, 0);
	glRotatef(-heading, 0, 1, 0);
	glTranslatef(-lookFrom[0][0], -lookFrom[0][1], -lookFrom[0][2]);
	// get modelview matrix for flares
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMat);

	// store this frame's camera mode for next frame
	lastCameraMode = kCamera;
	// Update mouse idle time
	if(kCamera == 2){
		mouseIdleTime += elapsedTime;
		if(mouseIdleTime > 300.0f)  // return to autonomous camera mode after 5 minutes
			kCamera = 1;
	}

	// update billboard rotation matrix for particles
	glPushMatrix();
	glLoadIdentity();
	glRotatef(heading, 0, 1, 0);
	glRotatef(pitch, 1, 0, 0);
	glGetDoublev(GL_MODELVIEW_MATRIX, billboardMat);
	glPopMatrix();

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT);

	// Slows fireworks, but not camera
	if(kSlowMotion)
		elapsedTime *= 0.5f;

	// Pause the animation?
	if(kAction){
		// update world
		updateWorld();
	
		// darken smoke
		std::list<particle>::iterator darkener = particles.begin();
		static float ambientlight = float(dAmbient) * 0.01f;
		while(darkener != particles.end()){
			if(darkener->depth <= MINDEPTH)
				darkener = particles.end();
			else{
				if(darkener->type == SMOKE)
					darkener->rgb[0] = darkener->rgb[1] = darkener->rgb[2] = ambientlight;
				darkener++;
			}
		}

		// Change rocket firing rate
		static float rocketTimer = 0.0f;
		static float rocketTimeConst = 10.0f / float(dMaxrockets);
		static float changeRocketTimeConst = 20.0f;
		changeRocketTimeConst -= elapsedTime;
		if(changeRocketTimeConst <= 0.0f){
			float temp = myRandf(4.0f);
			rocketTimeConst = (temp * temp) + (10.0f / float(dMaxrockets));
			changeRocketTimeConst = myRandf(30.0f) + 10.0f;
		}
		// add new rocket to list
		rocketTimer -= elapsedTime;
		if((rocketTimer <= 0.0f) || (userDefinedExplosion >= 0)){
			if(numRockets < dMaxrockets){
				particle* rock = addParticle();
				if(myRandi(30) || (userDefinedExplosion >= 0)){  // Usually launch a rocket
					rock->initRocket();
					if(userDefinedExplosion >= 0)
						rock->explosiontype = userDefinedExplosion;
					else{
						if(!myRandi(3000)){  // big one!
							if(myRandi(2))
								rock->explosiontype = 19;  // sucker and shockwave
							else
								rock->explosiontype = 20;  // stretcher and bigmama
						}
						else{
							// Distribution of regular explosions
							if(myRandi(2)){  // 0 - 2 (all types of spheres)
								if(!myRandi(10))
									rock->explosiontype = 2;
								else
									rock->explosiontype = myRandi(2);
							}
							else{
								if(!myRandi(3))  //  ring, double sphere, sphere and ring
									rock->explosiontype = myRandi(3) + 3;
								else{
									if(myRandi(2)){  // 6, 7, 8, 9, 10, 11
										if(myRandi(2))
											rock->explosiontype = myRandi(2) + 6;
										else
											rock->explosiontype = myRandi(4) + 8;
									}
									else{
										if(myRandi(2))  // 12, 13, 14
											rock->explosiontype = myRandi(3) + 12;
										else  // 15 - 18
											rock->explosiontype = myRandi(4) + 15;
									}
								}
							}
						}
					}
					numRockets++;
				}
				else{  // sometimes make fountains and spinners instead of rockets
					rock->initFountain();
					i = myRandi(2);
					for(j=0; j<i; j++){
						rock = addParticle();
						rock->initFountain();
					}
				}
			}
			if(dMaxrockets)
				rocketTimer = myRandf(rocketTimeConst);
			else
				rocketTimer = 60.0f;  // arbitrary number since no rockets ever fire
			if(userDefinedExplosion >= 0){
				userDefinedExplosion = -1;
				rocketTimer = 20.0f;  // Wait 20 seconds after user launches a rocket before launching any more
			}
		}

		// update particles
		numRockets = 0;
		std::list<particle>::iterator curpart = particles.begin();
		while(curpart != particles.end()){
			if(curpart->depth <= MINDEPTH)
				curpart = particles.end();
			else{
				curpart->update();
				if(curpart->type == ROCKET)
					numRockets++;
					curpart->findDepth();
				if(curpart->life <= 0.0f || curpart->xyz[1] < 0.0f){
					if(curpart->type == ROCKET){  // become explosion
						if(curpart->xyz[1] <= 0.0f){  // move above ground for explosion
							curpart->xyz[1] = 0.1f;
							curpart->vel[1] *= -0.7f;
						}
						if(curpart->explosiontype == 18)
							curpart->initSpinner();
						else
							curpart->initExplosion();
					}
					else{
						if(curpart->type == POPPER){
							switch(curpart->explosiontype){
							case STAR:
								curpart->explosiontype = 100;
								curpart->initExplosion();
								break;
							case STREAMER:
								curpart->explosiontype = 101;
								curpart->initExplosion();
								break;
							case METEOR:
								curpart->explosiontype = 102;
								curpart->initExplosion();
								break;
							case POPPER:
								curpart->type = STAR;
								curpart->rgb.set(1.0f, 0.8f, 0.6f);
								curpart->t = curpart->tr = 0.2f;
							}
						}
						else{
							if(curpart->type == SUCKER || curpart->type == STRETCHER){  // become big explosion
								if(curpart->type == SUCKER)
									curpart->initShockwave();
								else
									curpart->initBigmama();
							}
							else  // remove particles from list
								removeParticle(curpart);
						}
					}
				}
				curpart++;
			}
		}
		sortParticles();
	}  // kAction

	else{
		// Only sort particles if they're not being updated (the camera could still be moving)
		std::list<particle>::iterator curpart = particles.begin();
		while(curpart != particles.end()){
			if(curpart->depth <= MINDEPTH)
				curpart = particles.end();
			else{
				curpart->findDepth();
				curpart++;
			}
		}
		sortParticles();
	}

	// the world
	drawWorld();

	// draw particles
	glEnable(GL_BLEND);
	std::list<particle>::iterator drawer = particles.begin();
	while(drawer != particles.end()){
		if(drawer->depth <= MINDEPTH)
			drawer = particles.end();
		else{
			drawer->draw();
			drawer ++;
		}
	}

	// draw lens flares
	if(dFlare){
		makeFlareList();
		for(i=0; i<numFlares; i++){
			if(allflares[i][0] == 0)
				flare(allflares[i][1], allflares[i][2], allflares[i][3], 
					allflares[i][4], allflares[i][5], allflares[i][6]);
			else
				superFlare(allflares[i][1], allflares[i][2], allflares[i][3], 
					allflares[i][4], allflares[i][5], allflares[i][6]);
		}
		numFlares = 0;
	}

	// do sound stuff
	if(dSound){
		float listenerOri[6];
		listenerOri[0] = float(-modelMat[2]);
		listenerOri[1] = float(-modelMat[6]);
		listenerOri[2] = float(-modelMat[10]);
		listenerOri[3] = float(modelMat[1]);
		listenerOri[4] = float(modelMat[5]);
		listenerOri[5] = float(modelMat[9]);
		updateSound(cameraPos.v, cameraVel.v, listenerOri);
	}

	//draw_overlay(elapsedTime);

    glFlush();
	wglSwapLayerBuffers(hdc, WGL_SWAP_MAIN_PLANE);
}


void IdleProc(){
	static DWORD thisTime = timeGetTime(), lastTime;
	static float times[5] = {0.03f, 0.03f, 0.03f, 0.03f, 0.03f};
	static int timeindex = 0;

	// update time
	lastTime = thisTime;
    thisTime = timeGetTime();
    if(thisTime >= lastTime)
		times[timeindex] = float(thisTime - lastTime) * 0.001f;
	else  // else use elapsedTime from last frame
		times[timeindex] = elapsedTime;

	// average last 5 frame times together
	elapsedTime = 0.2f * (times[0] + times[1] + times[2] + times[3] + times[4]);

	timeindex ++;
	if(timeindex >= 5)
		timeindex = 0;

	if(readyToDraw && !checkingPassword && elapsedTime > 0.0f)
		draw();
}


void initSaver(HWND hwnd){
	RECT rect;

	// Initialize pseudorandom number generator
	srand((unsigned)time(NULL));
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();
	rand(); rand(); rand(); rand(); rand();

	// Window initialization
	hdc = GetDC(hwnd);
	SetBestPixelFormat(hdc);
	hglrc = wglCreateContext(hdc);
	GetClientRect(hwnd, &rect);
	wglMakeCurrent(hdc, hglrc);
	xsize = rect.right - rect.left;
	ysize = rect.bottom - rect.top;
	centerx = rect.left + xsize / 2;
	centery = rect.top + ysize / 2;
	glViewport(rect.left, rect.top, xsize, ysize);
	glGetIntegerv(GL_VIEWPORT, viewport);
	aspectRatio = float(rect.right) / float(rect.bottom);

	//glMatrixMode(GL_MODELVIEW_MATRIX);

	// Set OpenGL state
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	// Initialize data structures
	initFlares();
	//initRockets();
	if(dSmoke)
		initSmoke();
	initWorld();
	initShockwave();
	if(dSound)
		initSound(hwnd);

	//init_overlay();
}


void cleanup(HWND hwnd){
	// Free memory
	std::list<particle>::iterator killer = particles.begin();
	while(killer != particles.end()){
		killer = particles.erase(killer);
	}

	// clean up sound data structures
	if(dSound)
		cleanUpSound();

	// Kill device context
	ReleaseDC(hwnd, hdc);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc);
}


void setDefaults(){
	dMaxrockets = 8;
	dSmoke = 5;
	dExplosionsmoke = 0;
	dWind = 20;
	dAmbient = 10;
	dStardensity = 20;
	dFlare = 20;
	dMoonglow = 20;
	dSound = 100;
	dMoon = 1;
	dClouds = 1;
	dEarth = 1;
	dIllumination = 1;
}


// Initialize all user-defined stuff
void readRegistry(){
	LONG result;
	HKEY skey;
	DWORD valtype, valsize, val;

	setDefaults();
	dPriority = 0;

	result = RegOpenKeyEx(HKEY_CURRENT_USER, registryPath, 0, KEY_READ, &skey);
	if(result != ERROR_SUCCESS)
		return;

	valsize=sizeof(val);

	result = RegQueryValueEx(skey, "Maxrockets", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dMaxrockets = val;
	result = RegQueryValueEx(skey, "Smoke", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dSmoke = val;
	result = RegQueryValueEx(skey, "Explosionsmoke", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dExplosionsmoke = val;
	result = RegQueryValueEx(skey, "Wind", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dWind = val;
	result = RegQueryValueEx(skey, "Ambient", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dAmbient = val;
	result = RegQueryValueEx(skey, "Stardensity", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dStardensity = val;
	result = RegQueryValueEx(skey, "Flare", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dFlare = val;
	result = RegQueryValueEx(skey, "Moonglow", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dMoonglow = val;
	result = RegQueryValueEx(skey, "Sound", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dSound = val;
	result = RegQueryValueEx(skey, "Moon", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dMoon = val;
	result = RegQueryValueEx(skey, "Clouds", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dClouds = val;
	result = RegQueryValueEx(skey, "Earth", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dEarth = val;
	result = RegQueryValueEx(skey, "Illumination", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dIllumination = val;
	result = RegQueryValueEx(skey, "Priority", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		dPriority = val;
	result = RegQueryValueEx(skey, "theVideoMode", 0, &valtype, (LPBYTE)&val, &valsize);
	if(result == ERROR_SUCCESS)
		theVideoMode = val;

	RegCloseKey(skey);
}


// Save all user-defined stuff
void writeRegistry(){
    LONG result;
	HKEY skey;
	DWORD val, disp;

	result = RegCreateKeyEx(HKEY_CURRENT_USER, registryPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &skey, &disp);
	if(result != ERROR_SUCCESS)
		return;

	val = dMaxrockets;
	RegSetValueEx(skey, "Maxrockets", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dSmoke;
	RegSetValueEx(skey, "Smoke", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dExplosionsmoke;
	RegSetValueEx(skey, "Explosionsmoke", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dWind;
	RegSetValueEx(skey, "Wind", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dAmbient;
	RegSetValueEx(skey, "Ambient", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dStardensity;
	RegSetValueEx(skey, "Stardensity", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dFlare;
	RegSetValueEx(skey, "Flare", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dMoonglow;
	RegSetValueEx(skey, "Moonglow", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dSound;
	RegSetValueEx(skey, "Sound", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dMoon;
	RegSetValueEx(skey, "Moon", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dClouds;
	RegSetValueEx(skey, "Clouds", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dEarth;
	RegSetValueEx(skey, "Earth", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dIllumination;
	RegSetValueEx(skey, "Illumination", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = dPriority;
	RegSetValueEx(skey, "Priority", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));
	val = theVideoMode;
	RegSetValueEx(skey, "theVideoMode", 0, REG_DWORD, (CONST BYTE*)&val, sizeof(val));

	RegCloseKey(skey);
}


BOOL aboutProc(HWND hdlg, UINT msg, WPARAM wpm, LPARAM lpm){
	switch(msg){
	case WM_CTLCOLORSTATIC:
		if(HWND(lpm) == GetDlgItem(hdlg, WEBPAGE)){
			SetTextColor(HDC(wpm), RGB(0,0,255));
			SetBkColor(HDC(wpm), COLORREF(GetSysColor(COLOR_3DFACE)));
			return(int(GetSysColorBrush(COLOR_3DFACE)));
		}
		break;
    case WM_COMMAND:
		switch(LOWORD(wpm)){
		case IDOK:
		case IDCANCEL:
			EndDialog(hdlg, LOWORD(wpm));
			break;
		case WEBPAGE:
			ShellExecute(NULL, "open", "http://www.reallyslick.com", NULL, NULL, SW_SHOWNORMAL);
		}
	}
	return FALSE;
}


void initControls(HWND hdlg){
	char cval[16];

	SendDlgItemMessage(hdlg, MAXROCKETS, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(0), DWORD(100))));
	SendDlgItemMessage(hdlg, MAXROCKETS, TBM_SETPOS, 1, LPARAM(dMaxrockets));
	SendDlgItemMessage(hdlg, MAXROCKETS, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, MAXROCKETS, TBM_SETPAGESIZE, 0, LPARAM(5));
	sprintf(cval, "%d", dMaxrockets);
	SendDlgItemMessage(hdlg, MAXROCKETSTEXT, WM_SETTEXT, 0, LPARAM(cval));
	SendDlgItemMessage(hdlg, SMOKELIFE, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(0), DWORD(60))));
	SendDlgItemMessage(hdlg, SMOKELIFE, TBM_SETPOS, 1, LPARAM(dSmoke));
	SendDlgItemMessage(hdlg, SMOKELIFE, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, SMOKELIFE, TBM_SETPAGESIZE, 0, LPARAM(5));
	sprintf(cval, "%d", dSmoke);
	SendDlgItemMessage(hdlg, SMOKELIFETEXT, WM_SETTEXT, 0, LPARAM(cval));
	SendDlgItemMessage(hdlg, EXPLOSIONSMOKE, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(0), DWORD(WHICHSMOKES))));
	SendDlgItemMessage(hdlg, EXPLOSIONSMOKE, TBM_SETPOS, 1, LPARAM(dExplosionsmoke));
	SendDlgItemMessage(hdlg, EXPLOSIONSMOKE, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, EXPLOSIONSMOKE, TBM_SETPAGESIZE, 0, LPARAM(5));
	sprintf(cval, "%d", dExplosionsmoke);
	SendDlgItemMessage(hdlg, EXPLOSIONSMOKETEXT, WM_SETTEXT, 0, LPARAM(cval));
	SendDlgItemMessage(hdlg, WIND, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(0), DWORD(100))));
	SendDlgItemMessage(hdlg, WIND, TBM_SETPOS, 1, LPARAM(dWind));
	SendDlgItemMessage(hdlg, WIND, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, WIND, TBM_SETPAGESIZE, 0, LPARAM(5));
	sprintf(cval, "%d", dWind);
	SendDlgItemMessage(hdlg, WINDTEXT, WM_SETTEXT, 0, LPARAM(cval));
	SendDlgItemMessage(hdlg, AMBIENT, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(0), DWORD(100))));
	SendDlgItemMessage(hdlg, AMBIENT, TBM_SETPOS, 1, LPARAM(dAmbient));
	SendDlgItemMessage(hdlg, AMBIENT, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, AMBIENT, TBM_SETPAGESIZE, 0, LPARAM(5));
	sprintf(cval, "%d", dAmbient);
	SendDlgItemMessage(hdlg, AMBIENTTEXT, WM_SETTEXT, 0, LPARAM(cval));
	SendDlgItemMessage(hdlg, STARDENSITY, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(0), DWORD(100))));
	SendDlgItemMessage(hdlg, STARDENSITY, TBM_SETPOS, 1, LPARAM(dStardensity));
	SendDlgItemMessage(hdlg, STARDENSITY, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, STARDENSITY, TBM_SETPAGESIZE, 0, LPARAM(5));
	sprintf(cval, "%d", dStardensity);
	SendDlgItemMessage(hdlg, STARDENSITYTEXT, WM_SETTEXT, 0, LPARAM(cval));
	SendDlgItemMessage(hdlg, FLARE, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(0), DWORD(100))));
	SendDlgItemMessage(hdlg, FLARE, TBM_SETPOS, 1, LPARAM(dFlare));
	SendDlgItemMessage(hdlg, FLARE, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, FLARE, TBM_SETPAGESIZE, 0, LPARAM(5));
	sprintf(cval, "%d", dFlare);
	SendDlgItemMessage(hdlg, FLARETEXT, WM_SETTEXT, 0, LPARAM(cval));
	SendDlgItemMessage(hdlg, MOONGLOW, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(0), DWORD(100))));
	SendDlgItemMessage(hdlg, MOONGLOW, TBM_SETPOS, 1, LPARAM(dMoonglow));
	SendDlgItemMessage(hdlg, MOONGLOW, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, MOONGLOW, TBM_SETPAGESIZE, 0, LPARAM(5));
	sprintf(cval, "%d", dMoonglow);
	SendDlgItemMessage(hdlg, MOONGLOWTEXT, WM_SETTEXT, 0, LPARAM(cval));
	SendDlgItemMessage(hdlg, SOUND, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(0), DWORD(100))));
	SendDlgItemMessage(hdlg, SOUND, TBM_SETPOS, 1, LPARAM(dSound));
	SendDlgItemMessage(hdlg, SOUND, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, SOUND, TBM_SETPAGESIZE, 0, LPARAM(5));
	sprintf(cval, "%d", dSound);
	SendDlgItemMessage(hdlg, SOUNDTEXT, WM_SETTEXT, 0, LPARAM(cval));
	CheckDlgButton(hdlg, MOON, dMoon);
	CheckDlgButton(hdlg, CLOUDS, dClouds);
	CheckDlgButton(hdlg, EARTH, dEarth);
	CheckDlgButton(hdlg, ILLUMINATION, dIllumination);
	SendDlgItemMessage(hdlg, PRIORITY, TBM_SETRANGE, 0, LPARAM(MAKELONG(DWORD(0), DWORD(2))));
	SendDlgItemMessage(hdlg, PRIORITY, TBM_SETPOS, 1, LPARAM(dPriority));
	SendDlgItemMessage(hdlg, PRIORITY, TBM_SETLINESIZE, 0, LPARAM(1));
	SendDlgItemMessage(hdlg, PRIORITY, TBM_SETPAGESIZE, 0, LPARAM(1));
	sprintf(cval, "%d", dPriority);
	SendDlgItemMessage(hdlg, PRIORITYTEXT, WM_SETTEXT, 0, LPARAM(cval));

	InitVideoModeComboBox(hdlg, VIDEOMODE);
}


BOOL ScreenSaverConfigureDialog(HWND hdlg, UINT msg, WPARAM wpm, LPARAM lpm){
	int ival;
	char cval[16];

    switch(msg){
    case WM_INITDIALOG:
        InitCommonControls();
        readRegistry();
        initControls(hdlg);
        return TRUE;
    case WM_COMMAND:
        switch(LOWORD(wpm)){
        case IDOK:
			dMaxrockets = SendDlgItemMessage(hdlg, MAXROCKETS, TBM_GETPOS, 0, 0);
            dSmoke = SendDlgItemMessage(hdlg, SMOKELIFE, TBM_GETPOS, 0, 0);
			dExplosionsmoke = SendDlgItemMessage(hdlg, EXPLOSIONSMOKE, TBM_GETPOS, 0, 0);
			dWind = SendDlgItemMessage(hdlg, WIND, TBM_GETPOS, 0, 0);
			dAmbient = SendDlgItemMessage(hdlg, AMBIENT, TBM_GETPOS, 0, 0);
			dStardensity = SendDlgItemMessage(hdlg, STARDENSITY, TBM_GETPOS, 0, 0);
			dFlare = SendDlgItemMessage(hdlg, FLARE, TBM_GETPOS, 0, 0);
			dMoonglow = SendDlgItemMessage(hdlg, MOONGLOW, TBM_GETPOS, 0, 0);
			dSound = SendDlgItemMessage(hdlg, SOUND, TBM_GETPOS, 0, 0);
			dMoon = (IsDlgButtonChecked(hdlg, MOON) == BST_CHECKED);
			dClouds = (IsDlgButtonChecked(hdlg, CLOUDS) == BST_CHECKED);
			dEarth = (IsDlgButtonChecked(hdlg, EARTH) == BST_CHECKED);
			dIllumination = (IsDlgButtonChecked(hdlg, ILLUMINATION) == BST_CHECKED);
			dPriority = SendDlgItemMessage(hdlg, PRIORITY, TBM_GETPOS, 0, 0);
			theVideoMode = SendDlgItemMessage(hdlg, VIDEOMODE, CB_GETCURSEL, 0, 0);
			writeRegistry();
            // Fall through
        case IDCANCEL:
            EndDialog(hdlg, LOWORD(wpm));
            break;
		case DEFAULTS:
			setDefaults();
			initControls(hdlg);
			break;
		case ABOUT:
			DialogBox(mainInstance, MAKEINTRESOURCE(DLG_ABOUT), hdlg, DLGPROC(aboutProc));
		}
        return TRUE;
	case WM_HSCROLL:
		if(HWND(lpm) == GetDlgItem(hdlg, MAXROCKETS)){
			ival = SendDlgItemMessage(hdlg, MAXROCKETS, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, MAXROCKETSTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, SMOKELIFE)){
			ival = SendDlgItemMessage(hdlg, SMOKELIFE, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, SMOKELIFETEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, EXPLOSIONSMOKE)){
			ival = SendDlgItemMessage(hdlg, EXPLOSIONSMOKE, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, EXPLOSIONSMOKETEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, WIND)){
			ival = SendDlgItemMessage(hdlg, WIND, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, WINDTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, AMBIENT)){
			ival = SendDlgItemMessage(hdlg, AMBIENT, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, AMBIENTTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, STARDENSITY)){
			ival = SendDlgItemMessage(hdlg, STARDENSITY, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, STARDENSITYTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, FLARE)){
			ival = SendDlgItemMessage(hdlg, FLARE, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, FLARETEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, MOONGLOW)){
			ival = SendDlgItemMessage(hdlg, MOONGLOW, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, MOONGLOWTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, SOUND)){
			ival = SendDlgItemMessage(hdlg, SOUND, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, SOUNDTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		if(HWND(lpm) == GetDlgItem(hdlg, PRIORITY)){
			ival = SendDlgItemMessage(hdlg, PRIORITY, TBM_GETPOS, 0, 0);
			sprintf(cval, "%d", ival);
			SendDlgItemMessage(hdlg, PRIORITYTEXT, WM_SETTEXT, 0, LPARAM(cval));
		}
		return TRUE;
    }
    return FALSE;
}


LONG ScreenSaverProc(HWND hwnd, UINT msg, WPARAM wpm, LPARAM lpm){
	switch(msg){
	case WM_CREATE:
		readRegistry();
		initSaver(hwnd);
		switch(dPriority){
		case 0:
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
			break;
		case 1:
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
			break;
		case 2:
			SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
		}
		readyToDraw = 1;
		break;
	case WM_KEYDOWN:
		switch(int(wpm)){
		// pause the motion of the fireworks
		case 'a':
		case 'A':
			if(kAction)
				kAction = 0;
			else
				kAction = 1;
			if(kSlowMotion)
				kSlowMotion = 0;
			return(0);
		// pause the motion of the camera
		case 'c':
		case 'C':
			if(kCamera == 0)
				kCamera = 1;
			else{
				if(kCamera == 1)
					kCamera = 0;
				else{
					if(kCamera == 2)
						kCamera = 1;
				}
			}
			return(0);
		// toggle mouse camera control
		case 'm':
		case 'M':
			if(kCamera == 2)
				kCamera = 1;
			else{
				kCamera = 2;
				mouseSpeed = 0.0f;
				mouseIdleTime = 0.0f;
			}
			return(0);
		// new camera view
		case 'n':
		case 'N':
			kNewCamera = 1;
			return(0);
		// slow the motion of the fireworks
		case 's':
		case 'S':
			if(kSlowMotion)
				kSlowMotion = 0;
			else
				kSlowMotion = 1;
			if(!kAction)
				kAction = 1;
			return(0);
		// choose which type of rocket explosion
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			userDefinedExplosion = int(wpm) - 49;  // explosions 0 - 8
			return(0);
		case '0':
			userDefinedExplosion = 9;
			return(0);
		case 'q':
		case 'Q':
			userDefinedExplosion = 10;
			return(0);
		case 'w':
		case 'W':
			userDefinedExplosion = 11;
			return(0);
		case 'e':
		case 'E':
			userDefinedExplosion = 12;
			return(0);
		case 'r':
		case 'R':
			userDefinedExplosion = 13;
			return(0);
		case 't':
		case 'T':
			userDefinedExplosion = 14;
			return(0);
		case 'y':
		case 'Y':
			userDefinedExplosion = 15;
			return(0);
		case 'u':
		case 'U':
			userDefinedExplosion = 16;
			return(0);
		case 'i':
		case 'I':
			userDefinedExplosion = 17;
			return(0);
		case 'o':
		case 'O':
			userDefinedExplosion = 18;  // spinner
			return(0);
		case 'b':
		case 'B':
		case 'd':
		case 'D':
		case 'f':
		case 'F':
		case 'g':
		case 'G':
		case 'h':
		case 'H':
		case 'j':
		case 'J':
		case 'k':
		case 'K':
		case 'l':
		case 'L':
		case 'p':
		case 'P':
		case 'v':
		case 'V':
		case 'x':
		case 'X':
		case 'z':
		case 'Z':
			// These letters do nothing
			// Disabling them makes it harder to make mistakes
			return(0);
		}
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
		if(kCamera == 2){
			mouseButtons = wpm;        // key flags 
			mousex = LOWORD(lpm);  // horizontal position of cursor 
			mousey = HIWORD(lpm);
			mouseIdleTime = 0.0f;
			return(0);
		}
		break;
	case WM_DESTROY:
		readyToDraw = 0;
		cleanup(hwnd);
		break;
	}
	return DefScreenSaverProc(hwnd, msg, wpm, lpm);
}