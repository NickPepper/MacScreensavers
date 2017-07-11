#ifndef __FLUX__
#define __FLUX__

#define NUMCONSTS 8
#define PIx2 6.28318530718f
#define DEG2RAD 0.0174532925f
#define LIGHTSIZE 64

class flux;
class particle;

typedef struct FluxSaverSettings
{
    int standardSet;
    
    flux *fluxes;
    float lumdiff;
    int whichparticle;
    float cameraAngle;
    float cosCameraAngle, sinCameraAngle;
    unsigned char lightTexture[LIGHTSIZE][LIGHTSIZE];
    // Parameters edited in the dialog box
    int dFluxes;
    int dParticles;
    int dTrail;
    int dGeometry;
    int dSize;
    int dComplexity;
    int dRandomize;
    int dExpansion;
    int dRotation;
    int dWind;
    int dInstability;
    int dBlur;
    int dSmart;
} FluxSaverSettings;

// This class is poorly named.  It's actually a whole trail of particles.

class particle
{
    public:
    
        int trail;
    
        float **vertices;
        int counter;
        float offset[3];
    
        particle(FluxSaverSettings * inSettings);
        virtual ~particle();
        float update(float *c,FluxSaverSettings * inSettings);
};

class flux
{
    public:
    
        particle *particles;
        int randomize;
        float c[NUMCONSTS];     // constants
        float cv[NUMCONSTS];    // constants' change velocities
        int currentSmartConstant;
        float oldDistance;
    
        flux(FluxSaverSettings * inSettings);
        virtual ~flux();
        
        void update(FluxSaverSettings * inSettings);
};

__private_extern__ void draw(FluxSaverSettings * inSettings);
__private_extern__ void initSaver(int width,int height,FluxSaverSettings * inSettings);
__private_extern__ void cleanSettings(FluxSaverSettings * inSettings);
__private_extern__ void setDefaults(int which,FluxSaverSettings * inSettings);

#endif