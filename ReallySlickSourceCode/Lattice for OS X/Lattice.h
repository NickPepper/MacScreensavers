#ifndef __LATTICE__
#define __LATTICE__

#include "rsMath.h"

#define PI 3.14159265359f
#define PIx2 6.28318530718f
#define D2R 0.0174532925f
#define R2D 57.2957795131f
#define NUMOBJECTS 10
#define LATSIZE 10

#define LATTICE_TEXTURE_NONE		0
#define LATTICE_TEXTURE_INDUSTRIAL	1
#define LATTICE_TEXTURE_CRYSTAL		2
#define LATTICE_TEXTURE_CHROME		3
#define LATTICE_TEXTURE_BRASS		4
#define LATTICE_TEXTURE_SHINY		5
#define LATTICE_TEXTURE_GHOSTLY		6
#define LATTICE_TEXTURE_CIRCUITS	7
#define LATTICE_TEXTURE_DOUGHNUTS	8
#define LATTICE_TEXTURE_RANDOM		9

class camera;

typedef struct LatticeSaverSettings
{
    int standardSet;
    
    int seg;  // Section of path
	float where;
    
    rsVec oldxyz;
	rsVec oldDir;
	rsVec oldAngvel;
    
    rsQuat quat;
    
    int flymode;
    float flymodeChange;
    float rollVel;
    float rollAcc;
    float rollChange;
    float elapsedTime;
    int dLongitude;
    int dLatitude;
    int dThick;
    int dDensity;
    int dDepth;
    int dFov;
    int dPathrand;
    int dSpeed;
    int dTexture;
    int dUsedTexture;
    unsigned char dSmooth;
    unsigned char dFog;
    unsigned char dWidescreen;
    
    camera* theCamera;
    
    int lattice[LATSIZE][LATSIZE][LATSIZE];
    int globalxyz[3];
    int lastBorder;
    float ** bPnt;  // Border points and direction vectors where camera can cross from cube to cube
    float ** path;
    
} LatticeSaverSettings;

__private_extern__ void reconfigure(LatticeSaverSettings * inSettings);

__private_extern__ void makeLatticeObjects(LatticeSaverSettings * inSettings);

__private_extern__ void draw(LatticeSaverSettings * inSettings);

__private_extern__ void initSaver(int width,int height,LatticeSaverSettings * inSettings);

__private_extern__ void cleanSettings(LatticeSaverSettings * inSettings);

__private_extern__ void setDefaults(int which,LatticeSaverSettings * inSettings);

#endif