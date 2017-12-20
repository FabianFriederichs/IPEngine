// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "GraphicsModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
GraphicsModule::GraphicsModule(void)
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
	handles.push_back(sched.subscribe(ipengine::TaskFunction::make_func<GraphicsModule, &GraphicsModule::render>(this), 0, ipengine::Scheduler::SubType::Frame, 1, &m_core->getThreadPool(), true));
	//std::vector<ipengine::any> anyvector;
	//anyvector.push_back(this);
	//anyvector.push_back(&m_scmID);
//	m_info.expoints.execute("TestPoint", { "this", "test" }, anyvector);
	return true;
}

void GraphicsModule::loadShaders()
{

}

void GraphicsModule::render()
{
	std::vector<ipengine::any> anyvector;
	anyvector.push_back(static_cast<IGraphics_API*>(this));
	anyvector.push_back(&m_scmID);
	m_info.expoints.execute("TestPoint", { "this", "test" }, anyvector);
	bool res;
	//if (wglGetCurrentContext() == NULL)
	//{
	//	res = wglMakeCurrent(info.info.win.hdc, wincontext);
	//}

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
		glDepthFunc(GL_LESS); GLERR;
	//go through meshes
	auto& activeentitynames = getActiveEntityNames(*m_scm);
	auto& entities = m_scm->getThreeDimEntities();
	for (auto eid : activeentitynames)
	{
		if (entities.count(eid) <= 0)
		{
			continue;
		}
		auto findent = entities.find(eid);

		if (findent !=entities.end())
		{
			auto mO = findent->second;
			for (auto mesh : mO->m_mesheObjects->m_meshes)
			{
				//activate shader
				auto shader = m_scmshadertoprogram[mesh->m_material->m_shaderId];
				shader->use();

				//set uniforms/light/transform/view/proj/camera pos
				shader->setUniform("model", mO->m_transformData.getData()->m_transformMatrix, false);
				
				if (cameraentity != IPID_INVALID)
				{
					
					auto cent = m_scm->getEntityById(cameraentity);
					if (cent != nullptr)
					{

						auto transdata = cent->m_transformData.getData();
						viewmat = ViewFromTransData(transdata);//glm::inverse(transdata->m_transformMatrix);//glm::toMat4(transdata->m_rotation) * translate(glm::mat4(1.0f), -transdata->m_location);
					}
				}


				shader->setUniform("view", viewmat, false);
				
				shader->setUniform("projection", projmat, false);
				
				shader->setUniform("camerapos", camerapos);

				//set material uniforms
				GLint inndex = 0;
				for (auto tdata : mesh->m_material->m_textures)
				{
					/*if (!this->getTextures()[i]->isBound())
					{*/
					m_scmtexturetot2d[tdata.second.m_texturefileId]->bind(inndex);

					//}

					shader->setUniform(tdata.first, inndex);

					shader->setUniform("offset", tdata.second.m_offset);

					shader->setUniform("size", tdata.second.m_size);

					//_material->getTextures()[i]->bindToTextureUnit(i);
				}

				shader->setUniform("material.texcount", (GLint)(inndex+1));
					
				

				//draw mesh
				drawSCMMesh(mesh->m_meshId);

				 //QUESTION is this necessary?
			}
		}
	}
	/*SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {}*/
	
	//wglMakeCurrent(NULL, NULL);

}

void GraphicsModule::render(ipengine::TaskContext & c)
{
	std::vector<ipengine::any> anyvector;
	anyvector.push_back(static_cast<IGraphics_API*>(this));
	anyvector.push_back(renderMatrixes({ &projmat, &viewmat }));
	m_info.expoints.execute("PreRender", { "this", "rendermatrixes"}, anyvector);
	anyvector.clear();

	render();

	anyvector.push_back(static_cast<IGraphics_API*>(this));
	anyvector.push_back(&m_scmID);
	m_info.expoints.execute("PostRender", { "this" }, anyvector);


	SDL_GL_SwapWindow(window);
	
}

std::vector<ipengine::ipid> GraphicsModule::getActiveEntityNames(SCM::ISimpleContentModule_API & scm)
{
	auto& entities = scm.getEntities();
	std::vector<ipengine::ipid> ids;
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
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
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
	//wglMakeCurrent(NULL, NULL);
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
		auto findent = entities.find(eid);
		if (findent != entities.end())
		{
			auto mO =findent->second;

			for (auto mesh : mO->m_mesheObjects->m_meshes)
			{
				//TODO Check for NULL material
				for (auto texts : mesh->m_material->m_textures)
				{
					if (m_scmtexturetot2d.count(texts.second.m_texturefileId) < 1)
					{
						for (auto fexfiles : textures)
						{
							if (fexfiles.m_textureId == texts.second.m_texturefileId)
							{
								m_scmtexturetot2d[texts.second.m_texturefileId] = GLUtils::loadGLTexture(fexfiles.m_path);
							}
						}
						//GLUtils::loadGLTexture(textures[])
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
					auto files = m_scm->getShaderById(mesh->m_material->m_shaderId);
					auto prog = GLUtils::createShaderProgram(files->m_shaderFiles[0], files->m_shaderFiles[1]);
					m_scmshadertoprogram[files->m_shaderId] = prog;
				}
			}
			//if (mO->m_transformData.getData()->m_isMatrixDirty)
			//{
			//	auto transdata = mO->m_transformData.getData();
			//	glm::mat4 tmat = glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
			//	mO->m_transformData.setData()->m_transformMatrix = tmat;//glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
			//	mO->m_transformData.setData()->m_isMatrixDirty = false;
			//	mO->m_transformData.setData()->m_localX = glm::normalize(glm::vec3(tmat[0][0], tmat[0][1], tmat[0][2]));
			//	mO->m_transformData.setData()->m_localY = glm::normalize(glm::vec3(tmat[1][0], tmat[1][1], tmat[1][2]));
			//	mO->m_transformData.setData()->m_localZ = glm::normalize(glm::vec3(tmat[2][0], tmat[2][1], tmat[2][2]));
			//}
		}
	}
	//bla
}

void GraphicsModule::drawSCMMesh(ipengine::ipid meshid)
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
	//if(m_scm->getEntityById(v) !=nullptr)
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

void GraphicsModule::setClipRange(float n, float f)
{
	znear = n; zfar = f;
	recalcProj();
}

glm::mat4 GraphicsModule::ViewFromTransData(const SCM::TransformData *transform)
{
	
	/*	x y z					negative translation	x	y	z
	|x.x	x.y		x.z		-cp.x| camera x achse	1	0	0
	|y.x	y.y		y.z		-cp.y| camera y achse	0	1	0
	|z.x	z.y		z.z		-cp.z| camera z achse   0	0	1
	|0		0		0		1    | */
	//transform.GetTransformMat();
		
	glm::mat4 viewRot = glm::mat4(
		glm::vec4(transform->m_localX.x, transform->m_localY.x, transform->m_localZ.x, 0.0f),
		glm::vec4(transform->m_localX.y, transform->m_localY.y, transform->m_localZ.y, 0.0f),
		glm::vec4(transform->m_localX.z, transform->m_localY.z, transform->m_localZ.z, 0.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));


	glm::mat4 viewTr = glm::mat4(
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(-transform->m_location.x, -transform->m_location.y, -transform->m_location.z, 1));

	viewmat = viewRot * viewTr;
	//vdirty = false;
	/*std::cout << "Camera axes: \n";
	std::cout << "X: " << transform.localx.x << " " << transform.localx.y << " " << transform.localx.z << "\n";
	std::cout << "Y: " << transform.localy.x << " " << transform.localy.y << " " << transform.localy.z << "\n";
	std::cout << "Z: " << transform.localz.x << " " << transform.localz.y << " " << transform.localz.z << "\n\n";*/
	
	return viewmat;
}

void GraphicsModule::recalcProj()
{
	projmat = glm::perspective(m_fov, width / height, znear, zfar);
}

uint32_t GraphicsModule::getCameraEntity()
{
	return cameraentity;
}

uint32_t GraphicsModule::getFOV()
{
	return m_fov;
}

void GraphicsModule::getResolution(uint32_t &w, uint32_t &h)
{
	w = width;
	h = height;
}

void GraphicsModule::getClipRange(float &n,float&f)
{
	n = znear;
	f = zfar;
}
