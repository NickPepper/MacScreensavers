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


#include "impCubeVolume.h"


impCubeVolume::impCubeVolume(){
    cubes = NULL;
    corners = NULL;
    edges = NULL;
    surface = 0;
    init(4, 4, 4, 0.2f);
    surfacevalue = 0.5f;
	thecubetable = new impCubeTable();
	cubetable = thecubetable->cubetable;
	crawltable = thecubetable->crawltable;
}


impCubeVolume::~impCubeVolume(){
	delete[] cubes;
	delete[] corners;
	delete[] edges;
}


void impCubeVolume::init(int width, int height, int length, float cw){
	int i, j, k;
    
	if(cubes)
    	delete[] cubes;
    if(corners){
		/*for(i=0; i<=width; i++){
			for(j=0; j<=height; j++){
				for(k=0; k<=length; k++)
					delete[] corners[i][j][k];
				delete[] corners[i][j];
			}
			delete[] corners[i];
		}*/
		delete[] corners;
	}
	if(edges)
		delete[] edges;

    whl[0] = width;
    whl[1] = height;
    whl[2] = length;

	// calculate position of left-bottom-front corner
	cubewidth = cw;
	lbf[0] = -float(width) * cubewidth * 0.5f;
	lbf[1] = -float(height) * cubewidth * 0.5f;
	lbf[2] = -float(length) * cubewidth * 0.5f;

	// allocate cubeinfo memory and set cube positions
	cubes = new cubeinfo**[width];
	for(i=0; i<width; i++){
		cubes[i] = new cubeinfo*[height];
		for(j=0; j<height; j++){
			cubes[i][j] = new cubeinfo[length];
			for(k=0; k<length; k++){
				cubes[i][j][k].done = 0;
				cubes[i][j][k].x = lbf[0] + (cubewidth * (float(i) + 0.5f));
				cubes[i][j][k].y = lbf[1] + (cubewidth * (float(j) + 0.5f));
				cubes[i][j][k].z = lbf[2] + (cubewidth * (float(k) + 0.5f));
			}
		}
	}

	// "corners" will store the position of each corner
	// plus the value of the gradient at that location
	corners = new cornerinfo**[width+1];
	for(i=0; i<=width; i++){
		corners[i] = new cornerinfo*[height+1];
		for(j=0; j<=height; j++){
			corners[i][j] = new cornerinfo[length+1];
			for(k=0; k<=length; k++){
				corners[i][j][k].done = 0;
				corners[i][j][k].position[0] = lbf[0] + (cubewidth * float(i));
				corners[i][j][k].position[1] = lbf[1] + (cubewidth * float(j));
				corners[i][j][k].position[2] = lbf[2] + (cubewidth * float(k));
			}
		}
	}
	/*corners = new float***[width+1];
	for(i=0; i<=width; i++){
		corners[i] = new float**[height+1];
		for(j=0; j<=height; j++){
			corners[i][j] = new float*[length+1];
			for(k=0; k<=length; k++)
				corners[i][j][k] = new float[4];
		}
	}
    
	for(i=0; i<=width; i++){
		for(j=0; j<=height; j++){
			for(k=0; k<=length; k++){
				corners[i][j][k][0] = lbf[0] + (cubewidth * float(i));
				corners[i][j][k][1] = lbf[1] + (cubewidth * float(j));
				corners[i][j][k][2] = lbf[2] + (cubewidth * float(k));
			}
		}
	}*/

	edges = new edgeinfo***[3];  // a set of edges aligned with each axis
	// edges along x-axis
	edges[0] = new edgeinfo**[width];
	for(i=0; i<width; i++){
		edges[0][i] = new edgeinfo*[height+1];
		for(j=0; j<=height; j++){
			edges[0][i][j] = new edgeinfo[length+1];
			for(k=0; k<=length; k++)
				edges[0][i][j][k].done = 0;
		}
	}
	// edges along y-axis
	edges[1] = new edgeinfo**[width+1];
	for(i=0; i<=width; i++){
		edges[1][i] = new edgeinfo*[height];
		for(j=0; j<height; j++){
			edges[1][i][j] = new edgeinfo[length+1];
			for(k=0; k<=length; k++)
				edges[1][i][j][k].done = 0;
		}
	}
	// edges along z-axis
	edges[2] = new edgeinfo**[width+1];
	for(i=0; i<=width; i++){
		edges[2][i] = new edgeinfo*[height+1];
		for(j=0; j<=height; j++){
			edges[2][i][j] = new edgeinfo[length];
			for(k=0; k<length; k++)
				edges[2][i][j][k].done = 0;
		}
	}
}


void impCubeVolume::make_surface(){
	int i, j, k;

	if(surface == NULL)
		return;

	// find gradient value at every corner
	for(i=0; i<=whl[0]; i++){
		for(j=0; j<=whl[1]; j++){
			for(k=0; k<=whl[2]; k++)
				corners[i][j][k].value = function(corners[i][j][k].position);
		}
	}

	// polygonize surface
	for(i=0; i<whl[0]; i++){
		for(j=0; j<whl[1]; j++){
			for(k=0; k<whl[2]; k++){
				cubes[i][j][k].index = findcubetableindex(i, j, k);
				polygonize(i, j, k);
			}
		}
	}
 
	// zero-out edges' done labels
	for(i=0; i<whl[0]; i++){
		for(j=0; j<=whl[1]; j++){
			for(k=0; k<=whl[2]; k++){
				edges[0][i][j][k].done = 0;
			}
		}
	}
	for(i=0; i<=whl[0]; i++){
		for(j=0; j<whl[1]; j++){
			for(k=0; k<=whl[2]; k++){
				edges[1][i][j][k].done = 0;
			}
		}
	}
	for(i=0; i<=whl[0]; i++){
		for(j=0; j<=whl[1]; j++){
			for(k=0; k<whl[2]; k++){
				edges[2][i][j][k].done = 0;
			}
		}
	}
}


void impCubeVolume::make_surface(float eyex, float eyey, float eyez){
	int i, j, k;
	int index;
	cubelistelem* currentcube;
	float xdist, ydist, zdist;

	if(surface == NULL)
		return;

	// find gradient value at every corner
	for(i=0; i<=whl[0]; i++){
		for(j=0; j<=whl[1]; j++){
			for(k=0; k<=whl[2]; k++)
				corners[i][j][k].value = function(corners[i][j][k].position);
		}
	}

	// erase list from last frame
	cubelist.clear();

	// polygonize surface
	for(i=0; i<whl[0]; i++){
		for(j=0; j<whl[1]; j++){
			for(k=0; k<whl[2]; k++){
				index = findcubetableindex(i, j, k);
				if(index != 0 && index != 255){
					cubelist.push_back(cubelistelem(index));
					currentcube = &(cubelist.back());
					currentcube->position[0] = i;
					currentcube->position[1] = j;
					currentcube->position[2] = k;
					xdist = cubes[i][j][k].x - eyex;
					ydist = cubes[i][j][k].y - eyey;
					zdist = cubes[i][j][k].z - eyez;
					currentcube->depth = xdist * xdist + ydist * ydist + zdist * zdist;
				}
			}
		}
	}

	// sort the cubes
	cubelist.sort();

	// polygonize surface
	polygonize();
    
	// zero-out edges' done labels
	for(i=0; i<whl[0]; i++){
		for(j=0; j<=whl[1]; j++){
			for(k=0; k<=whl[2]; k++){
				edges[0][i][j][k].done = 0;
			}
		}
	}
	for(i=0; i<=whl[0]; i++){
		for(j=0; j<whl[1]; j++){
			for(k=0; k<=whl[2]; k++){
				edges[1][i][j][k].done = 0;
			}
		}
	}
	for(i=0; i<=whl[0]; i++){
		for(j=0; j<=whl[1]; j++){
			for(k=0; k<whl[2]; k++){
				edges[2][i][j][k].done = 0;
			}
		}
	}
}


void impCubeVolume::make_surface(std::list<impCrawlPoint> crawlpointlist){
	int i, j, k;
	bool crawlpointexit;
	int index;

	if(surface == NULL)
		return;

	// crawl from every crawl point to create the surface
	std::list<impCrawlPoint>::iterator crawliter = crawlpointlist.begin();
	while(crawliter != crawlpointlist.end()){
		// find cube corresponding to crawl point
		i = int((crawliter->position[0] - lbf[0]) / cubewidth);
		if(i < 0)
			i = 0;
		if(i >= whl[0])
			i = whl[0] - 1;
		j = int((crawliter->position[1] - lbf[1]) / cubewidth);
		if(j < 0)
			j = 0;
		if(j >= whl[1])
			j = whl[1] - 1;
		k = int((crawliter->position[2] - lbf[2]) / cubewidth);
		if(k < 0)
			k = 0;
		if(k >= whl[2])
			k = whl[2] - 1;

		// escape if starting on a finished cube
		crawlpointexit = 0;
		while(!crawlpointexit){
			if(cubes[i][j][k].done)
				crawlpointexit = 1;  // escape if starting on a finished cube
			else{  // find index for this cube
				findcornervalues(i, j, k);
				index = findcubetableindex(i, j, k);
				// save index for uncrawling
				cubes[i][j][k].index = index;
				if(index == 255)  // escape if outside surface
					crawlpointexit = 1;
				else{
					if(index == 0){  // this cube is inside volume
						cubes[i][j][k].done = 1;
						i --;  // step to an adjacent cube and start over
						if(i < 0)  // escape if you step outside of volume
							crawlpointexit = 1;
					}
					else{
						crawl_nosort(i, j, k);
						crawlpointexit = 1;
					}
				}
			}
		}

		crawliter ++;
	}

	// crawl from every crawl point to zero-out done flags
	crawliter = crawlpointlist.begin();
	while(crawliter != crawlpointlist.end()){
		// find cube corresponding to crawl point
		i = int((crawliter->position[0] - lbf[0]) / cubewidth);
		if(i < 0)
			i = 0;
		if(i >= whl[0])
			i = whl[0] - 1;
		j = int((crawliter->position[1] - lbf[1]) / cubewidth);
		if(j < 0)
			j = 0;
		if(j >= whl[1])
			j = whl[1] - 1;
		k = int((crawliter->position[2] - lbf[2]) / cubewidth);
		if(k < 0)
			k = 0;
		if(k >= whl[2])
			k = whl[2] - 1;

		// escape if starting on a finished cube
		crawlpointexit = 0;
		while(!crawlpointexit){
			if(!(cubes[i][j][k].done))
				crawlpointexit = 1;  // escape if starting on an unused cube
			else{
				index = cubes[i][j][k].index;//findcubetableindex(i, j, k);
				if(index == 0){
					cubes[i][j][k].done = 0;
					corners[i][j][k].done = 0;
					corners[i][j][k+1].done = 0;
					corners[i][j+1][k].done = 0;
					corners[i][j+1][k+1].done = 0;
					corners[i+1][j][k].done = 0;
					corners[i+1][j][k+1].done = 0;
					corners[i+1][j+1][k].done = 0;
					corners[i+1][j+1][k+1].done = 0;
					i --;  // step to an adjacent cube and start over
					if(i < 0)  // escape if you step outside of volume
						crawlpointexit = 1;
				}
				else{
					uncrawl(i, j, k);
					crawlpointexit = 1;
				}
			}
		}

		crawliter ++;
	}
}


void impCubeVolume::make_surface(float eyex, float eyey, float eyez, std::list<impCrawlPoint> crawlpointlist){
	int i, j, k;
	bool crawlpointexit;
	int index;
	float xdist, ydist, zdist;

	if(surface == NULL)
		return;

	// erase list from last fram
	cubelist.clear();

	// crawl from every crawl point to create the surface
	std::list<impCrawlPoint>::iterator crawliter = crawlpointlist.begin();
	while(crawliter != crawlpointlist.end()){
		// find cube corresponding to crawl point
		i = int((crawliter->position[0] - lbf[0]) / cubewidth);
		if(i < 0)
			i = 0;
		if(i >= whl[0])
			i = whl[0] - 1;
		j = int((crawliter->position[1] - lbf[1]) / cubewidth);
		if(j < 0)
			j = 0;
		if(j >= whl[1])
			j = whl[1] - 1;
		k = int((crawliter->position[2] - lbf[2]) / cubewidth);
		if(k < 0)
			k = 0;
		if(k >= whl[2])
			k = whl[2] - 1;

		// escape if starting on a finished cube
		crawlpointexit = 0;
		while(!crawlpointexit){
			if(cubes[i][j][k].done)
				crawlpointexit = 1;  // escape if starting on a finished cube
			else{  // find index for this cube
				findcornervalues(i, j, k);
				index = findcubetableindex(i, j, k);
				// save index for uncrawling
				cubes[i][j][k].index = index;
				if(index == 255)  // escape if outside surface
					crawlpointexit = 1;
				else{
					if(index == 0){  // this cube is inside volume
						cubes[i][j][k].done = 1;
						i --;  // step to an adjacent cube and start over
						if(i < 0)  // escape if you step outside of volume
							crawlpointexit = 1;
					}
					else{
						crawl_sort(i, j, k);
						crawlpointexit = 1;
					}
				}
			}
		}

		crawliter ++;
	}

	// sort the cubes
	std::list<cubelistelem>::iterator cubeiter = cubelist.begin();
	while(cubeiter != cubelist.end()){
		i = cubeiter->position[0];
		j = cubeiter->position[1];
		k = cubeiter->position[2];
		xdist = cubes[i][j][k].x - eyex;
		ydist = cubes[i][j][k].y - eyey;
		zdist = cubes[i][j][k].z - eyez;
		cubeiter->depth = xdist * xdist + ydist * ydist + zdist * zdist;
		cubeiter ++;
	}
	cubelist.sort();

	// polygonize surface
	polygonize();

	// crawl from every crawl point to zero-out done flags
	crawliter = crawlpointlist.begin();
	while(crawliter != crawlpointlist.end()){
		// find cube corresponding to crawl point
		i = int((crawliter->position[0] - lbf[0]) / cubewidth);
		if(i < 0)
			i = 0;
		if(i >= whl[0])
			i = whl[0] - 1;
		j = int((crawliter->position[1] - lbf[1]) / cubewidth);
		if(j < 0)
			j = 0;
		if(j >= whl[1])
			j = whl[1] - 1;
		k = int((crawliter->position[2] - lbf[2]) / cubewidth);
		if(k < 0)
			k = 0;
		if(k >= whl[2])
			k = whl[2] - 1;

		// escape if starting on a finished cube
		crawlpointexit = 0;
		while(!crawlpointexit){
			if(!(cubes[i][j][k].done))
				crawlpointexit = 1;  // escape if starting on an unused cube
			else{
				index = cubes[i][j][k].index;
				if(index == 0){
					cubes[i][j][k].done = 0;
					corners[i][j][k].done = 0;
					corners[i][j][k+1].done = 0;
					corners[i][j+1][k].done = 0;
					corners[i][j+1][k+1].done = 0;
					corners[i+1][j][k].done = 0;
					corners[i+1][j][k+1].done = 0;
					corners[i+1][j+1][k].done = 0;
					corners[i+1][j+1][k+1].done = 0;
					i --;  // step to an adjacent cube and start over
					if(i < 0)  // escape if you step outside of volume
						crawlpointexit = 1;
				}
				else{
					uncrawl(i, j, k);
					crawlpointexit = 1;
				}
			}
		}

		crawliter ++;
	}
}


// calculate index into cubetable
inline int impCubeVolume::findcubetableindex(int x, int y, int z){
	int index;

	index = 0;
	if(corners[x][y][z].value < surfacevalue)
		index |= LBF;
	if(corners[x][y][z+1].value < surfacevalue)
		index |= LBN;
	if(corners[x][y+1][z].value < surfacevalue)
		index |= LTF;
	if(corners[x][y+1][z+1].value < surfacevalue)
		index |= LTN;
	if(corners[x+1][y][z].value < surfacevalue)
		index |= RBF;
	if(corners[x+1][y][z+1].value < surfacevalue)
		index |= RBN;
	if(corners[x+1][y+1][z].value < surfacevalue)
		index |= RTF;
	if(corners[x+1][y+1][z+1].value < surfacevalue)
		index |= RTN;

	return(index);
}


inline void impCubeVolume::crawl_nosort(int x, int y, int z){
	findcornervalues(x, y, z);
	int index = findcubetableindex(x, y, z);

	// quit if this cube has been done or does not intersect surface
	if(cubes[x][y][z].done || index == 0 || index == 255)
		return;

	// save index for polygonizing and uncrawling
	cubes[x][y][z].index = index;

	// polygonize this cube if it intersects surface
	polygonize(x, y, z);

	// mark this cube as completed
	cubes[x][y][z].done = 1;

	// polygonize adjacent cubes
	if(crawltable[index][0] && x > 0)
		crawl_nosort(x-1, y, z);
	if(crawltable[index][1] && x < whl[0]-1)
		crawl_nosort(x+1, y, z);
	if(crawltable[index][2] && y > 0)
		crawl_nosort(x, y-1, z);
	if(crawltable[index][3] && y < whl[1]-1)
		crawl_nosort(x, y+1, z);
	if(crawltable[index][4] && z > 0)
		crawl_nosort(x, y, z-1);
	if(crawltable[index][5] && z < whl[2]-1)
		crawl_nosort(x, y, z+1);
}


inline void impCubeVolume::crawl_sort(int x, int y, int z){
	findcornervalues(x, y, z);
	int index = findcubetableindex(x, y, z);

	// quit if this cube has been done or does not intersect surface
	if(cubes[x][y][z].done || index == 0 || index == 255)
		return;

	// save index for uncrawling
	cubes[x][y][z].index = index;

	// mark this cube as completed
	cubes[x][y][z].done = 1;

	// add cube to list
	cubelist.push_back(cubelistelem(index));
	cubelistelem* currentcube = &(cubelist.back());
	currentcube->position[0] = x;
	currentcube->position[1] = y;
	currentcube->position[2] = z;

	// polygonize adjacent cubes
	if(crawltable[index][0] && x > 0)
		crawl_sort(x-1, y, z);
	if(crawltable[index][1] && x < whl[0]-1)
		crawl_sort(x+1, y, z);
	if(crawltable[index][2] && y > 0)
		crawl_sort(x, y-1, z);
	if(crawltable[index][3] && y < whl[1]-1)
		crawl_sort(x, y+1, z);
	if(crawltable[index][4] && z > 0)
		crawl_sort(x, y, z-1);
	if(crawltable[index][5] && z < whl[2]-1)
		crawl_sort(x, y, z+1);
}


inline void impCubeVolume::uncrawl(int x, int y, int z){
	if(!(cubes[x][y][z].done))
		return;

	corners[x][y][z].done = 0;
	corners[x][y][z+1].done = 0;
	corners[x][y+1][z].done = 0;
	corners[x][y+1][z+1].done = 0;
	corners[x+1][y][z].done = 0;
	corners[x+1][y][z+1].done = 0;
	corners[x+1][y+1][z].done = 0;
	corners[x+1][y+1][z+1].done = 0;

	cubes[x][y][z].done = 0;

	edges[0][x][y][z].done = 0;
	edges[0][x][y+1][z].done = 0;
	edges[0][x][y][z+1].done = 0;
	edges[0][x][y+1][z+1].done = 0;
	edges[1][x][y][z].done = 0;
	edges[1][x+1][y][z].done = 0;
	edges[1][x][y][z+1].done = 0;
	edges[1][x+1][y][z+1].done = 0;
	edges[2][x][y][z].done = 0;
	edges[2][x+1][y][z].done = 0;
	edges[2][x][y+1][z].done = 0;
	edges[2][x+1][y+1][z].done = 0;

	// uncrawl adjacent cubes
	int index = cubes[x][y][z].index;
	if(crawltable[index][0] && x > 0)
		uncrawl(x-1, y, z);
	if(crawltable[index][1] && x < whl[0]-1)
		uncrawl(x+1, y, z);
	if(crawltable[index][2] && y > 0)
		uncrawl(x, y-1, z);
	if(crawltable[index][3] && y < whl[1]-1)
		uncrawl(x, y+1, z);
	if(crawltable[index][4] && z > 0)
		uncrawl(x, y, z-1);
	if(crawltable[index][5] && z < whl[2]-1)
		uncrawl(x, y, z+1);
}


// polygonize an individual cube
inline void impCubeVolume::polygonize(int x, int y, int z){
	// stores data for computed triangle strips
	static int i;
	static float data[42];
	static int index, counter;
	static int nedges;
	static int copysize = 6 * sizeof(float);

	// find index into cubetable
	index = cubes[x][y][z].index;

	counter = 0;
	nedges = cubetable[index][counter];
	while(nedges != 0){
		for(i=0; i<nedges; i++){
			// generate vertex position and normal data
			switch(cubetable[index][i+counter+1]){
			case 0:
				findvert(2, x, y, z);
				memcpy(&data[i*6], edges[2][x][y][z].data, copysize);
				break;
			case 1:
				findvert(1, x, y, z);
				memcpy(&data[i*6], edges[1][x][y][z].data, copysize);
				break;
			case 2:
				findvert(1, x, y, z+1);
				memcpy(&data[i*6], edges[1][x][y][z+1].data, copysize);
				break;
			case 3:
				findvert(2, x, y+1, z);
				memcpy(&data[i*6], edges[2][x][y+1][z].data, copysize);
				break;
			case 4:
				findvert(0, x, y, z);
				memcpy(&data[i*6], edges[0][x][y][z].data, copysize);
				break;
			case 5:
				findvert(0, x, y, z+1);
				memcpy(&data[i*6], edges[0][x][y][z+1].data, copysize);
				break;
			case 6:
				findvert(0, x, y+1, z);
				memcpy(&data[i*6], edges[0][x][y+1][z].data, copysize);
				break;
			case 7:
				findvert(0, x, y+1, z+1);
				memcpy(&data[i*6], edges[0][x][y+1][z+1].data, copysize);
				break;
			case 8:
				findvert(2, x+1, y, z);
				memcpy(&data[i*6], edges[2][x+1][y][z].data, copysize);
				break;
			case 9:
				findvert(1, x+1, y, z);
				memcpy(&data[i*6], edges[1][x+1][y][z].data, copysize);
				break;
			case 10:
				findvert(1, x+1, y, z+1);
				memcpy(&data[i*6], edges[1][x+1][y][z+1].data, copysize);
				break;
			case 11:
				findvert(2, x+1, y+1, z);
				memcpy(&data[i*6], edges[2][x+1][y+1][z].data, copysize);
			}
		}
		surface->addstrip(nedges, data);
		counter += (nedges + 1);
		nedges = cubetable[index][counter];
	}
}


// polygonize a list of cubes stored in "cubelist"
inline void impCubeVolume::polygonize(){
	// stores data for computed triangle strips
	static int x, y, z;
	static int i;
	static float data[42];
	static int index, counter;
	static int nedges;
	static int copysize = 6 * sizeof(float);

	// polygonize in reverse order so that the farthest cube
	// is drawn first and the closest is drawn last
	std::list<cubelistelem>::iterator iter = cubelist.end();
	while(iter != cubelist.begin()){
		iter --;
	
		x = iter->position[0];
		y = iter->position[1];
		z = iter->position[2];
    
		// find index into cubetable
		index = iter->index;

		counter = 0;
		nedges = cubetable[index][counter];
		while(nedges != 0){
			for(i=0; i<nedges; i++){
				// generate vertex position and normal data
				switch(cubetable[index][i+counter+1]){
				case 0:
					findvert(2, x, y, z);
					memcpy(&data[i*6], edges[2][x][y][z].data, copysize);
					break;
				case 1:
					findvert(1, x, y, z);
					memcpy(&data[i*6], edges[1][x][y][z].data, copysize);
					break;
				case 2:
					findvert(1, x, y, z+1);
					memcpy(&data[i*6], edges[1][x][y][z+1].data, copysize);
					break;
				case 3:
					findvert(2, x, y+1, z);
					memcpy(&data[i*6], edges[2][x][y+1][z].data, copysize);
					break;
				case 4:
					findvert(0, x, y, z);
					memcpy(&data[i*6], edges[0][x][y][z].data, copysize);
					break;
				case 5:
					findvert(0, x, y, z+1);
					memcpy(&data[i*6], edges[0][x][y][z+1].data, copysize);
					break;
				case 6:
					findvert(0, x, y+1, z);
					memcpy(&data[i*6], edges[0][x][y+1][z].data, copysize);
					break;
				case 7:
					findvert(0, x, y+1, z+1);
					memcpy(&data[i*6], edges[0][x][y+1][z+1].data, copysize);
					break;
				case 8:
					findvert(2, x+1, y, z);
					memcpy(&data[i*6], edges[2][x+1][y][z].data, copysize);
					break;
				case 9:
					findvert(1, x+1, y, z);
					memcpy(&data[i*6], edges[1][x+1][y][z].data, copysize);
					break;
				case 10:
					findvert(1, x+1, y, z+1);
					memcpy(&data[i*6], edges[1][x+1][y][z+1].data, copysize);
					break;
				case 11:
					findvert(2, x+1, y+1, z);
					memcpy(&data[i*6], edges[2][x+1][y+1][z].data, copysize);
				}
			}
		    surface->addstrip(nedges, data);
		    counter += (nedges + 1);
		    nedges = cubetable[index][counter];
		}
	}
}


// find value at all corners of this cube
inline void impCubeVolume::findcornervalues(int x, int y, int z){
	if(!corners[x][y][z].done){
		corners[x][y][z].value = function(corners[x][y][z].position);
		corners[x][y][z].done = 1;
	}
	z ++;
	if(!corners[x][y][z].done){
		corners[x][y][z].value = function(corners[x][y][z].position);
		corners[x][y][z].done = 1;
	}
	y ++;
	if(!corners[x][y][z].done){
		corners[x][y][z].value = function(corners[x][y][z].position);
		corners[x][y][z].done = 1;
	}
	z --;
	if(!corners[x][y][z].done){
		corners[x][y][z].value = function(corners[x][y][z].position);
		corners[x][y][z].done = 1;
	}
	x ++;
	if(!corners[x][y][z].done){
		corners[x][y][z].value = function(corners[x][y][z].position);
		corners[x][y][z].done = 1;
	}
	z ++;
	if(!corners[x][y][z].done){
		corners[x][y][z].value = function(corners[x][y][z].position);
		corners[x][y][z].done = 1;
	}
	y --;
	if(!corners[x][y][z].done){
		corners[x][y][z].value = function(corners[x][y][z].position);
		corners[x][y][z].done = 1;
	}
	z --;
	if(!corners[x][y][z].done){
		corners[x][y][z].value = function(corners[x][y][z].position);
		corners[x][y][z].done = 1;
	}
}


inline void impCubeVolume::findvert(int axis, int x, int y, int z){
	// values used to determine the normal vector
	static float no, nx, ny, nz;
	static float pos[3];
	static int copysize = 3 * sizeof(float);
    
	if(edges[axis][x][y][z].done)
		return;
	edges[axis][x][y][z].done = 1;

	// find position of vertex along this edge
	switch(axis){
		case 0:  // x-axis
			edges[axis][x][y][z].data[3] = corners[x][y][z].position[0]
				+ (cubewidth * ((surfacevalue - corners[x][y][z].value)
				/ (corners[x+1][y][z].value - corners[x][y][z].value)));
			edges[axis][x][y][z].data[4] = corners[x][y][z].position[1];
			edges[axis][x][y][z].data[5] = corners[x][y][z].position[2];
			break;
		case 1:  // y-axis
			edges[axis][x][y][z].data[3] = corners[x][y][z].position[0];
			edges[axis][x][y][z].data[4] = corners[x][y][z].position[1]
				+ (cubewidth * ((surfacevalue - corners[x][y][z].value)
				/ (corners[x][y+1][z].value - corners[x][y][z].value)));
			edges[axis][x][y][z].data[5] = corners[x][y][z].position[2];
			break;
		case 2:  // z-axis
			edges[axis][x][y][z].data[3] = corners[x][y][z].position[0];
			edges[axis][x][y][z].data[4] = corners[x][y][z].position[1];
			edges[axis][x][y][z].data[5] = corners[x][y][z].position[2]
				+ (cubewidth * ((surfacevalue - corners[x][y][z].value)
				/ (corners[x][y][z+1].value - corners[x][y][z].value)));
	}

	// find normal vector at vertex along this edge
	// first find normal vector origin value
	memcpy(pos, &(edges[axis][x][y][z].data[3]), copysize);
	no = function(pos);
	// then find values at slight displacements and subtract
	pos[0] -= 0.01f;
	nx = function(pos) - no;
	pos[0] += 0.01f;
	pos[1] -= 0.01f;
	ny = function(pos) - no;
	pos[1] += 0.01f;
	pos[2] -= 0.01f;
	nz = function(pos) - no;
	// then normalize
	no = 1.0f / sqrtf(nx * nx + ny * ny + nz * nz);
	edges[axis][x][y][z].data[0] = nx * no;
	edges[axis][x][y][z].data[1] = ny * no;
	edges[axis][x][y][z].data[2] = nz * no;
}
