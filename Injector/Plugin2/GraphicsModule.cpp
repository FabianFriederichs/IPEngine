// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "GraphicsModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
GraphicsModule::GraphicsModule()
{
	DataDepName = "Data";
	m_info.identifier = "GraphicsModule";
	m_info.version = "1.0";
	m_info.iam = "IGraphics_API";
	return;
}

bool GraphicsModule::startUp()
{
	m_scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(m_scmID);
	setupSDL();
	loadShaders();
	return true;
}

void GraphicsModule::loadShaders()
{

}

void GraphicsModule::render()
{
	//stuff
	//Clear buffer
	glClearColor(m_clearcolor.r, m_clearcolor.g, m_clearcolor.b, m_clearcolor.a); GLERR
	glEnable(GL_CULL_FACE); GLERR
	glFrontFace(GL_CCW); GLERR
	glCullFace(GL_BACK); GLERR
	glEnable(GL_DEPTH_TEST); GLERR
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GLERR

	//go through meshes
	auto& activeentitynames = getActiveEntityNames(*m_scm);
	auto& entities = m_scm->getEntities();
	for (auto eid : activeentitynames)
	{
		auto mO = dynamic_cast<SCM::ThreeDimEntity*>(&entities[eid]);

		if (mO)
		{
			for (auto mesh : mO->m_mesheObjects->m_meshes)
			{
				//activate shader
				auto shader = m_scmshadertovao[mesh->m_material->m_shaderid];
				shader->use();

				//set uniforms/light/transform/view/proj/camera pos
				shader->setUniform("model", mO->m_transformData.getData()->m_transformMatrix, false);
				glm::vec3 camerapos(0,0,0); 
				GLfloat m_fov(glm::pi<float>() / 2);

				glm::mat4 projmat = glm::perspective(m_fov, width / height, znear, zfar);
				glm::mat4 viewmat = glm::mat4(glm::quat(.0f, .0f, .0f, 1.0f))*translate(glm::mat4(1.0f), -camerapos);;
				shader->setUniform("view", viewmat, false);
				
				shader->setUniform("projection", projmat, false);
				
				shader->setUniform("camerapos", camerapos);

				//set material uniforms
				//TODO

				//draw mesh
				drawSCMMesh(mesh->m_meshId);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); GLERR //QUESTION is this necessary?
			}
		}
	}
	


}

std::vector<std::string> GraphicsModule::getActiveEntityNames(SCM::ISimpleContentModule_API & scm)
{
	auto& entities = scm.getEntities();
	std::vector<std::string> names;
	for (auto e : entities)
	{
		if (e.second.isActive)
		{
			names.push_back(e.second.m_name);
		}
	}
	return names;
}

void GraphicsModule::setupSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		//std::cout << "Could not initialize SDL." << std::endl;
	}
	
	window = SDL_CreateWindow("Demo Window", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	
	if (window == NULL) {
		//std::cout << "Could not create SDL window." << std::endl;
		/*printDebug("Could not create SDL window.\n");
		return 1;*/
	}
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	//SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//SDL_GL_CreateContext(window);
	////check null

	//glewExperimental = GL_TRUE;
	//glewInit();
	//bla
}

void GraphicsModule::updateData()
{
	auto& activeentitynames= getActiveEntityNames(*m_scm);
	auto& entities = m_scm->getEntities();
	auto& shaders = m_scm->getShaders();
	for (auto eid : activeentitynames)
	{
		auto mO = dynamic_cast<SCM::ThreeDimEntity*>(&entities[eid]);

		if (mO)
		{
			for (auto mesh : mO->m_mesheObjects->m_meshes)
			{
				if (m_scmmeshtovao.count(mesh->m_meshId)<1)
				{
					auto vao = GLUtils::createVAO(*mesh);
					m_scmmeshtovao[mesh->m_meshId] = vao;
				}
				if (m_scmshadertovao.count(mesh->m_material->m_shaderid) < 1)
				{
					auto files = shaders[mesh->m_material->m_shaderid];
					auto prog = GLUtils::createShaderProgram(files.m_shaderFiles[0], files.m_shaderFiles[1]);
					m_scmshadertovao[files.m_shaderId] = prog;
				}
			}
		}
	}
	//bla
}

void GraphicsModule::drawSCMMesh(SCM::IdType meshid)
{
	auto vao = m_scmmeshtovao[meshid];
	if (vao->vao != 0)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); GLERR
			glBindVertexArray(vao->vao); GLERR
			glDrawElements(GL_TRIANGLES, vao->indexCount, GL_UNSIGNED_INT, 0); GLERR
			glBindVertexArray(0); GLERR
	}
	else
	{

	}
}
