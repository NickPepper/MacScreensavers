#ifndef __SOLARWINDS__
#define __SOLARWINDS__

#define DEG2RAD 0.0174532925f
#define LIGHTSIZE 64
#define NUMCONSTS 9

class wind;

typedef struct SolarWindsSaverSettings
{
    int standardSet;

    wind *winds;

    unsigned char lightTexture[LIGHTSIZE][LIGHTSIZE];
    
    // Parameters edited in the dialog box
    
    int dWinds;
    int dEmitters;
    int dParticles;
    int dGeometry;
    int dSize;
    int dParticlespeed;
    int dEmitterspeed;
    int dWindspeed;
    int dBlur;
} SolarWindsSaverSettings;

class wind
{
    public:
    
        int dParticles;
        int dGeometry;
        int dEmitters;
        
        float **emitters;
        float **particles;
        int **linelist;
        int *lastparticle;
        int whichparticle;
        float c[NUMCONSTS];
        float ct[NUMCONSTS];
        float cv[NUMCONSTS];
    
        wind(SolarWindsSaverSettings * inSettings);
        virtual ~wind();
        void update(SolarWindsSaverSettings * inSettings);
};

__private_extern__ void draw(SolarWindsSaverSettings * inSettings);

__private_extern__ void cleanSettings(SolarWindsSaverSettings * inSettings);
__private_extern__ void initSaver(int width,int height,SolarWindsSaverSettings * inSettings);
__private_extern__ void setDefaults(int which,SolarWindsSaverSettings * inSettings);

#endif