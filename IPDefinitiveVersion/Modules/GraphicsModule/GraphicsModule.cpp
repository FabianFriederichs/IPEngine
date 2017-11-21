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
	//loadShaders();
	ipengine::Scheduler& sched = m_core->getScheduler();
	handles.push_back(sched.subscribe(ipengine::TaskFunction::make_func<GraphicsModule, &GraphicsModule::render>(this), 0, ipengine::Scheduler::SubType::Frame, 1, &m_core->getThreadPool()));
	std::vector<ipengine::any> anyvector;
	anyvector.push_back(this);
	anyvector.push_back(&m_scmID);
	m_info.expoints.execute("TestPoint", { "this", "test" }, anyvector);
	return true;
}

void GraphicsModule::loadShaders()
{

}

void GraphicsModule::render()
{
	bool res;
	if (wglGetCurrentContext() == NULL)
	{
		res = wglMakeCurrent(info.info.win.hdc, wincontext);
	}

	updateData();
	//stuff
	//Clear buffer
	glClearColor(m_clearcolor.r, m_clearcolor.g, m_clearcolor.b, m_clearcolor.a); GLERR
	glEnable(GL_CULL_FACE); GLERR
	glFrontFace(GL_CCW); GLERR
	glCullFace(GL_BACK); GLERR
	glEnable(GL_DEPTH_TEST); GLERR
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GLERR
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); GLERR
	//go through meshes
	auto& activeentitynames = getActiveEntityNames(*m_scm);
	auto& entities = m_scm->getThreeDimEntities();
	for (auto eid : activeentitynames)
	{
		if (entities.count(eid) <= 0)
		{
			continue;
		}
		auto mO = entities[eid]; 

		if (mO)
		{
			for (auto mesh : mO->m_mesheObjects->m_meshes)
			{
				//activate shader
				auto shader = m_scmshadertoprogram[mesh->m_material->m_shaderId];
				shader->use();

				//set uniforms/light/transform/view/proj/camera pos
				shader->setUniform("model", mO->m_transformData.getData()->m_transformMatrix, false);
				if (cameraentity != SCM::EntityId(-1))
				{
					auto cent = m_scm->getEntityById(cameraentity);
					auto transdata = cent->m_transformData.getData();
					viewmat = glm::toMat4(transdata->m_rotation)*translate(glm::mat4(1.0f), transdata->m_location);
				}
				else
				{
					viewmat = glm::mat4(glm::quat(1.0f, 0.0f, .0f, .0f))*translate(glm::mat4(1.0f), -camerapos);
				}

				shader->setUniform("view", viewmat, false);
				
				shader->setUniform("projection", projmat, false);
				
				shader->setUniform("camerapos", camerapos);

				//set material uniforms
				//TODO
				for (auto tdata : mesh->m_material->m_textures)
				{
					shader->setUniform(tdata.first, m_scmtexturefiletogpu[tdata.second.m_texturefileId]);
					if (tdata.second.m_size.length() == 0)
					{
						//TODO set offset+size uniforms?
					}
				}

				//draw mesh
				drawSCMMesh(mesh->m_meshId);

				 //QUESTION is this necessary?
			}
		}
	}
	
	SDL_GL_SwapWindow(window);
	//wglMakeCurrent(NULL, NULL);

}

void GraphicsModule::render(ipengine::TaskContext & c)
{
	render();
}

std::vector<SCM::EntityId> GraphicsModule::getActiveEntityNames(SCM::ISimpleContentModule_API & scm)
{
	auto& entities = scm.getEntities();
	std::vector<SCM::EntityId> ids;
	for (auto e : entities)
	{
		if (e.second->isActive)
		{
			ids.push_back(e.second->m_entityId);
		}
	}
	return ids;
}

void GraphicsModule::setupSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		//std::cout << "Could not initialize SDL." << std::endl;
	}
	

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	window = SDL_CreateWindow("Demo Window", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
	
	if (window == NULL) {
		//std::cout << "Could not create SDL window." << std::endl;
		/*printDebug("Could not create SDL window.\n");
		return 1;*/
	}
	if (!SDL_GetWindowWMInfo(window, &info))
	{

	}
	context = SDL_GL_CreateContext(window);
	wincontext = wglGetCurrentContext();
	//check null
	SDL_GL_SetSwapInterval(0);
	glewExperimental = GL_TRUE;
	glewInit();
	wglMakeCurrent(NULL, NULL);
	//bla
}

void GraphicsModule::updateData()
{
	auto& activeentitynames= getActiveEntityNames(*m_scm);
	auto& entities = m_scm->getThreeDimEntities();
	auto& shaders = m_scm->getShaders();
	auto& textures = m_scm->getTextures();
	for (auto eid : activeentitynames)
	{
		auto mO = entities[eid];

		if (mO)
		{
			for (auto mesh : mO->m_mesheObjects->m_meshes)
			{
				for (auto texts : mesh->m_material->m_textures)
				{
					if (m_scmtexturefiletogpu.count(texts.second.m_texturefileId) < 1)
					{
						//TODO
						//load texture into gpu memory?? 
					}
				}
				if (m_scmmeshtovao.count(mesh->m_meshId)<1)
				{
					auto vao = (mesh->m_dynamic ? GLUtils::createDynamicVAO(*mesh) : GLUtils::createVAO(*mesh));
					m_scmmeshtovao[mesh->m_meshId] = vao;
				}
				else if(mesh->m_dynamic && mesh->m_dirty)//update dynamic meshes
				{
					auto& vao = m_scmmeshtovao[mesh->m_meshId];
					GLUtils::updateVAO(vao, *mesh);
				}				
				if (m_scmshadertoprogram.count(mesh->m_material->m_shaderId) < 1)
				{
					auto files = shaders[mesh->m_material->m_shaderId];
					auto prog = GLUtils::createShaderProgram(files.m_shaderFiles[0], files.m_shaderFiles[1]);
					m_scmshadertoprogram[files.m_shaderId] = prog;
				}
			}
			if (mO->m_transformData.getData()->m_isMatrixDirty)
			{
				auto transdata = mO->m_transformData.getData();
				mO->m_transformData.setData()->m_transformMatrix = glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
				mO->m_transformData.setData()->m_isMatrixDirty = false;
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

void GraphicsModule::setCameraEntity(uint32_t v)
{
	if(m_scm->getEntityById(v) !=nullptr)
		cameraentity = v;
}

void GraphicsModule::setFOV(uint32_t v)
{
	m_fov = v;
	recalcProj();
}

void GraphicsModule::setResolution(uint32_t x, uint32_t y)
{
	width = x;
	height = y;
	recalcProj();
}

void GraphicsModule::setClipRange(uint32_t n, uint32_t f)
{
	znear = n; zfar = f;
	recalcProj();
}

void GraphicsModule::recalcProj()
{
	projmat = glm::perspective(m_fov, width / height, znear, zfar);
}
