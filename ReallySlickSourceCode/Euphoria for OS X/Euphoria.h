#ifndef __EUPHORIA__
#define __EUPHORIA__

#ifdef __MACOSXSCREENSAVER__

#define NUMCONSTS 9

class wisp;

typedef struct EuphoriaSaverSettings
{
    int standardSet;
    
    wisp * wisps;
    wisp * backwisps;
    
    unsigned char* feedbackmap;
    float aspectRatio;
    int viewport[4];
    float elapsedTime;
    // Parameters edited in the dialog box
    int dWisps;
    int dBackground;
    int dDensity;
    int dVisibility;
    int dSpeed;
    int dFeedback;
    int dFeedbackspeed;
    int dFeedbacksize;
    int dWireframe;
    int dTexture;
    
    unsigned int tex;
    
    
    // feedback texture object
    unsigned int feedbacktex;
    int feedbacktexsize;
    // feedback variables
    float fr[4];
    float fv[4];
    float f[4];
    // feedback limiters
    float lr[3];
    float lv[3];
    float l[3];
} EuphoriaSaverSettings;

class wisp
{
    public:
    
        float ***vertices;
        float c[NUMCONSTS];     // constants
        float cr[NUMCONSTS];    // constants' radial position
        float cv[NUMCONSTS];    // constants' change velocities
        float hsl[3];
        float rgb[3];
        float hueSpeed;
        float saturationSpeed;
        
        int dDensity;
    
        wisp(EuphoriaSaverSettings * inSettings);
        ~wisp();
        
        void update(EuphoriaSaverSettings * inSettings);
        void draw(EuphoriaSaverSettings * inSettings);
        void drawAsBackground(EuphoriaSaverSettings * inSettings);
};

__private_extern__ void initSaver(int width,int height,EuphoriaSaverSettings * inSettings);
__private_extern__ void draw(EuphoriaSaverSettings * inSettings);

__private_extern__ void cleanSettings(EuphoriaSaverSettings * inSettings);

__private_extern__ void setDefaults(int which,EuphoriaSaverSettings * inSettings);
#endif

#endif