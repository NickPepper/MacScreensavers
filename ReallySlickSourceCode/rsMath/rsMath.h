/*
 * Copyright (C) 2002  Terence M. Welsh
 *
 * rsMath is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
 *
 * rsMath is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef RSMATH_H
#define RSMATH_H



#include <math.h>
#include "rsDefines.h"
#include "rsRand.h"



class rsVec;
class rsMatrix;
class rsQuat;




class rsVec{
public:
	float v[3];

	rsVec();
	rsVec(float xx, float yy, float zz);
	virtual ~rsVec();
	void set(float xx, float yy, float zz);
	float length();
	float normalize();
	float dot(rsVec);
	void cross(rsVec, rsVec);
	void scale(float);
	void transPoint(const rsMatrix &m);
	void transVec(const rsMatrix &m);
	int almostEqual(rsVec vec, float tolerance);

	float & operator [] (int i) {return v[i];}
	const float & operator [] (int i) const {return v[i];}
	rsVec & operator = (const rsVec &vec)
		{v[0]=vec[0];v[1]=vec[1];v[2]=vec[2];return *this;};
	rsVec operator + (const rsVec &vec)
		{return(rsVec(v[0]+vec[0], v[1]+vec[1], v[2]+vec[2]));};
	rsVec operator - (const rsVec &vec)
		{return(rsVec(v[0]-vec[0], v[1]-vec[1], v[2]-vec[2]));};
	rsVec operator * (const float &mul)
		{return(rsVec(v[0]*mul, v[1]*mul, v[2]*mul));};
	rsVec operator / (const float &div)
		{float rec = 1.0f/div; return(rsVec(v[0]*rec, v[1]*rec, v[2]*rec));};
	rsVec & operator += (const rsVec &vec)
		{v[0]+=vec[0];v[1]+=vec[1];v[2]+=vec[2];return *this;};
	rsVec & operator -= (const rsVec &vec)
		{v[0]-=vec[0];v[1]-=vec[1];v[2]-=vec[2];return *this;};
	rsVec & operator *= (const rsVec &vec)
		{v[0]*=vec[0];v[1]*=vec[1];v[2]*=vec[2];return *this;};
	rsVec & operator *= (const float &mul)
		{v[0]*=mul;v[1]*=mul;v[2]*=mul;return *this;};
};


// Generic vector math functions
// Mostly outdated stuff
// (Sheesh... I'm too young to have legacy code)
__private_extern__ float rsLength(float *);
__private_extern__ float rsNormalize(float *);
__private_extern__ float rsDot(float *, float *);
__private_extern__ void rsCross(float *, float *, float *);
__private_extern__ void rsScaleVec(float *, float);



class rsMatrix{
public:
	float m[16];
	// 1 0 0 x   0 4 8  12  <-- Column order matrix just like OpenGL
	// 0 1 0 y   1 5 9  13
	// 0 0 1 z   2 6 10 14
	// 0 0 0 1   3 7 11 15

	rsMatrix();
	~rsMatrix();
	void identity();
	void set(float* mat);
	void get(float* mat);
	void copy(const rsMatrix &mat);
	void preMult(const rsMatrix &postMat);
	void postMult(const rsMatrix &preMat);
	void makeTrans(float x, float y, float z);
	void makeTrans(float* p);
	void makeTrans(const rsVec &vec);
	void makeScale(float s);
	void makeScale(float x, float y, float z);
	void makeScale(float* s);
	void makeScale(const rsVec &vec);
	void makeRot(float a, float x, float y, float z);	// angle, axis
	void makeRot(float a, const rsVec &v);	// angle, axis
	void makeRot(rsQuat &q);
	float determinant();
	int invert(const rsMatrix &mat);  // general matrix inversion
	void rotationInvert(const rsMatrix &mat);  // rotation matrix inversion
	void fromQuat(const rsQuat &q);

	const float & operator [] (int i) const {return m[i];}
	rsMatrix & operator = (const rsMatrix &mat);
};



class rsQuat{
public:
	float q[4];

	rsQuat();
	rsQuat(float x, float y, float z, float w);
	~rsQuat();
	void set(float x, float y, float z, float w);	// x, y, z, w
	void copy(rsQuat);						// Copy another quaternion
	void make(float a, float x, float y, float z);	// angle, axis
	void make(float a, const rsVec &v);
	void normalize();
	void preMult(rsQuat &postQuat);			// Multiply this quaternion by
											// the passed quaternion
											// (this * passed)
	void postMult(rsQuat &preQuat);			// Multiply the passed quaternion
											// by this quaternion
											// (passed * this)
	void toMat(float *);					// Convert quaternion to array of 16 floats
	void fromMat(float *);					// Convert array of 16 floats to quaternion
	void fromEuler(float, float, float);	// Convert from hpr angles
	void slerp(rsQuat, rsQuat, float);		// Interpolate first quaternion
											// to second using float from 0.0
											// to 1.0

	float & operator [] (int i) {return q[i];}
	const float & operator [] (int i) const {return q[i];}
};




#endif  // RSMATH_H