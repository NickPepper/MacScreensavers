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


#ifndef SOUND_H
#define SOUND_H



#include <list>
#include <al\al.h>
#include <al\alc.h>
#include "../../rsMath/rsMath.h"
#include "launchsound.h"
#include "boomsound.h"
#include "poppersound.h"
#include "sucksound.h"
#include "nukesound.h"
#include "whistlesound.h"


#define NUM_SOURCES 16
#define NUM_BUFFERS 10
#define LAUNCH1SOUND 0
#define LAUNCH2SOUND 1
#define BOOM1SOUND 2
#define BOOM2SOUND 3
#define BOOM3SOUND 4
#define BOOM4SOUND 5
#define POPPERSOUND 6
#define SUCKSOUND 7
#define NUKESOUND 8
#define WHISTLESOUND 9

// reference distance, max distance
// reference distance = sound is partly attenuated
// maximum distance = sound is completely attenuated
float sound_distances[NUM_BUFFERS][2] = 
	{300.0f, 600.0f,  // launch sounds
	300.0f, 600.0f,
	4000.0f, 8000.0f,  // booms
	4000.0f, 8000.0f,
	4000.0f, 8000.0f,
	6000.0f, 12000.0f,
	2500.0f, 5000.0f,  // poppers
	10000.0f, 20000.0f,  // suck
	20000.0f, 40000.0f,  // nuke
	2500.0f, 5000.0f,  // whistle
};



class soundnode;

ALuint g_Buffers[NUM_BUFFERS];
ALuint g_Sources[NUM_SOURCES];
extern float elapsedTime;
extern int dSound;
extern int kSlowMotion;




void initSound(HWND hwnd){
	ALCcontext *Context;
	ALCdevice *Device;

	// Open device
 	Device=alcOpenDevice((ALubyte*)"DirectSound3D");
	// Create context(s)
	Context=alcCreateContext(Device,NULL);
	// Set active context
	alcMakeContextCurrent(Context);

	//alDistanceModel(AL_INVERSE_DISTANCE);
	// As of October, 2001, AL_ROLLOFF_FACTOR isn't implemented in the Windows 
	// version of OpenAL, so the distance attenuation models won't work.  You 
	// have to set the gains yourself.
	alDistanceModel(AL_NONE);

	alDopplerVelocity(1130.0f);  // Sound travels at 1130 feet/sec
	alListenerf(AL_GAIN, float(dSound) * 0.01f);  // Volume

	// Initialize sound data
	alGenBuffers(NUM_BUFFERS, g_Buffers);
	alGenSources(NUM_SOURCES, g_Sources);
	alBufferData(g_Buffers[LAUNCH1SOUND], AL_FORMAT_MONO16, launch1SoundData, launch1SoundSize, 44100);
	alBufferData(g_Buffers[LAUNCH2SOUND], AL_FORMAT_MONO16, launch2SoundData, launch2SoundSize, 44100);
	alBufferData(g_Buffers[BOOM1SOUND], AL_FORMAT_MONO16, boom1SoundData, boom1SoundSize, 44100);
	alBufferData(g_Buffers[BOOM2SOUND], AL_FORMAT_MONO16, boom2SoundData, boom2SoundSize, 44100);
	alBufferData(g_Buffers[BOOM3SOUND], AL_FORMAT_MONO16, boom3SoundData, boom3SoundSize, 44100);
	alBufferData(g_Buffers[BOOM4SOUND], AL_FORMAT_MONO16, boom4SoundData, boom4SoundSize, 44100);
	alBufferData(g_Buffers[POPPERSOUND], AL_FORMAT_MONO16, popperSoundData, popperSoundSize, 44100);
	alBufferData(g_Buffers[SUCKSOUND], AL_FORMAT_MONO16, suckSoundData, suckSoundSize, 44100);
	alBufferData(g_Buffers[NUKESOUND], AL_FORMAT_MONO16, nukeSoundData, nukeSoundSize, 44100);
	alBufferData(g_Buffers[WHISTLESOUND], AL_FORMAT_MONO16, whistleSoundData, whistleSoundSize, 44100);
}



class soundnode{
public:
	int sound;
	float pos[3];
	float dist;
	float time;  // time until sound plays

	soundnode(){};
	~soundnode(){};
};


std::list<soundnode> soundlist;


void insertSoundNode(int sound, rsVec source, rsVec observer){
	soundnode* node;
	rsVec dir = observer - source;

	soundlist.push_back(soundnode());
	node = &soundlist.back();
	node->sound = sound;
	node->pos[0] = source[0];
	node->pos[1] = source[1];
	node->pos[2] = source[2];
	// distance to sound
	node->dist = sqrtf(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
	// Sound travels at 1130 feet/sec
	node->time = node->dist * 0.000885f;
	if(node->sound == POPPERSOUND)  // poppers have a little delay
		node->time += 2.5f;
}

void updateSound(float* listenerPos, float* listenerVel, float* listenerOri){
	static int source = 0;

	// Set current listener attributes
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);

	std::list<soundnode>::iterator cursound = soundlist.begin();
	while(cursound != soundlist.end()){
		cursound->time -= elapsedTime;
		if(cursound->time <= 0.0f){
				if(cursound->dist < sound_distances[cursound->sound][1]){
					alSourceStop(g_Sources[source]);
					alSourcei(g_Sources[source], AL_BUFFER, g_Buffers[cursound->sound]);
					//alSourcef(g_Sources[source], AL_REFERENCE_DISTANCE, sound_distances[cursound->sound][0]);
					//alSourcef(g_Sources[source], AL_ROLLOFF_FACTOR, 0.013557606f);
					// HACK:  AL_REFERENCE_DISTANCE must be set or OpenAL won't make any sound at all.
					// I assume this will be fixed in future implementations of OpenAL.
					alSourcef(g_Sources[source], AL_REFERENCE_DISTANCE, 1000000.0f);
					// As of October, 2001, AL_ROLLOFF_FACTOR isn't implemented in the Windows 
					// version of OpenAL, so the distance attenuation models won't work.  You 
					// have to set the gains yourself.
					// Here I implement the AL_INVERSE_DISTANCE model formula from the OpenAL spec.
					//float gain = 1.0f - 20.0f * log10(1.0f + 0.013557606f 
					//	* (cursound->dist - sound_distances[cursound->sound][0]) 
					//	/ sound_distances[cursound->sound][0]);
					// But I don't like the way it sounds, so I'll just fudge everything
					float gain = 1.0f - (cursound->dist / sound_distances[cursound->sound][1]);
					if(gain > 1.0f)
						gain = 1.0f;
					if(gain < 0.0f)
						gain = 0.0f;
					alSourcef(g_Sources[source], AL_GAIN, gain);
					alSourcefv(g_Sources[source], AL_POSITION, cursound->pos);
					alSourcei(g_Sources[source], AL_LOOPING, AL_FALSE);
					if(kSlowMotion)  // Slow down the sound
						alSourcef(g_Sources[source], AL_PITCH, 0.5f);
					else  // Sound at regular speed
						alSourcef(g_Sources[source], AL_PITCH, 1.0f);
					alSourcePlay(g_Sources[source]);
					source ++;
					if(source >= NUM_SOURCES)
						source = 0;
				}
			cursound = soundlist.erase(cursound);
		}
		else
			cursound++;
	}
}



void cleanUpSound(){
	ALCcontext *Context;
	ALCdevice *Device;

	std::list<soundnode>::iterator cursound = soundlist.begin();
	while(cursound != soundlist.end()){
		cursound = soundlist.erase(cursound);
	}

	alDeleteBuffers(NUM_BUFFERS, g_Buffers);
	alDeleteSources(NUM_SOURCES, g_Sources);
	//Get active context
	Context = alcGetCurrentContext();
	//Get device for active context
	Device = alcGetContextsDevice(Context);
	//Release context(s)
	alcDestroyContext(Context);
	//Close device
	alcCloseDevice(Device);

}



#endif  // SOUND_H