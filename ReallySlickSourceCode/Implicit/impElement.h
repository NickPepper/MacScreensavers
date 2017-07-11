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


#ifndef IMPELEMENT_H
#define IMPELEMENT_H


// This is a generic class from which specific elements are
// derived.  An element would be a sphere or torus or anything
// that defines an implicit surface.


#include <math.h>


class impElement{
public:
	float mat[16];
    float invmat[16];

    impElement();
    ~impElement(){};
    void setPosition(float x, float y, float z);
    void setPosition(float* position);
    void setMatrix(float* m);
	void invertMatrix();
    // returns the value of this element at a given position
    // "position" is an array of 3 floats
    virtual float value(float* position);
    // assigns a center of the element's volume to "position"
    virtual void center(float* position);
};




#endif
