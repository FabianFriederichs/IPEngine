#include "Scene.h"

Scene::Scene(int width, int height) :
	m_cloth(50, 50, glm::vec3(0.0f, 0.0f, 0.0f), 0.3f),
	windowWidth(width),
	windowHeight(height)
{

}

void Scene::init()
{
	m_shader = GLUtils::createShaderProgram("assets/shaders/flagshader.vs", "assets/shaders/flagshader.fs");
	worldmat = glm::mat4(1.0f);
	lightdir = glm::vec3(-1.0f, -1.0f, -1.0f);
	lightcolor = glm::vec3(1.0f, 1.0f, 1.0f);
	projmat = glm::perspective(glm::radians(90.0f), static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);
	/*viewmat = glm::mat4(
		glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
		glm::vec4(0.0f, -0.3f, -3.0f, 1.0f)
	);*/

	viewmat = glm::lookAt(glm::vec3(0.0f, -10.0f, 20.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	//Wir aktivieren Backface Culling
	glDisable(GL_CULL_FACE);

	//Und den Tiefentest
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	m_shader->use();

	GLint location;

	location = glGetUniformLocation(m_shader->prog, "world_mat"); GLERR
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(worldmat)); GLERR																		
	location = glGetUniformLocation(m_shader->prog, "view_mat"); GLERR
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(viewmat)); GLERR
	location = glGetUniformLocation(m_shader->prog, "proj_mat"); GLERR
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(projmat)); GLERR



	//Uniforms für den Fragment Shader

	//Lichtrichtung
	location = glGetUniformLocation(m_shader->prog, "lightDir"); GLERR
	//Lichtrichtung von Worldspace in Viewspace transformieren
	glm::vec4 lightDirInViewSpace(glm::normalize(viewmat * glm::vec4(lightdir, 0.0f)));
	//Vec3 daraus konstruieren und in die Uniform Variable schreiben
	glUniform3fv(location, 1, glm::value_ptr(glm::vec3(lightDirInViewSpace.x, lightDirInViewSpace.y, lightDirInViewSpace.z)));

	//Lichtfarbe
	location = glGetUniformLocation(m_shader->prog, "light_color"); GLERR
	glUniform3fv(location, 1, glm::value_ptr(lightcolor)); GLERR


	/*texture->bind(0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); GLERR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); GLERR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); GLERR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); GLERR
	GLfloat aniso;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	location = glGetUniformLocation(m_shader->prog, "tex"); GLERR
	glUniform1i(location, 0); GLERR*/


	//PhysicsContext
	phctx.gravity = glm::vec3(0.0f, -9.81f, 0.0f);

	//Colliders
	SphereCollider ball(glm::vec3(0.0f, -6.0f, 0.0f), 5.0f, true);
	phctx.globalColliders.push_back(ball);
	phctx.particleMass = 0.1f;
	phctx.springKd = 0.12f;
	phctx.springKs = 40.0f;
	phctx.airfric = 0.01f;

	m_cloth.create(phctx);
	/*m_cloth.fixParticle(0, 0, true);
	m_cloth.fixParticle(0, m_cloth.height() - 1, true);
	m_cloth.fixParticle(m_cloth.width() - 1, 0, true);
	m_cloth.fixParticle(m_cloth.width() - 1, m_cloth.height() - 1, true);*/
	//render settings
	renderWireframe = false;
}

void Scene::update(double dt)
{
	m_cloth.update(dt, phctx);
}

void Scene::render(double dt)
{
	//Colorbuffer und Depthbuffer des Backbuffers clearen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GLERR

	//Unsere World Matrix verändert sich. Also müssen wir sie bei jedem Frame updaten.
	GLint location;
	location = glGetUniformLocation(m_shader->prog, "world_mat"); GLERR
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(worldmat)); GLERR

	//update camera
	location = glGetUniformLocation(m_shader->prog, "view_mat"); GLERR
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(viewmat)); GLERR

	m_cloth.render(dt, renderWireframe);
}
