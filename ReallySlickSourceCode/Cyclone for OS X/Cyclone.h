#ifndef __CYCLONE__
#define __CYCLONE__

class cyclone;
class particle;


#define PIx2 6.28318530718f
#define PI 3.14159265359f
#define wide 200
#define high 200

// Global variables

// Parameters edited in the dialog box

typedef struct CycloneSaverSettings
{
    int dCyclones;
    int dParticles;
    int dSize;
    int dComplexity;
    int dSpeed;
    
    int dStretch;
    int dShowCurves;
    // Other globals
    cyclone **cyclones;
    particle **particles;

    float fact[13];
} CycloneSaverSettings;

class cyclone
{
    public:
        int dComplexity;
        
        float **targetxyz;
        float **xyz;
        float **oldxyz;
        float *targetWidth;
        float *width;
        float *oldWidth;
        float targethsl[3];
        float hsl[3];
        float oldhsl[3];
        int **xyzChange;
        int **widthChange;
        int hslChange[2];
    
        cyclone(CycloneSaverSettings * inSettings);
        virtual ~cyclone();
        void update(CycloneSaverSettings * inSettings);
};

class particle
{
    public:
        float r, g, b;
        float xyz[3], lastxyz[3];
        float width;
        float step;
        float spinAngle;
        cyclone *cy;
    
        particle(cyclone *);
        virtual ~particle(){};
        void init();
        void update(CycloneSaverSettings * inSettings);
};

__private_extern__ void draw(CycloneSaverSettings * inSettings);
__private_extern__ void initSaver(int width, int height,CycloneSaverSettings * inSettings);
__private_extern__ void setDefaults(CycloneSaverSettings * inSettings);
__private_extern__ void cleanSettings(CycloneSaverSettings * inSettings);


#endif