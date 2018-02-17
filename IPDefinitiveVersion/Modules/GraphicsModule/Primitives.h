#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_
#include "libheaders.h"
#include "glerror.h"

class Primitives
{
public:
	static void drawNDCCube();
	static void drawNDCQuad();

private:
	static GLuint cubevbo;
	static GLuint cubevao;
	static GLuint quadvbo;
	static GLuint quadvao;
};
#endif