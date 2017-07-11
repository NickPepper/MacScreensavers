/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * Implicit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * Implicit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef IMPCUBEVOLUME_H
#define IMPCUBEVOLUME_H



#include "impSurface.h"
#include "impCubeTable.h"
#include "impCrawlPoint.h"
#include <list>

#include <math.h>
#include <fstream.h>



struct cubeinfo{
	int index;
    bool done;  // has this cube been checked yet?
    int edgebits;  // index into cubetable
    float x;
    float y;
    float z;
};

struct cornerinfo{
    bool done;  // has this corner had its value calculated
    float value;  // field value at this corner
    float position[3];
};

struct edgeinfo{
    bool done;  // has this edge been calculated
    float data[6];  // normal vector and position
};

// For making a list of cubes to be polygonized.
// The list can be sorted by depth before polygonization in
// the case of transparent surfaces.
class cubelistelem{
public:
    int position[3];
    int index;
    float depth;  // distance squared from eyepoint
    
    cubelistelem(){};
    cubelistelem(int ind){index = ind;};
    ~cubelistelem(){};
    
    friend bool operator < (const cubelistelem& a, const cubelistelem& b){return((a.depth)<(b.depth));}
    friend bool operator > (const cubelistelem& a, const cubelistelem& b){return((a.depth)>(b.depth));}
    friend bool operator == (const cubelistelem& a, const cubelistelem& b){return((a.depth)==(b.depth));}
    friend bool operator != (const cubelistelem& a, const cubelistelem& b){return((a.depth)!=(b.depth));}
};


class impCubeVolume{
public:
    float lbf[3];  // left-bottom-far corner of volume
    float cubewidth;
    int whl[3];  // width, height, and length in cubes
    float surfacevalue;  // surface's position on gradient
    cubeinfo*** cubes;
	cornerinfo*** corners;  // position and value at each corner
    edgeinfo**** edges;
    std::list<cubelistelem> cubelist;
    float (*function)(float* position);
    impSurface* surface;

    impCubeVolume();
    ~impCubeVolume();
    // pass dimensions of volume in cubes plus "cubewidth"
    void init(int width, int height, int length, float cw);
    // These routines compute geometry and store it in "surface"
	// Providing an eyepoint indicates that you want to sort the surface
	// so that transparent surfaces will be drawn back-to-front.
	// Providing a list of crawlpoints indicates that you want to use a 
	// surface crawling algorithm.
	// If no crawlpoint list is provided, then every cube is checked, which
	// is slow but thorough (there's no chance of missing a piece of the
	// surface).
    void make_surface();
    void make_surface(float eyex, float eyey, float eyez);
    void make_surface(std::list<impCrawlPoint> crawlpointlist);
    void make_surface(float eyex, float eyey, float eyez, std::list<impCrawlPoint> crawlpointlist);

private:
	impCubeTable* thecubetable;
	int** cubetable;
	bool** crawltable;
    // x, y, and z define position of cube in this volume
    inline int findcubetableindex(int x, int y, int z);
	inline void crawl_nosort(int x, int y, int z);
	inline void crawl_sort(int x, int y, int z);
	inline void uncrawl(int x, int y, int z);
    inline void polygonize(int x, int y, int z);
    inline void polygonize();
	inline void findcornervalues(int x, int y, int z);
    inline void findvert(int axis, int x, int y, int z);
};



#endif
