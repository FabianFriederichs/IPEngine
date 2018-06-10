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

static GLfloat SphereVerts[(PRIM_SPHERE_SEGMENTS * PRIM_SPHERE_RINGS * 2 - (PRIM_SPHERE_SEGMENTS * 2)) * 3 * 3];

//initialize gl handles
GLuint Primitives::cubevbo = 0;
GLuint Primitives::cubevao = 0;
GLuint Primitives::quadvbo = 0;
GLuint Primitives::quadvao = 0;
GLuint Primitives::spherevao = 0;
GLuint Primitives::spherevbo = 0;

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

void Primitives::drawNDCSphere()
{
	if (spherevao == 0)
	{
		//generate sphere verts
		float segstep = 2.f * glm::pi<float>() / static_cast<float>(PRIM_SPHERE_SEGMENTS); //range for theta
		float ringstep = glm::pi<float>() / static_cast<float>(PRIM_SPHERE_RINGS);		//range for phi
		size_t vertct = 0;

		float theta = 0.0f;
		float phi = 0.0f;

		for (size_t r = 0; r < PRIM_SPHERE_RINGS; ++r)
		{
			for (size_t s = 0; s < PRIM_SPHERE_SEGMENTS; ++s)
			{
				//rt:st
				glm::vec3 v1{
					glm::sin(phi) * glm::cos(theta),
					glm::cos(phi),
					glm::sin(phi) * glm::sin(theta)
				};
				//rt:st+1
				glm::vec3 v2{
					glm::sin(phi) * glm::cos(theta + segstep),
					glm::cos(phi),
					glm::sin(phi) * glm::sin(theta + segstep)
				};
				//rt+1:st
				glm::vec3 v3{
					glm::sin(phi + ringstep) * glm::cos(theta),
					glm::cos(phi + ringstep),
					glm::sin(phi + ringstep) * glm::sin(theta)
				};
				//rt+1:st+1
				glm::vec3 v4{
					glm::sin(phi + ringstep) * glm::cos(theta + segstep),
					glm::cos(phi + ringstep),
					glm::sin(phi + ringstep) * glm::sin(theta + segstep)
				};

				if (r == 0) //top cap
				{
					SphereVerts[vertct++] = v1.x; SphereVerts[vertct++] = v1.y; SphereVerts[vertct++] = v1.z;
					SphereVerts[vertct++] = v4.x; SphereVerts[vertct++] = v4.y; SphereVerts[vertct++] = v4.z;
					SphereVerts[vertct++] = v3.x; SphereVerts[vertct++] = v3.y; SphereVerts[vertct++] = v3.z;
				}
				else if (r == PRIM_SPHERE_RINGS - 1) //bottom cap
				{
					SphereVerts[vertct++] = v1.x; SphereVerts[vertct++] = v1.y; SphereVerts[vertct++] = v1.z;
					SphereVerts[vertct++] = v2.x; SphereVerts[vertct++] = v2.y; SphereVerts[vertct++] = v2.z;
					SphereVerts[vertct++] = v4.x; SphereVerts[vertct++] = v4.y; SphereVerts[vertct++] = v4.z;

					SphereVerts[vertct++] = v1.x; SphereVerts[vertct++] = v1.y; SphereVerts[vertct++] = v1.z;
					SphereVerts[vertct++] = v4.x; SphereVerts[vertct++] = v4.y; SphereVerts[vertct++] = v4.z;
					SphereVerts[vertct++] = v3.x; SphereVerts[vertct++] = v3.y; SphereVerts[vertct++] = v3.z;
				}
				else //body
				{
					SphereVerts[vertct++] = v2.x; SphereVerts[vertct++] = v2.y; SphereVerts[vertct++] = v2.z;
					SphereVerts[vertct++] = v3.x; SphereVerts[vertct++] = v3.y; SphereVerts[vertct++] = v3.z;
					SphereVerts[vertct++] = v1.x; SphereVerts[vertct++] = v1.y; SphereVerts[vertct++] = v1.z;
				}

				theta += segstep;
			}
			phi += ringstep;
		}

		glGenVertexArrays(1, &spherevao);
		if (checkglerror())
		{
			spherevao = 0;
			return;
		}
		glGenBuffers(1, &spherevbo);
		if (checkglerror())
		{
			glDeleteVertexArrays(1, &spherevao);
			spherevao = 0;
			spherevbo = 0;
			return;
		}
		glBindVertexArray(spherevao); GLERR
			glBindBuffer(GL_ARRAY_BUFFER, spherevbo); GLERR
			glBufferData(GL_ARRAY_BUFFER, sizeof(SphereVerts), SphereVerts, GL_STATIC_DRAW); GLERR
			glEnableVertexAttribArray(0); GLERR
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, reinterpret_cast<void*>(0)); GLERR
			glBindBuffer(GL_ARRAY_BUFFER, 0); GLERR
			glBindVertexArray(0); GLERR
	}

	glBindVertexArray(spherevao); GLERR
		glDrawArrays(GL_TRIANGLES, 0, (PRIM_SPHERE_SEGMENTS * PRIM_SPHERE_RINGS * 2 - (PRIM_SPHERE_SEGMENTS * 2)) * 3); GLERR
		glBindVertexArray(0); GLERR
}