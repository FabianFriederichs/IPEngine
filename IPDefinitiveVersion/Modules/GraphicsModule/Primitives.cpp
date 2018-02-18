#include "Primitives.h"

static const GLfloat CubeVerts[] = {
	// Positions          
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};

static const GLfloat QuadVerts[] = {   
									   
	-1.0f, 1.0f, 0.0f, 
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f, 

	-1.0f, 1.0f, 0.0f ,
	1.0f, -1.0f, 0.0f ,
	1.0f, 1.0f,0.0f
};

//initialize gl handles
GLuint Primitives::cubevbo = 0;
GLuint Primitives::cubevao = 0;
GLuint Primitives::quadvbo = 0;
GLuint Primitives::quadvao = 0;

void Primitives::drawNDCCube()
{
	if (cubevao == 0)
	{
		glGenVertexArrays(1, &cubevao);
		if (checkglerror())
		{
			cubevao = 0;
			return;
		}
		glGenBuffers(1, &cubevbo);
		if (checkglerror())
		{
			glDeleteVertexArrays(1, &cubevao);
			cubevao = 0;
			cubevbo = 0;
			return;
		}
		glBindVertexArray(cubevao); GLERR
		glBindBuffer(GL_ARRAY_BUFFER, cubevbo); GLERR
		glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVerts), CubeVerts, GL_STATIC_DRAW); GLERR
		glEnableVertexAttribArray(0); GLERR
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, reinterpret_cast<void*>(0)); GLERR
		glBindBuffer(GL_ARRAY_BUFFER, 0); GLERR
		glBindVertexArray(0); GLERR
	}

	glBindVertexArray(cubevao); GLERR
	glDrawArrays(GL_TRIANGLES, 0, 36); GLERR
	glBindVertexArray(0); GLERR
}

void Primitives::drawNDCQuad()
{
	if (quadvao == 0)
	{
		glGenVertexArrays(1, &quadvao);
		if (checkglerror())
		{
			quadvao = 0;
			return;
		}
		glGenBuffers(1, &quadvbo);
		if (checkglerror())
		{
			glDeleteVertexArrays(1, &quadvao);
			quadvao = 0;
			quadvbo = 0;
			return;
		}
		glBindVertexArray(quadvao);
		glBindBuffer(GL_ARRAY_BUFFER, quadvbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVerts), QuadVerts, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, reinterpret_cast<void*>(0));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glBindVertexArray(quadvao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}