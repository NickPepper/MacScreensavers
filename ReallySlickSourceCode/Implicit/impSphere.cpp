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


#include "impSphere.h"



void impSphere::setScale(float s){
    scale = s;
    invscale = 1.0f / scale;
}


float impSphere::value(float* position){
    float x = (invmat[12] + position[0]) * invscale;
    float y = (invmat[13] + position[1]) * invscale;
    float z = (invmat[14] + position[2]) * invscale;
    return(1.0f / (x*x + y*y + z*z));
}