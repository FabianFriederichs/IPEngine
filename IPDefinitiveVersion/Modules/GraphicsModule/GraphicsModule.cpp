// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "GraphicsModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition

//public interface implementation ---------------------------------------------------------------------------------------------
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
	//get dependencies
	m_scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(m_scmID);	

	//setup
	setup();

	//subscribe to scheduler
	ipengine::Scheduler& sched = m_core->getScheduler();
	handles.push_back(sched.subscribe(ipengine::TaskFunction::make_func<GraphicsModule, &GraphicsModule::render>(this),
									  0,
									  ipengine::Scheduler::SubType::Frame,
									  1,
									  &m_core->getThreadPool(),
									  true)
	);
	return true;
}
void GraphicsModule::render(ipengine::TaskContext & c)
{
	updateData();

	std::vector<ipengine::any> anyvector;
	anyvector.push_back(static_cast<IGraphics_API*>(this));
	anyvector.push_back(renderMatrixes({&projmat, &viewmat}));
	m_info.expoints.execute("PreRender", {"this", "rendermatrixes"}, anyvector);
	anyvector.clear();

	render();

	//render();

	/*anyvector.push_back(static_cast<IGraphics_API*>(this));
	anyvector.push_back(&m_scmID);
	m_info.expoints.execute("PostRender", { "this" }, anyvector);*/


	SDL_GL_SwapWindow(window);
}
void GraphicsModule::render()
{
	static bool first = true;
	if (first)
	{
		updateData();
		first = false;
	}
	if (m_shadows)
	{
		renderDirectionalLightShadowMap();
	}
	//forward pbr render pass
	glClearColor(m_clearcolor.r, m_clearcolor.g, m_clearcolor.b, m_clearcolor.a);
	glViewport(0, 0, width, height);
	auto shader = m_s_pbrforward.get();
	shader->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//set scene uniforms (view, projection, lights etc..)
	setSceneUniforms(shader);
	//render opaque geometry
	drawScene(shader);
}
void GraphicsModule::setCameraEntity(ipengine::ipid v)
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
ipengine::ipid GraphicsModule::getCameraEntity()
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
void GraphicsModule::getClipRange(float &n, float&f)
{
	n = znear;
	f = zfar;
}

//setup -----------------------------------------------------------------------------------------------------------------------
void GraphicsModule::loadShaders()
{
	//load forward pbr shader
	auto vspath = m_core->getConfigManager().getString("graphics.shaders.pbr_forward.vertex");
	auto fspath = m_core->getConfigManager().getString("graphics.shaders.pbr_forward.fragment");

	m_s_pbrforward = GLUtils::createShaderProgram(vspath, fspath);

	if (m_ibl)
	{
		vspath = m_core->getConfigManager().getString("graphics.shaders.pbribl_forward.vertex");
		fspath = m_core->getConfigManager().getString("graphics.shaders.pbribl_forward.fragment");

		m_s_pbriblforward = GLUtils::createShaderProgram(vspath, fspath);

		if (m_ibldiffuse)
		{
			vspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.irradiance.vertex");
			fspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.irradiance.fragment");
			auto gspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.irradiance.geometry");

			m_s_ibldiff = GLUtils::createShaderProgram(vspath, fspath, gspath);
		}
		if (m_iblspecular)
		{
			vspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.specular.vertex");
			fspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.specular.fragment");
			auto gspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.specular.geometry");

			m_s_iblspec = GLUtils::createShaderProgram(vspath, fspath, gspath);

			vspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.brdf.vertex");
			fspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.brdf.fragment");

			m_s_iblbrdf = GLUtils::createShaderProgram(vspath, fspath);
		}
	}

	if (m_shadows)
	{
		vspath = m_core->getConfigManager().getString("graphics.shaders.shadow.vertex");
		fspath = m_core->getConfigManager().getString("graphics.shaders.shadow.fragment");

		m_s_shadow = GLUtils::createShaderProgram(vspath, fspath);

		vspath = m_core->getConfigManager().getString("graphics.shaders.gblur.vertex");
		fspath = m_core->getConfigManager().getString("graphics.shaders.gblur.fragment");

		m_s_gblur = GLUtils::createShaderProgram(vspath, fspath);
	}
}
void GraphicsModule::setupFrameBuffers()
{
	//shadow mapping framebuffers
	//shadow map
	FrameBufferDesc sfbd{
		{
			RenderTargetDesc{
				m_shadow_res_x,
				m_shadow_res_y,
				GL_RG32F,
				GL_COLOR_ATTACHMENT0,
				RenderTargetType::Texture2D
			}
		},
		RenderTargetDesc{
			m_shadow_res_x,
			m_shadow_res_y,
			GL_DEPTH24_STENCIL8,
			GL_DEPTH_STENCIL_ATTACHMENT,
			RenderTargetType::RenderBuffer
		}
	};
	m_fb_shadow = GLUtils::createFrameBuffer(sfbd);
	//shadow map blur
	FrameBufferDesc bfbd{
		{
			RenderTargetDesc{
				m_shadow_res_x,
				m_shadow_res_y,
				GL_RG32F,
				GL_COLOR_ATTACHMENT0,
				RenderTargetType::Texture2D
			}
		},
		RenderTargetDesc{} //empty: no depth test needed
	};
	m_fb_gblur1 = GLUtils::createFrameBuffer(bfbd);
	m_fb_gblur2 = GLUtils::createFrameBuffer(bfbd);
	//TODO: ibl gen framebuffers
}
void GraphicsModule::renderIBLMaps()
{}
void GraphicsModule::readSettings()
{
	//get settings from config file
	width = static_cast<float>(m_core->getConfigManager().getInt("graphics.window.width"));
	height = static_cast<float>(m_core->getConfigManager().getInt("graphics.window.height"));

	//texture map params
	bool en_mip = m_core->getConfigManager().getBool("graphics.materials.texturemaps.enable_mipmapping");
	bool lin_min = m_core->getConfigManager().getBool("graphics.materials.texturemaps.enable_mipmapping");
	bool lin_mag = m_core->getConfigManager().getBool("graphics.materials.texturemaps.enable_mipmapping");
	bool lin_mip = m_core->getConfigManager().getBool("graphics.materials.texturemaps.enable_mipmapping");
	bool en_aniso = m_core->getConfigManager().getBool("graphics.materials.texturemaps.enable_aniso_filter");
	int max_aniso = static_cast<int>(m_core->getConfigManager().getInt("graphics.materials.texturemaps.max_aniso_level"));

	if (lin_min)
		if (en_mip)
			if (lin_mip)
				m_mtexMinFilter = GL_LINEAR_MIPMAP_LINEAR;
			else
				m_mtexMinFilter = GL_LINEAR_MIPMAP_NEAREST;
		else
			m_mtexMinFilter = GL_LINEAR;
	else
		if (en_mip)
			if (lin_mip)
				m_mtexMinFilter = GL_NEAREST_MIPMAP_LINEAR;
			else
				m_mtexMinFilter = GL_NEAREST_MIPMAP_NEAREST;
		else
			m_mtexMinFilter = GL_NEAREST;

	if (lin_mag)
		m_mtexMagFilter = GL_LINEAR;
	else
		m_mtexMagFilter = GL_NEAREST;

	if (en_aniso)
	{
		m_mtexAniso = true;
		m_mtexMaxAnisoLevel = max_aniso;
	}

	//light settings
	m_ambientLight = glm::vec3(
		static_cast<float>(m_core->getConfigManager().getFloat("graphics.lighting.ambient_light.r")),
		static_cast<float>(m_core->getConfigManager().getFloat("graphics.lighting.ambient_light.g")),
		static_cast<float>(m_core->getConfigManager().getFloat("graphics.lighting.ambient_light.b"))
	);

	m_max_dirlights = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.max_dirlights"));
	m_max_pointlights = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.max_pointlights"));
	m_max_spotlights = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.max_spotlights"));

	//hdr exposure
	m_exposure = static_cast<float>(m_core->getConfigManager().getFloat("graphics.lighting.tone_mapping_exposure"));

	//ibl settings
	m_ibl = m_core->getConfigManager().getBool("graphics.lighting.ibl.enable_ibl");
	m_ibldiffuse = m_core->getConfigManager().getBool("graphics.lighting.ibl.diffuse_ibl");
	m_iblspecular = m_core->getConfigManager().getBool("graphics.lighting.ibl.specular_ibl");

	//shadow settings
	m_shadows = m_ibldiffuse = m_core->getConfigManager().getBool("graphics.lighting.shadows.enable_shadows");
	m_shadow_res_x = m_core->getConfigManager().getInt("graphics.lighting.shadows.res.x");
	m_shadow_res_y = m_core->getConfigManager().getInt("graphics.lighting.shadows.res.y");
	m_shadow_blur_passes = m_core->getConfigManager().getInt("graphics.lighting.shadows.blur_passes");
}
void GraphicsModule::prepareAssets()
{
	//TODO: render ibl maps here
	if (m_ibl)
	{
		renderIBLMaps();
	}
}
void GraphicsModule::setDefaultGLState()
{
	//initial gl state
	glClearColor(m_clearcolor.r, m_clearcolor.g, m_clearcolor.b, m_clearcolor.a);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}
void GraphicsModule::setup()
{
	//setup
	readSettings();
	setupSDL();
	loadShaders();
	setupFrameBuffers();
	prepareAssets();
	setDefaultGLState();
}
void GraphicsModule::setupSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		//std::cout << "Could not initialize SDL." << std::endl;
	}


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, static_cast<int>(m_core->getConfigManager().getInt("graphics.opengl.version_major")));
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, static_cast<int>(m_core->getConfigManager().getInt("graphics.opengl.version_minor")));
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if (m_core->getConfigManager().getBool("graphics.window.msaa"))
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, static_cast<int>(m_core->getConfigManager().getBool("graphics.window.msaa_samples")));
	}
	window = SDL_CreateWindow(m_core->getConfigManager().getString("graphics.window.title").c_str(), SDL_WINDOWPOS_CENTERED,
							  SDL_WINDOWPOS_CENTERED, static_cast<int>(width), static_cast<int>(height), SDL_WINDOW_OPENGL);

	if (window == NULL)
	{
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
	SDL_GL_SetSwapInterval(m_core->getConfigManager().getBool("graphics.window.vsync") ? 1 : 0);
	glewExperimental = GL_TRUE;
	glewInit();
	//wglMakeCurrent(NULL, NULL);
	//bla
}

//update ----------------------------------------------------------------------------------------------------------------------
void GraphicsModule::updateData()
{
	auto& activeentitynames = getActiveEntityNames(*m_scm);
	auto& entities = m_scm->getThreeDimEntities();
	auto& shaders = m_scm->getShaders();
	auto& textures = m_scm->getTextures();
	for (auto eid : activeentitynames)
	{
		auto findent = entities.find(eid);
		if (findent != entities.end())
		{
			auto mO = findent->second;

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
								auto glt = GLUtils::loadGLTexture(fexfiles.m_path, true);
								setMaterialTexParams(glt->tex);
								m_scmtexturetot2d[texts.second.m_texturefileId] = glt;
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
				else if (mesh->m_dynamic && mesh->m_dirty)//update dynamic meshes
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

			/*if (mO->m_transformData.getData()->m_isMatrixDirty)
			{
			mO->m_transformData.setData()->updateTransform();
			}*/
		}
	}
	//bla
}

//rendering -------------------------------------------------------------------------------------------------------------------
void GraphicsModule::drawScene(ShaderProgram* shader)
{
	auto& drawableEntities = m_scm->getThreeDimEntities();
	for (auto ep : drawableEntities)
	{
		auto entity = ep.second;
		if (!entity->isActive)
			continue;

		drawEntity(entity, shader);
	}
}
void GraphicsModule::drawSceneShadow(ShaderProgram* shader)
{
	auto& drawableEntities = m_scm->getThreeDimEntities();
	for (auto ep : drawableEntities)
	{
		auto entity = ep.second;
		if (!entity->isActive)
			continue;

		drawEntityShadow(entity, shader);
	}
}
void GraphicsModule::setSceneUniforms(ShaderProgram* shader)
{
	if (cameraentity != IPID_INVALID)
	{
		auto cent = m_scm->getEntityById(cameraentity);
		if (cent != nullptr)
		{
			auto transdata = cent->m_transformData.getData();
			viewmat = ViewFromTransData(transdata);//glm::inverse(transdata->m_transformMatrix);//glm::toMat4(transdata->m_rotation) * translate(glm::mat4(1.0f), -transdata->m_location);
		}
	}
	shader->setUniform("u_view_matrix", viewmat, false);
	shader->setUniform("u_projection_matrix", projmat, false);
	shader->setUniform("u_toneMappingExposure", m_exposure);
	setLightUniforms(shader);

	//quick test. remove that shit!
	shader->setUniform("u_directionalLights[0].color", glm::vec3(1.0f));
	shader->setUniform("u_directionalLights[0].direction", glm::mat3(viewmat) * glm::vec3(-1.0f, -1.0f, -1.0f));
	shader->setUniform("u_dirLightCount", 1);
}
void GraphicsModule::setLightUniforms(ShaderProgram* shader)
{
	auto& dirlights = m_scm->getDirLights();
	auto& pointlights = m_scm->getPointLights();
	auto& spotlights = m_scm->getSpotLights();

	//for now hacked dir light shadow
	if (m_shadows)
	{
		shader->setUniform("u_enableShadows", 1);
		shader->setUniform("u_light_matrix", m_dirLightMat, false);		
		m_ot_shadowmap->bind(20);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		float impmax;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &impmax);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_mtexMaxAnisoLevel >= impmax ? impmax : m_mtexMaxAnisoLevel);
		shader->setUniform("u_shadowMap", 20);
	}
	else
	{
		shader->setUniform("u_enableShadows", 0);
	}

	int lc = 0;
	//somehow store shadow stuff in light structs and set that too
	for (auto dl : dirlights)
	{
		shader->setUniform("u_directionslLights[" + std::to_string(lc) + "].color", dl.second->m_color);
		shader->setUniform("u_directionslLights[" + std::to_string(lc) + "].direction", dl.second->getVSDirection(viewmat));
		++lc;
		if (lc >= m_max_dirlights)
			break;
	}
	shader->setUniform("u_dirLightCount", lc);

	lc = 0;
	for (auto pl : pointlights)
	{
		shader->setUniform("u_pointLights[" + std::to_string(lc) + "].color", pl.second->m_color);
		shader->setUniform("u_pointLights[" + std::to_string(lc) + "].position", pl.second->getVSPosition(viewmat));
		shader->setUniform("u_pointLights[" + std::to_string(lc) + "].max_range", pl.second->m_range);
		++lc;
		if (lc >= m_max_pointlights)
			break;
	}
	shader->setUniform("u_pointLightCount", lc);

	lc = 0;
	for (auto sl : spotlights)
	{
		shader->setUniform("u_spotLights[" + std::to_string(lc) + "].color", sl.second->m_color);
		shader->setUniform("u_spotLights[" + std::to_string(lc) + "].position", sl.second->getVSPosition(viewmat));
		shader->setUniform("u_spotLights[" + std::to_string(lc) + "].max_range", sl.second->m_range);
		shader->setUniform("u_spotLights[" + std::to_string(lc) + "].direction", sl.second->getVSDirection(viewmat));
		shader->setUniform("u_spotLights[" + std::to_string(lc) + "].outer_cone_angle", sl.second->m_outerConeAngle);
		shader->setUniform("u_spotLights[" + std::to_string(lc) + "].inner_cone_angle", sl.second->m_innerConeAngle);
		++lc;
		if (lc >= m_max_spotlights)
			break;
	}
	shader->setUniform("u_spotLightCount", lc);

	shader->setUniform("u_ambientLight", m_ambientLight);
}
void GraphicsModule::setMaterialUniforms(SCM::MaterialData * mdata, ShaderProgram* shader)
{
	int tc = 0;
	for (auto tp : mdata->m_textures)
	{
		auto& t = tp.second;
		auto& tex = m_scmtexturetot2d[t.m_texturefileId];
		tex->bind(tc);
		shader->setUniform("u_material." + tp.first, tc);
		++tc;
	}
	//shader->setUniform("u_material.texcount", tc);
	//TODO: texture scaling / offset
}
void GraphicsModule::drawEntity(SCM::ThreeDimEntity * entity, ShaderProgram* shader)
{
	//set per entity uniforms
	const glm::mat4& transformMat = entity->m_transformData.getData()->m_transformMatrix;
	shader->setUniform("u_model_matrix", transformMat, false);

	//draw all meshes
	for (auto m : entity->m_mesheObjects->m_meshes)
	{
		//TODO:: optimization! create batches of meshes with the same material
		setMaterialUniforms(m->m_material, shader);
		drawSCMMesh(m->m_meshId);
	}
}
void GraphicsModule::drawEntityShadow(SCM::ThreeDimEntity * entity, ShaderProgram* shader)
{
	//set per entity uniforms
	const glm::mat4& transformMat = entity->m_transformData.getData()->m_transformMatrix;
	shader->setUniform("u_model_matrix", transformMat, false);

	//draw all meshes
	for (auto m : entity->m_mesheObjects->m_meshes)
	{
		drawSCMMesh(m->m_meshId);
	}
}
void GraphicsModule::renderDirectionalLightShadowMap(/*SCM::DirectionalLight& dirLight, vec3 max, vec3 min*/)
{
	glm::mat4 lightView;
	glm::mat4 lightProj;

	//for testing: handcraft the matrices
	lightProj = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);
	lightView = glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	m_dirLightMat = lightProj * lightView;
	//later do that:
	//lightMatDirectionalLight(lightView, lightProj, dirLight, max, min);
	//render scene into shadow map
	m_fb_shadow->bind(GL_FRAMEBUFFER);
	glClearColor(m_shadowclearcolor.r, m_shadowclearcolor.g, m_shadowclearcolor.b, m_shadowclearcolor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_shadow_res_x, m_shadow_res_y);
	m_s_shadow->use();
	m_s_shadow->setUniform("u_light_matrix", m_dirLightMat, false);
	drawSceneShadow(m_s_shadow.get());	
	if (m_shadow_blur_passes <= 0)
	{
		m_fb_shadow->unbind(GL_FRAMEBUFFER);
		m_ot_shadowmap = m_fb_shadow->colorTargets[0].tex;
		setDefaultGLState();
		return;
	}
	//blur shadow map
	m_s_gblur->use();
	glDisable(GL_DEPTH_TEST);
	int currentFB = 1;
	for (int i = 0; i < m_shadow_blur_passes; ++i)
	{
		//blur horizontal
		m_fb_gblur1->bind(GL_FRAMEBUFFER);		
		glClear(GL_COLOR_BUFFER_BIT);
		if (i == 0)		
			m_fb_shadow->colorTargets[0].tex->bind(0);		
		else		
			m_fb_gblur2->colorTargets[0].tex->bind(0);		
		m_s_gblur->setUniform("u_input", 0);
		m_s_gblur->setUniform("u_horizontal", 1);
		Primitives::drawNDCQuad();
		//blur vertical
		m_fb_gblur2->bind(GL_FRAMEBUFFER);
		glClear(GL_COLOR_BUFFER_BIT);
		m_fb_gblur1->colorTargets[0].tex->bind(0);
		m_s_gblur->setUniform("u_input", 0);
		m_s_gblur->setUniform("u_horizontal", 0);
		Primitives::drawNDCQuad();
	}
	m_ot_shadowmap = m_fb_gblur2->colorTargets[0].tex;
	m_fb_gblur2->unbind(GL_FRAMEBUFFER);
	setDefaultGLState();
}
void GraphicsModule::setMaterialTexParams(GLuint tex)
{
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_mtexMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_mtexMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	float impmax;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &impmax);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, m_mtexMaxAnisoLevel >= impmax ? impmax : m_mtexMaxAnisoLevel);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void GraphicsModule::drawSCMMesh(ipengine::ipid meshid)
{
	auto vao = m_scmmeshtovao[meshid];
	if (vao->vao != 0)
	{
		glBindVertexArray(vao->vao);
		glDrawElements(GL_TRIANGLES, vao->indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
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
void GraphicsModule::lightMatDirectionalLight(glm::mat4& view, glm::mat4& proj, SCM::DirectionalLight& dirLight, const glm::vec3& min, const glm::vec3& max)
{
	//hmm rework later
	proj = glm::ortho(min.x, max.x, min.y, max.y, 0.1f, max.z - min.z);
	view = ViewFromTransData(dirLight.m_transformData.getData());
}
//helpers ---------------------------------------------------------------------------------------------------------------------
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


//working comments

//void GraphicsModule::drawScene()
//{
//	auto& activeentitynames = getActiveEntityNames(*m_scm);
//	auto& entities = m_scm->getThreeDimEntities();
//	for (auto eid : activeentitynames)
//	{
//		if (entities.count(eid) <= 0)
//		{
//			continue;
//		}
//		auto findent = entities.find(eid);
//
//		if (findent != entities.end())
//		{
//			auto mO = findent->second;
//			for (auto mesh : mO->m_mesheObjects->m_meshes)
//			{
//				//activate shader
//				auto shader = m_scmshadertoprogram[mesh->m_material->m_shaderId];
//				shader->use();
//
//				//set uniforms/light/transform/view/proj/camera pos
//				shader->setUniform("model_matrix", mO->m_transformData.getData()->m_transformMatrix, false);
//
//				if (cameraentity != IPID_INVALID)
//				{
//
//					auto cent = m_scm->getEntityById(cameraentity);
//					if (cent != nullptr)
//					{
//
//						auto transdata = cent->m_transformData.getData();
//						viewmat = ViewFromTransData(transdata);//glm::inverse(transdata->m_transformMatrix);//glm::toMat4(transdata->m_rotation) * translate(glm::mat4(1.0f), -transdata->m_location);
//					}
//				}
//
//
//				shader->setUniform("view_matrix", viewmat, false);
//
//				shader->setUniform("projection_matrix", projmat, false);
//
//				//shader->setUniform("camerapos", camerapos);
//
//				//set material uniforms
//				GLint inndex = 0;
//				for (auto tdata : mesh->m_material->m_textures)
//				{
//					/*if (!this->getTextures()[i]->isBound())
//					{*/
//					m_scmtexturetot2d[tdata.second.m_texturefileId]->bind(inndex);
//
//					//}
//
//					shader->setUniform(tdata.first, inndex);
//
//					shader->setUniform("tc_offset", tdata.second.m_offset);
//
//					shader->setUniform("tc_scale", tdata.second.m_size);
//
//					//_material->getTextures()[i]->bindToTextureUnit(i);
//				}
//
//				shader->setUniform("material.texcount", (GLint)(inndex + 1));
//
//
//
//				//draw mesh
//				drawSCMMesh(mesh->m_meshId);
//
//				//QUESTION is this necessary?
//			}
//		}
//	}
//}