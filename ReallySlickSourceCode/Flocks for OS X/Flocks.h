#ifndef __FLOCKS__
#define __FLOCKS__

class bug;

typedef struct FlocksSaverSettings
{
    float elapsedTime;
    float aspectRatio;
    int wide;
    int high;
    int deep;
    bug *lBugs;
    bug *fBugs;
    float colorFade;
    // Parameters edited in the dialog box
    int dLeaders;
    int dFollowers;
    int dGeometry;
    int dSize;
    int dComplexity;
    int dSpeed;
    int dStretch;
    int dColorfadespeed;
    unsigned char dChromatek;
    unsigned char dConnections;
} PlasmaSaverSettings;

class bug
{
    public:
        int type;  // 0 = leader   1 = follower
        float h, s, l;
        float r, g, b;
        float halfr, halfg, halfb;
        float x, y, z;
        float xSpeed, ySpeed, zSpeed, maxSpeed;
        float accel;
        int right, up, forward;
        int leader;
        float craziness;  // How prone to switching direction is this leader
        float nextChange;  // Time until this leader's next direction change
    
        bug();
        virtual ~bug();
        void initLeader(FlocksSaverSettings * inSettings);
        void initFollower(FlocksSaverSettings * inSettings);
        void update(bug *bugs,FlocksSaverSettings * inSettings);
};

__private_extern__ void draw(PlasmaSaverSettings * inSettings);

__private_extern__ void cleanSettings(PlasmaSaverSettings * inSettings);
__private_extern__ void initSaver(int width,int height,PlasmaSaverSettings * inSettings);
__private_extern__ void setDefaults(PlasmaSaverSettings * inSettings);

#endif