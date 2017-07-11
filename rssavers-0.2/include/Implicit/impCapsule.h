/*
 * Copyright (C) 2010  Terence M. Welsh
 *
 * This file is part of Implicit.
 *
 * Implicit is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Implicit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#ifndef IMPCAPSULE_H
#define IMPCAPSULE_H


#include <Implicit/impShape.h>


// An impCapsule is defined as the inverse square falloff from a segment.
class impCapsule : public impShape{
	float length;  // dimension on z-axis

public:
	impCapsule(){
		length = 1.0f;
	};
	~impCapsule(){};
	void setLength(float l){ length = l; }
	virtual float value(float* position);
};


#endif
