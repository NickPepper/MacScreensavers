#ifndef __PLASMA__
#define __PLASMA__

#define NUMCONSTS 18
#define MAXTEXSIZE 1024

typedef struct PlasmaSaverSettings
{
    float aspectRatio;
    float wide;
    float high;
    float c[NUMCONSTS];  // constant
    float ct[NUMCONSTS];  // temporary value of constant
    float cv[NUMCONSTS];  // velocity of constant
    float ***position;
    float ***plasma;
    int texsize;
    int plasmasize;
    float *plasmamap;
    // Parameters edited in the dialog box
    int dZoom;
    int dFocus;
    int dSpeed;
    int dResolution;
} PlasmaSaverSettings;


__private_extern__ void draw(PlasmaSaverSettings * inSettings);

__private_extern__ void cleanSettings(PlasmaSaverSettings * inSettings);
__private_extern__ void initSaver(int width,int height,PlasmaSaverSettings * inSettings);
__private_extern__ void setDefaults(PlasmaSaverSettings * inSettings);

#endif