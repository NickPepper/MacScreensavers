#include <GL/glut.h>
#include "../rsMath/rsMath.h"
#include "impCubeVolume.h"

#include "impCrawlPoint.h"
#include "impSphere.h"
#include "impEllipsoid.h"
#include "impTorus.h"
#include "impHexahedron.h"

#include "texture.h"



impCubeVolume* volume;
impSurface* surface;


impSphere sphere;
impEllipsoid ellipsoid;
impTorus torus1, torus2;
impHexahedron hexa;




void reshape(int width, int height){
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, float(width)/float(height), 0.1f, 100.0f);
}


void initOpengl(){
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float mat_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float mat_none[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_none);
    glMaterialf(GL_FRONT, GL_SHININESS, 40.0f);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    float light0_ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    float light0_diffuse[] = {0.5f, 0.05f, 0.15f, 1.0f};
    float light0_specular[] = {0.3f, 0.3f, 0.3f, 1.0f};
    float light0_direction[] = {8.0f, 8.0f, 12.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, mat_none);

	glBindTexture(GL_TEXTURE_2D, 1);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, TEXSIZE, TEXSIZE, GL_RGBA, 
		GL_UNSIGNED_BYTE, spheremap);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_2D);
}


void display(){
	static float matrix[16];
	static float center[3];
	static float move[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	static rsMatrix mat1, mat2;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -6.0f);

    move[0] += 0.005;
    move[1] += 0.007;
    move[2] += 0.01;
    move[3] += 0.013;
    move[4] += 0.023;
    move[5] += 0.031;
    move[6] += 0.037;
    move[7] += 0.041;

	sphere.setPosition(cosf(move[4]), sinf(move[5]), cosf(move[6]));
	sphere.setScale(0.3 + 0.15 * cosf(move[7]));

	mat1.makeScale(0.25f + 0.12f * cosf(move[7] * 4.0f),
		0.25f + 0.12f * cosf(move[7] * 4.0f),
		0.3f + 0.15f * cosf(move[7] * 4.0f + 1.5707f));
	mat2.makeRot(move[2], 1.0f, 0.0f, 0.0f);
	mat1.postMult(mat2);
	mat2.makeRot(move[4], 0.0f, 1.0f, 0.0f);
	mat1.postMult(mat2);
	mat2.makeTrans(cosf(move[5]), cosf(move[6]), sinf(move[3]));
	mat1.postMult(mat2);
	mat1.get(matrix);
	ellipsoid.setMatrix(matrix);

	mat1.makeRot(move[0], 1.0f, 0.0f, 0.0f);
	mat2.makeRot(move[1], 0.0f, 1.0f, 0.0f);
	mat1.preMult(mat2);
	mat1.get(matrix);
	torus1.setMatrix(matrix);
	torus1.radius = 1.1f;
	torus1.scale = 0.17f;

	mat1.makeRot(move[2], 0.0f, 1.0f, 0.0f);
	mat2.makeRot(move[3], 1.0f, 0.0f, 0.0f);
	mat1.postMult(mat2);
	mat1.get(matrix);
	torus2.setMatrix(matrix);
	torus2.radius = 1.5f;
	torus2.scale = 0.17f;

	mat1.makeScale(0.25f, 0.25f, 0.25f + 0.15 * cosf(move[0]));
	mat2.makeRot(move[3], 1.0f, 0.0f, 0.0f);
	mat1.postMult(mat2);
	mat2.makeRot(move[5], 0.0f, 1.0f, 0.0f);
	mat1.postMult(mat2);
	mat2.makeTrans(sinf(move[2]), cosf(move[5]), cosf(move[6]));
	mat1.postMult(mat2);
	mat1.get(matrix);
	hexa.setMatrix(matrix);

	std::list<impCrawlPoint> crawlpointlist;
	//sphere.center(center);
	//crawlpointlist.push_back(impCrawlPoint(center));
	ellipsoid.center(center);
	crawlpointlist.push_back(impCrawlPoint(center));
	torus1.center(center);
	crawlpointlist.push_back(impCrawlPoint(center));
	torus2.center(center);
	crawlpointlist.push_back(impCrawlPoint(center));
	hexa.center(center);
	crawlpointlist.push_back(impCrawlPoint(center));

	int time1, time2;
	static int totalTime = 0;
	static int numTimes = 0;

    glDisable(GL_BLEND);
    surface->reset();
    volume->surfacevalue = 0.5f;
	//time1 = timeGetTime();
	//volume->make_surface();
	//volume->make_surface(0,0,0);
	volume->make_surface(crawlpointlist);
	//volume->make_surface(0,0,0,crawlpointlist);
	//time2 = timeGetTime();

	/*if(time2 - time1 >= 0){
		totalTime += time2 - time1;
		numTimes ++;
	}
	cout << float(totalTime) / float(numTimes) << endl;*/

    surface->draw();

    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();
}


float function(float* position){
    return(//sphere.value(position)
		+ ellipsoid.value(position)
		+ torus1.value(position)
		+ torus2.value(position)
		+ hexa.value(position)
	);
}


void main(int argc,  char **argv){
    glutInit(&argc, argv);
	glutInitWindowSize(400, 400);
    glutInitWindowPosition(100,  100);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    int windowID = glutCreateWindow("Implicit surface test");

    glutReshapeFunc(reshape);

    glutSetWindow(windowID);
    glutDisplayFunc(display);

    initOpengl();

    volume = new impCubeVolume;
    volume->init(32, 32, 32, 0.12f);
	//volume->init(50, 50, 50, 0.08f);
    volume->function = function;
    surface = new impSurface;
    surface->init(8000);
    volume->surface = surface;

    glutMainLoop();
    
    glutDestroyWindow(windowID);
}
