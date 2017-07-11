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


#ifndef IMPSURFACE_H
#define IMPSURFACE_H


#ifdef WIN32
#include <windows.h>
#endif

#include <string.h>
#include <GL/gl.h>


class impSurface{
public:
    impSurface();
    ~impSurface();
    void init(int max);
    void reset();
    int addstrip(int length, float* data);
    void draw();
    void draw_wireframe();

private:
    int num_tristrips;
    int max_tristrips;
    int* tristrips;
    float** vertices;
};



#endif
