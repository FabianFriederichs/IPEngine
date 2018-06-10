#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_
#include "libheaders.h"
#include "glerror.h"

#define PRIM_SPHERE_SEGMENTS 16
#define PRIM_SPHERE_RINGS 16

class Primitives
{
public:
	static void drawNDCCube();
	static void drawNDCQuad();
	static void drawNDCSphere();

private:
	static GLuint cubevbo;
	static GLuint cubevao;
	static GLuint quadvbo;
	static GLuint quadvao;
	static GLuint spherevbo;
	static GLuint spherevao;
};
#endif