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


#ifndef IMPSPHERE_H
#define IMPSPHERE_H



#include "impElement.h"



class impSphere : public impElement{
private:
	// This class stores scale information outside of impElement's matrix
	// so that there is no need for a complete matrix multiply in the value() function
	float scale;
    float invscale;

public:
    impSphere(){scale = 1.0f;};
    ~impSphere(){};
    
    void setScale(float s);
    virtual float value(float* position);
};



#endif
