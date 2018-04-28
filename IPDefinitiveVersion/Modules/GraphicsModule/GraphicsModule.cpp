// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "GraphicsModule.h"

bool GraphicsModule::_startup()
{
	//get dependencies
	m_scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(m_scmID);

	//setup
	setup();
	int mvc;
	glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &mvc);
	std::cout << "Max varying components: " << mvc << "\n";
	//subscribe to scheduler
	ipengine::Scheduler& sched = m_core->getScheduler();
	handles.push_back(sched.subscribe(ipengine::TaskFunction::make_func<GraphicsModule, &GraphicsModule::render>(this),
		0,
		ipengine::Scheduler::SubType::Frame,
		1,
		&m_core->getThreadPool(),
		true)
	);
	auto lid = m_core->createID();
	
	m_scm->getDirLights()[lid] = new SCM::DirectionalLight(
		lid,
		SCM::Transform(glm::vec3(5, 5, 5), SCM::ISimpleContentModule_API::dirToQuat(glm::vec3(-1, -1, -1)), glm::vec3(1, 1, 1)),
		SCM::BoundingData(SCM::BoundingSphere()),
		false,
		true,
		glm::vec3(1, 1, 1),
		1024,
		1024,
		1,
		0.001,
		0.1,
		65.0f,
		glm::vec3(-8, -8, 40.0f),
		glm::vec3(8, 8, 0.1f)
	);

	/*lid = m_core->createID();

	m_scm->getDirLights()[lid] = new SCM::DirectionalLight(
		lid,
		SCM::Transform(SCM::TransformData(glm::vec3(-5, 5, 5), SCM::ISimpleContentModule_API::dirToQuat(glm::vec3(1, -1, -1)), glm::vec3(1, 1, 1))),
		SCM::BoundingData(SCM::BoundingSphere()),
		false,
		true,
		glm::vec3(0.34, 0.2, 0.2),
		1024,
		1024,
		1,
		1.e-6,
		0.3,
		65.0f,
		glm::vec3(-5, -5, 40.0f),
		glm::vec3(5, 5, 0.1f)
	);*/
	return true;
}

void GraphicsModule::loadTextureFromMemory(const GrAPI::t2d & data, const ipengine::ipid id)
{
	GLsizei width;
	GLsizei height;
	GLsizei channels;
	GLuint texid = 0;
	if (data.data == nullptr)
	{
		throw std::logic_error("Texture file coudn't be read.");
	}
	else
	{
		GLint internalformat;
		GLenum format;
		switch (data.channels)
		{
			case 1:
				internalformat = GL_R8;
				format = GL_RED;
				break;
			case 2:
				internalformat = GL_RG8;
				format = GL_RG;
				break;
			case 3:
				internalformat = GL_RGB8;
				format = GL_RGB;
				break;
			case 4:
				internalformat = GL_RGBA8;
				format = GL_RGBA;
				break;
			default:
				internalformat = GL_RGB8;
				format = GL_RGB;
				break;
		}
		glGenTextures(1, &texid); GLERR
		if (texid == 0)
		{
			throw std::logic_error("OpenGL texture object creation failed.");
		}
		glBindTexture(GL_TEXTURE_2D, texid); GLERR
		glTexImage2D(
			GL_TEXTURE_2D,		
			0,					
			internalformat,			
			data.width,				
			data.height,				
			0,					
			format,			
			GL_UNSIGNED_BYTE,	
			data.data				
		);
		if (checkglerror())
		{
			glDeleteTextures(1, &texid);
			//SOIL_free_image_data(image);
			throw std::logic_error("Error. Could not buffer texture data.");
		}
		/*if(genMipMaps)
			glGenerateMipmap(GL_TEXTURE_2D); GLERR*/
		glBindTexture(GL_TEXTURE_2D, 0); GLERR

		//SOIL_free_image_data(image);
	}

	m_scmtexturetot2d[id] = std::make_shared<Texture2D>(texid);

	//return std::make_shared<Texture2D>(texid);

}

bool GraphicsModule::_shutdown()
{
	handles.clear();
	//!TODO free graphics related stuff
	//destroy all opengl objects

	m_dirLightShadowTargets.clear();
	m_dirLightShadowBlurTargets1.clear();
	m_dirLightShadowBlurTargets2.clear();
	m_dirLightMatrices.clear();
	m_cube_envmap.reset();
	m_er_envmap.reset();
	m_s_pbrforward.reset();
	m_s_pbriblforward.reset();
	m_s_gblur.reset();
	m_s_skybox.reset();
	m_s_shadow.reset();
	m_s_ibldiff.reset();
	m_s_iblspec.reset();
	m_s_iblbrdf.reset();
	m_s_envconv.reset();
	m_fb_shadow.reset();
	m_fb_gblur1.reset();
	m_fb_gblur2.reset();
	m_fb_iblgenirradiance.reset();
	m_fb_iblgenspecular.reset();
	m_fb_iblgenbrdf.reset();
	m_fb_envconv.reset();
	m_ot_shadowmap.reset();
	m_ot_irradiance.reset();
	m_ot_specularradiance.reset();
	m_ot_brdfresponse.reset();
	m_scmmeshtovao.clear();
	m_scmshadertoprogram.clear();
	m_scmtexturetot2d.clear();


	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	return true;
}

void GraphicsModule::setMaterialTexDefaultParams(ipengine::ipid id, bool genmipmaps)
{
	auto& it = m_scmtexturetot2d.find(id);
	if (it == m_scmtexturetot2d.end())
		return;

	if (genmipmaps)
	{
		it->second->genMipMaps();
		it->second->setTexParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, m_mtexMaxAnisoLevel);
	}
	else
	{
		it->second->setTexParams(GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT, m_mtexMaxAnisoLevel);
	}
}

//public interface implementation ---------------------------------------------------------------------------------------------
GraphicsModule::GraphicsModule(void)
{
	DataDepName = "Data";
	m_info.identifier = "GraphicsModule";
	m_info.version = "1.0";
	m_info.iam = "IGraphics_API";
	return;
}

void GraphicsModule::render(ipengine::TaskContext & c)
{
	updateData();

	m_shadowsdirty = true;

	std::vector<ipengine::any> anyvector;
	anyvector.push_back(static_cast<IGraphics_API*>(this));
	anyvector.push_back(renderMatrixes({&projmat, &viewmat}));
	m_info.expoints.execute("PreRender", {"this", "rendermatrixes"}, anyvector);
	anyvector.clear();

	render(0, width, height, this->m_multisample);

	//render();

	/*anyvector.push_back(static_cast<IGraphics_API*>(this));
	anyvector.push_back(&m_scmID);
	m_info.expoints.execute("PostRender", { "this" }, anyvector);*/


	SDL_GL_SwapWindow(window);
}
void GraphicsModule::render()
{
	//vr init hack
	/*static bool vrinited = false;
	if (!vrinited)
	{
		updateData();
		vrinited = true;
	}*/
	//forward pbr render pass
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//render skybox or er map
	renderEnvMap();
	auto shader = (m_ibl ? m_s_pbriblforward.get() : m_s_pbrforward.get());
	shader->use();
	//set scene uniforms (view, projection, lights etc..)
	setSceneUniforms(shader);
	//render opaque geometry
	drawScene(shader);
}
void GraphicsModule::render(int fbo, int viewportx, int viewporty, bool multisample)
{
	if (m_shadows && m_shadowsdirty)
	{
		//for each dirlight
		for (auto& dl : m_scm->getDirLights())
		{
			if (dl.second->castShadows)
				renderDirectionalLightShadowMap(*dl.second);
		}
		m_shadowsdirty = false;
	}

	if (multisample)
		glEnable(GL_MULTISAMPLE);
	else
		glDisable(GL_MULTISAMPLE);
	//forward pbr render pass
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, viewportx, viewporty);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//render skybox or er map
	renderEnvMap();
	auto shader = (m_ibl ? m_s_pbriblforward.get() : m_s_pbrforward.get());
	shader->use();
	//set scene uniforms (view, projection, lights etc..)
	setSceneUniforms(shader);
	//render opaque geometry
	drawScene(shader);

	if (m_debug_bvs)
	{
		renderBoundingVolumes();
	}
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
	if (m_shadows)
	{
		auto vspath = m_core->getConfigManager().getString("graphics.shaders.pbr_forward_shadow.vertex");
		auto fspath = m_core->getConfigManager().getString("graphics.shaders.pbr_forward_shadow.fragment");
		m_s_pbrforward = GLUtils::createShaderProgram(vspath, fspath);
	}
	else
	{
		auto vspath = m_core->getConfigManager().getString("graphics.shaders.pbr_forward.vertex");
		auto fspath = m_core->getConfigManager().getString("graphics.shaders.pbr_forward.fragment");
		m_s_pbrforward = GLUtils::createShaderProgram(vspath, fspath);
	}



	if (m_ibl)
	{
		if (m_shadows)
		{
			auto vspath = m_core->getConfigManager().getString("graphics.shaders.pbribl_forward_shadow.vertex");
			auto fspath = m_core->getConfigManager().getString("graphics.shaders.pbribl_forward_shadow.fragment");
			m_s_pbriblforward = GLUtils::createShaderProgram(vspath, fspath);
		}
		else
		{
			auto vspath = m_core->getConfigManager().getString("graphics.shaders.pbribl_forward.vertex");
			auto fspath = m_core->getConfigManager().getString("graphics.shaders.pbribl_forward.fragment");
			m_s_pbriblforward = GLUtils::createShaderProgram(vspath, fspath);
		}

		if (m_ibldiffuse)
		{
			auto vspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.irradiance.vertex");
			auto fspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.irradiance.fragment");
			auto gspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.irradiance.geometry");

			m_s_ibldiff = GLUtils::createShaderProgram(vspath, fspath, gspath);
		}
		if (m_iblspecular)
		{
			auto vspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.specular.vertex");
			auto fspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.specular.fragment");
			auto gspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.specular.geometry");

			m_s_iblspec = GLUtils::createShaderProgram(vspath, fspath, gspath);

			vspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.brdf.vertex");
			fspath = m_core->getConfigManager().getString("graphics.shaders.iblgen.brdf.fragment");

			m_s_iblbrdf = GLUtils::createShaderProgram(vspath, fspath);
		}
	}

	if (m_shadows)
	{
		auto vspath = m_core->getConfigManager().getString("graphics.shaders.shadow.vertex");
		auto fspath = m_core->getConfigManager().getString("graphics.shaders.shadow.fragment");

		m_s_shadow = GLUtils::createShaderProgram(vspath, fspath);

		vspath = m_core->getConfigManager().getString("graphics.shaders.gblur.vertex");
		fspath = m_core->getConfigManager().getString("graphics.shaders.gblur.fragment");

		m_s_gblur = GLUtils::createShaderProgram(vspath, fspath);
	}

	auto vspath = m_core->getConfigManager().getString("graphics.shaders.envconv.vertex");
	auto fspath = m_core->getConfigManager().getString("graphics.shaders.envconv.fragment");
	auto gspath = m_core->getConfigManager().getString("graphics.shaders.envconv.geometry");
	m_s_envconv = GLUtils::createShaderProgram(vspath, fspath, gspath);

	if (m_display_envmap)
	{
		vspath = m_core->getConfigManager().getString("graphics.shaders.skybox.vertex");
		fspath = m_core->getConfigManager().getString("graphics.shaders.skybox.fragment");
		m_s_skybox = GLUtils::createShaderProgram(vspath, fspath);
	}

	if (m_debug_bvs)
	{
		vspath = m_core->getConfigManager().getString("graphics.shaders.bv_debug.vertex");
		fspath = m_core->getConfigManager().getString("graphics.shaders.bv_debug.fragment");
		m_s_bvdebug = GLUtils::createShaderProgram(vspath, fspath);
	}
}
void GraphicsModule::setupFrameBuffers()
{
	//shadow mapping framebuffers
	//shadow map
	if (m_shadows)
	{
		/*FrameBufferDesc sfbd{
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
		};*/
		m_fb_shadow = GLUtils::createFrameBuffer();
		//shadow map blur
		//FrameBufferDesc bfbd{
		//	{
		//		RenderTargetDesc{
		//			m_shadow_res_x,
		//			m_shadow_res_y,
		//			GL_RG32F,
		//			GL_COLOR_ATTACHMENT0,
		//			RenderTargetType::Texture2D
		//		}
		//	},
		//	RenderTargetDesc{} //empty: no depth test needed
		//};
		m_fb_gblur1 = GLUtils::createFrameBuffer();
		m_fb_gblur2 = GLUtils::createFrameBuffer();
	}
	//convenv
	FrameBufferDesc fbconvenv{
		{
			RenderTargetDesc{
			m_envcuberes,
			m_envcuberes,
			GL_RGB16F,
			GL_COLOR_ATTACHMENT0,
			RenderTargetType::TextureCube
	}
		},
		RenderTargetDesc{} //empty: no depth test needed
	};
	m_fb_envconv = GLUtils::createFrameBuffer(fbconvenv);

	//ibl
	if (m_ibl)
	{
		//diffuse
		if (m_ibldiffuse)
		{
			FrameBufferDesc ibldfbd{
				{
					RenderTargetDesc{
						m_irradiance_map_resx,
						m_irradiance_map_resy,
						GL_RGB16F,
						GL_COLOR_ATTACHMENT0,
						RenderTargetType::TextureCube
					}
				},
				RenderTargetDesc{} //empty: no depth test needed
			};
			m_fb_iblgenirradiance = GLUtils::createFrameBuffer(ibldfbd);
		}

		//specular
		if (m_iblspecular)
		{
			FrameBufferDesc iblsfbd{
				{
					RenderTargetDesc{
						m_specular_map_resx,
						m_specular_map_resy,
						GL_RGB16F,
						GL_COLOR_ATTACHMENT0,
						RenderTargetType::TextureCube,
						m_specular_mipmap_levels
					}
				},
				RenderTargetDesc{} //empty: no depth test needed
			};
			m_fb_iblgenspecular = GLUtils::createFrameBuffer(iblsfbd);

			//brdf response
			FrameBufferDesc iblbfbd{
				{
					RenderTargetDesc{
						m_specular_brdf_resx,
						m_specular_brdf_resy,
						GL_RG16F,
						GL_COLOR_ATTACHMENT0,
						RenderTargetType::Texture2D
					}
				},
				RenderTargetDesc{} //empty: no depth test needed
			};
			m_fb_iblgenbrdf = GLUtils::createFrameBuffer(iblbfbd);
		}
	}
}
void GraphicsModule::renderIBLMaps()
{
	//diffuse
	if (m_ibldiffuse)
	{
		m_fb_iblgenirradiance->bind(GL_FRAMEBUFFER); GLERR
		glDisable(GL_DEPTH_TEST); GLERR
		glViewport(0, 0, m_irradiance_map_resx, m_irradiance_map_resy); GLERR
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GLERR
		glClear(GL_COLOR_BUFFER_BIT); GLERR
		m_s_ibldiff->use();	GLERR
		//generate layer matrices
		glm::mat4 pmat = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 layermats[] = {  //px, nx, py, ny, pz, nz
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
		};
		for (size_t i = 0; i < 6; i++)
			m_s_ibldiff->setUniform(("u_layer_matrices[" + std::to_string(i) + "]").c_str(), layermats[i], false); GLERR
		m_s_ibldiff->bindTex("u_envcube", m_cube_envmap.get());
		m_s_ibldiff->setUniform("u_sample_delta", m_irradiance_sample_delta); GLERR
		Primitives::drawNDCCube(); GLERR
		m_fb_iblgenirradiance->unbind(GL_FRAMEBUFFER); GLERR
		m_ot_irradiance = m_fb_iblgenirradiance->rtset.colorTargets[0].ctex;
		m_ot_irradiance->setTexParams(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_mtexMaxAnisoLevel);
	}

	m_fb_iblgenirradiance.reset();

	//specular
	if (m_iblspecular)
	{
		m_fb_iblgenspecular->bind(GL_FRAMEBUFFER); GLERR
		m_s_iblspec->use();	GLERR
		glm::mat4 pmat = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 layermats[] = {  //px, nx, py, ny, pz, nz
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
		};
		for (size_t i = 0; i < 6; i++)
			m_s_iblspec->setUniform(("u_layer_matrices[" + std::to_string(i) + "]").c_str(), layermats[i], false); GLERR

		m_s_iblspec->bindTex("u_envcube", m_cube_envmap.get());
		m_s_iblspec->setUniform("u_samplecount", static_cast<GLuint>(m_specular_samples)); GLERR
		m_s_iblspec->setUniform("u_cmres", m_envcuberes); GLERR

		for (int k = 0; k < m_specular_mipmap_levels; ++k)
		{
			bool res = m_fb_iblgenspecular->selectColorTargetMipmapLevel(0, k);
			int mipwidth = m_specular_map_resx * glm::pow(0.5f, static_cast<float>(k));
			int mipheight = m_specular_map_resy * glm::pow(0.5f, static_cast<float>(k));
			glViewport(0, 0, mipwidth, mipheight); GLERR
			glClear(GL_COLOR_BUFFER_BIT); GLERR
			//set roughness
			m_s_iblspec->setUniform("u_roughness", static_cast<float>(k) / static_cast<float>(m_specular_mipmap_levels - 1));
			Primitives::drawNDCCube(); GLERR
		}
		m_fb_iblgenspecular->unbind(GL_FRAMEBUFFER); GLERR
		m_ot_specularradiance = m_fb_iblgenspecular->rtset.colorTargets[0].ctex;
		m_ot_specularradiance->setTexParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_mtexMaxAnisoLevel);
		m_fb_iblgenspecular.reset();

		//brdf
		m_fb_iblgenbrdf->bind(GL_FRAMEBUFFER);
		glViewport(0, 0, m_specular_brdf_resx, m_specular_brdf_resy);
		glClear(GL_COLOR_BUFFER_BIT);
		m_s_iblbrdf->use();
		m_s_iblbrdf->setUniform("u_brdfsamples", static_cast<GLuint>(m_brdfsamples));
		Primitives::drawNDCQuad();
		m_fb_iblgenbrdf->unbind(GL_FRAMEBUFFER); GLERR
		m_ot_brdfresponse = m_fb_iblgenbrdf->rtset.colorTargets[0].tex;
		m_ot_brdfresponse->setTexParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_mtexMaxAnisoLevel);

		m_fb_iblgenbrdf.reset();
	}
	glFinish(); GLERR
}
void GraphicsModule::readSettings()
{
	//get settings from config file
	width = static_cast<float>(m_core->getConfigManager().getInt("graphics.window.width"));
	height = static_cast<float>(m_core->getConfigManager().getInt("graphics.window.height"));
	m_multisample = m_core->getConfigManager().getBool("graphics.window.msaa");

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
	m_irradiance_map_resx = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.ibl.generating.diffuse.resx"));
	m_irradiance_map_resy = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.ibl.generating.diffuse.resy"));
	m_irradiance_sample_delta = static_cast<float>(m_core->getConfigManager().getFloat("graphics.lighting.ibl.generating.diffuse.sample_delta"));
	m_specular_map_resx = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.ibl.generating.specular.resx"));
	m_specular_map_resy = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.ibl.generating.specular.resy"));
	m_specular_mipmap_levels = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.ibl.generating.specular.mipmap_levels"));
	m_specular_samples = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.ibl.generating.specular.nsamples"));
	m_specular_brdf_resx = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.ibl.generating.brdf.resx"));
	m_specular_brdf_resy = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.ibl.generating.brdf.resy"));
	m_brdfsamples = static_cast<int>(m_core->getConfigManager().getInt("graphics.lighting.ibl.generating.brdf.nsamples"));

	//shadow settings
	m_shadows = m_core->getConfigManager().getBool("graphics.lighting.shadows.enable_shadows");
	/*m_shadow_res_x = m_core->getConfigManager().getInt("graphics.lighting.shadows.res.x");
	m_shadow_res_y = m_core->getConfigManager().getInt("graphics.lighting.shadows.res.y");
	m_shadow_blur_passes = m_core->getConfigManager().getInt("graphics.lighting.shadows.blur_passes");
	m_shadow_variance_bias = static_cast<float>(m_core->getConfigManager().getFloat("graphics.lighting.shadows.variance_bias"));
	m_light_bleed_reduction = static_cast<float>(m_core->getConfigManager().getFloat("graphics.lighting.shadows.light_bleed_reduction"));*/

	//env map
	m_display_envmap = m_core->getConfigManager().getBool("graphics.envmap.display");
	m_envmap_type = m_core->getConfigManager().getInt("graphics.envmap.type");
	m_envcuberes = m_core->getConfigManager().getInt("graphics.envmap.conversion_resxy");
	if(m_envmap_type == 0)
		m_envmap_hdr = m_core->getConfigManager().getBool("graphics.envmap.texCube.hdr");
	else
		m_envmap_hdr = m_core->getConfigManager().getBool("graphics.envmap.texEr.hdr");

	//debug settings
	m_debug_bvs = m_core->getConfigManager().getBool("graphics.debug.draw_bounding_volumes");
}
void GraphicsModule::prepareAssets()
{
	//load envmap if m_ibl or m_display_envmap is true
	if (m_display_envmap || m_ibl)
	{
		if (m_envmap_type == 0)
		{
			if (m_envmap_hdr)
			{
				m_cube_envmap = GLUtils::loadGLCubeTextureHDR(
					m_core->getConfigManager().getString("graphics.envmap.texCube.px"),
					m_core->getConfigManager().getString("graphics.envmap.texCube.nx"),
					m_core->getConfigManager().getString("graphics.envmap.texCube.py"),
					m_core->getConfigManager().getString("graphics.envmap.texCube.ny"),
					m_core->getConfigManager().getString("graphics.envmap.texCube.pz"),
					m_core->getConfigManager().getString("graphics.envmap.texCube.nz"),
					true,
					true
				);
				m_cube_envmap->setTexParams(GL_LINEAR_MIPMAP_LINEAR,
											GL_LINEAR,
											GL_CLAMP_TO_EDGE,
											GL_CLAMP_TO_EDGE,
											GL_CLAMP_TO_EDGE,
											m_mtexMaxAnisoLevel);
				glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			}
			else
			{
				m_cube_envmap = GLUtils::loadGLCubeTexture(
					m_core->getConfigManager().getString("graphics.envmap.texCube.px"),
					m_core->getConfigManager().getString("graphics.envmap.texCube.nx"),
					m_core->getConfigManager().getString("graphics.envmap.texCube.py"),
					m_core->getConfigManager().getString("graphics.envmap.texCube.ny"),
					m_core->getConfigManager().getString("graphics.envmap.texCube.pz"),
					m_core->getConfigManager().getString("graphics.envmap.texCube.nz"),
					true
				);
				m_cube_envmap->setTexParams(GL_LINEAR_MIPMAP_LINEAR,
											GL_LINEAR,
											GL_CLAMP_TO_EDGE,
											GL_CLAMP_TO_EDGE,
											GL_CLAMP_TO_EDGE,
											m_mtexMaxAnisoLevel);
			}
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		}
		else
		{
			if (m_envmap_hdr)
			{
				m_er_envmap = GLUtils::loadGLTextureHDR(
					m_core->getConfigManager().getString("graphics.envmap.texEr.map"),
					true,
					true
				);
				m_er_envmap->setTexParams(GL_LINEAR_MIPMAP_LINEAR,
										  GL_LINEAR,
										  GL_CLAMP_TO_EDGE,
										  GL_CLAMP_TO_EDGE,
										  m_mtexMaxAnisoLevel);
			}
			else
			{
				m_er_envmap = GLUtils::loadGLTexture(
					m_core->getConfigManager().getString("graphics.envmap.texEr.map"),
					true
				);
				m_er_envmap->setTexParams(GL_LINEAR_MIPMAP_LINEAR,
										  GL_LINEAR,
										  GL_CLAMP_TO_EDGE,
										  GL_CLAMP_TO_EDGE,
										  m_mtexMaxAnisoLevel);
			}
			convertEnvMap();
			//m_er_envmap.reset();
		}
	}
	//TODO: render ibl maps here
	if (m_ibl)
	{
		std::cout << "Rendering diffuse ibl map...\n";
		renderIBLMaps();
		std::cout << "done!\n";
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
void GraphicsModule::convertEnvMap()
{
	m_fb_envconv->bind(GL_FRAMEBUFFER); GLERR
	glDisable(GL_DEPTH_TEST); GLERR
	glViewport(0, 0, m_envcuberes, m_envcuberes); GLERR
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GLERR
	glClear(GL_COLOR_BUFFER_BIT); GLERR
	m_s_envconv->use();	GLERR
	//generate layer matrices
	glm::mat4 pmat = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 layermats[] = {  //px, nx, py, ny, pz, nz
		//due weird opengl cube map sampling, we're rendering x and z faces rotated
		pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
		pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
		pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
		pmat * glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
	};
	for (size_t i = 0; i < 6; i++)
		m_s_envconv->setUniform(("u_layer_matrices[" + std::to_string(i) + "]").c_str(), layermats[i], false); GLERR
	//set envmap and sample settings
	m_s_envconv->bindTex("u_enver", m_er_envmap.get()); GLERR
	Primitives::drawNDCCube(); GLERR
	glFinish(); GLERR
	m_fb_envconv->unbind(GL_FRAMEBUFFER); GLERR
	m_cube_envmap = m_fb_envconv->rtset.colorTargets[0].ctex;
	m_cube_envmap->bind();
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);		
	m_cube_envmap->setTexParams(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, m_mtexMaxAnisoLevel);
	m_cube_envmap->unbind();
	m_fb_envconv.reset();
	m_er_envmap->unbind();
	m_er_envmap.reset();	
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
	if (m_multisample)
	{
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, static_cast<int>(m_core->getConfigManager().getInt("graphics.window.msaa_samples")));
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
	auto fsetting = m_core->getConfigManager().getInt("graphics.window.fullscreen");
	if(fsetting)
		SDL_SetWindowFullscreen(window, fsetting == 1 ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN);
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
	std::vector<ipengine::ipid> parents;
	for (auto eid : activeentitynames)
	{
		auto findent = entities.find(eid);
		if (findent != entities.end())
		{
			auto mO = findent->second;
			for (auto momats : mO->m_mesheObjects->meshtomaterial)
			{
				auto momat = m_scm->getMaterialById(momats.second);
				for (auto texts : momat->m_textures)
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
			}
			for (auto mesh : mO->m_mesheObjects->m_meshes)
			{
				//TODO Check for NULL material
				
				if (m_scmmeshtovao.count(mesh->m_meshId)<1)
				{
					auto vao = (mesh->m_dynamic ? GLUtils::createDynamicVAO(*mesh) : GLUtils::createVAO(*mesh));
					m_scmmeshtovao[mesh->m_meshId] = vao;
				}
				else if (mesh->m_dynamic && mesh->m_dirty)//update dynamic meshes
				{
					if (m_scmmeshtovao.count(mesh->m_meshId) > 0)
					{
						auto& vao = m_scmmeshtovao[mesh->m_meshId];
						mesh->updateNormals();
						mesh->updateTangents();
						GLUtils::updateVAO(vao, *mesh);
					}
				}
				if (mO->m_mesheObjects->meshtomaterial.size()>0 && m_scmshadertoprogram.count(m_scm->getMaterialById(mO->m_mesheObjects->meshtomaterial[mesh->m_meshId])->m_shaderId) < 1)
				{
					auto files = m_scm->getShaderById(m_scm->getMaterialById(mO->m_mesheObjects->meshtomaterial[mesh->m_meshId])->m_shaderId);
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
			//auto transmatrix = parentInfluencedTransform(cameraentity);//cent->m_transformData.getData();
			//SCM::TransformData transdata;
			//
			//
			//transdata.m_rotation = glm::quat_cast(transmatrix);
			//transdata.m_location = glm::vec3(transmatrix[3][0], transmatrix[3][1], transmatrix[3][2]);
			//transdata.m_isMatrixDirty = true;
			//transdata.updateTransform();
			viewmat = ViewFromTransData(cent->m_transformData);//glm::inverse(transdata->m_transformMatrix);//glm::toMat4(transdata->m_rotation) * translate(glm::mat4(1.0f), -transdata->m_location);
		}
	}
	shader->setUniform("u_view_matrix", viewmat, false);
	shader->setUniform("u_projection_matrix", projmat, false);
	shader->setUniform("u_toneMappingExposure", m_exposure);
	setLightUniforms(shader);

	//quick test. remove that shit!
	/*shader->setUniform("u_directionalLights[0].color", glm::vec3(3.0f));
	shader->setUniform("u_directionalLights[0].direction", glm::mat3(viewmat) * glm::vec3(-1.0f, -1.0f, -1.0f));
	shader->setUniform("u_dirLightCount", 1);*/
}
void GraphicsModule::setLightUniforms(ShaderProgram* shader)
{
	auto& dirlights = m_scm->getDirLights();
	auto& pointlights = m_scm->getPointLights();
	auto& spotlights = m_scm->getSpotLights();

	int lc = 0;
	//somehow store shadow stuff in light structs and set that too
	for (auto& dl : dirlights)
	{
		if (m_shadows)
		{
			//TODO: fix for non shadow casting dir lights
			if (dl.second->castShadows)
			{
				shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].color").c_str(), dl.second->m_color);
				shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].direction").c_str(), dl.second->getVSDirection(viewmat));
				shader->setUniform(("u_light_matrix[" + std::to_string(lc) + "]").c_str(), m_dirLightMatrices[dl.second->m_entityId], false);
				shader->bindTex(("u_directionalLights[" + std::to_string(lc) + "].shadowMap").c_str(),
					dl.second->shadowBlurPasses > 0 ? m_dirLightShadowBlurTargets2[dl.second->m_entityId].colorTargets[0].tex.get() : m_dirLightShadowTargets[dl.second->m_entityId].colorTargets[0].tex.get());
				shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].shadowVarianceBias").c_str(), dl.second->shadowVarianceBias);
				shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].lightBleedReduction").c_str(), dl.second->lightBleedReduction);
				shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].shadowWarpFactor").c_str(), dl.second->shadowWarpFactor);
				shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].shadowWarpFactor").c_str(), dl.second->shadowWarpFactor);
				shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].enableShadows").c_str(), dl.second->castShadows);
			}
			else
			{
				shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].color").c_str(), dl.second->m_color);
				shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].direction").c_str(), dl.second->getVSDirection(viewmat));
				shader->occupyTex(("u_directionalLights[" + std::to_string(lc) + "].shadowMap").c_str());
				shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].enableShadows").c_str(), dl.second->castShadows);
			}
			++lc;
		}
		else
		{
			shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].color").c_str(), dl.second->m_color);
			shader->setUniform(("u_directionalLights[" + std::to_string(lc) + "].direction").c_str(), dl.second->getVSDirection(viewmat));
			++lc;
		}
		if (lc >= m_max_dirlights)
			break;
	}
	shader->setUniform("u_dirLightCount", lc);
	if(m_shadows)
		shader->setUniform("u_num_light_matrices", lc);

	lc = 0;
	for (auto pl : pointlights)
	{
		shader->setUniform(("u_pointLights[" + std::to_string(lc) + "].color").c_str(), pl.second->m_color);
		shader->setUniform(("u_pointLights[" + std::to_string(lc) + "].position").c_str(), pl.second->getVSPosition(viewmat));
		shader->setUniform(("u_pointLights[" + std::to_string(lc) + "].max_range").c_str(), pl.second->m_range);
		++lc;
		if (lc >= m_max_pointlights)
			break;
	}
	shader->setUniform("u_pointLightCount", lc);

	lc = 0;
	for (auto sl : spotlights)
	{
		shader->setUniform(("u_spotLights[" + std::to_string(lc) + "].color").c_str(), sl.second->m_color);
		shader->setUniform(("u_spotLights[" + std::to_string(lc) + "].position").c_str(), sl.second->getVSPosition(viewmat));
		shader->setUniform(("u_spotLights[" + std::to_string(lc) + "].max_range").c_str(), sl.second->m_range);
		shader->setUniform(("u_spotLights[" + std::to_string(lc) + "].direction").c_str(), sl.second->getVSDirection(viewmat));
		shader->setUniform(("u_spotLights[" + std::to_string(lc) + "].outer_cone_angle").c_str(), sl.second->m_outerConeAngle);
		shader->setUniform(("u_spotLights[" + std::to_string(lc) + "].inner_cone_angle").c_str(), sl.second->m_innerConeAngle);
		++lc;
		if (lc >= m_max_spotlights)
			break;
	}
	shader->setUniform("u_spotLightCount", lc);

	shader->setUniform("u_ambientLight", m_ambientLight);

	//ibl stuff
	if (m_ibl)
	{
		shader->setUniform("u_diffuseibl", m_ibldiffuse);
		shader->setUniform("u_specularibl", m_iblspecular);
	
		//do bind those textures, even though they're empty.
		//some opengl implementations complain about inconsistent sampler types at this point,
		//because per default unused samplers are initialized to sample texture unit 0
		shader->bindTex("u_irradianceMap", m_ot_irradiance.get());
		shader->bindTex("u_prefilterMap", m_ot_specularradiance.get());
		shader->bindTex("u_brdfLUT", m_ot_brdfresponse.get());

		if (m_iblspecular)
			shader->setUniform("u_ibl_maxspeclod", static_cast<GLfloat>(m_specular_mipmap_levels - 1));
	}
}
void GraphicsModule::setMaterialUniforms(SCM::MaterialData * mdata, ShaderProgram* shader, const glm::vec2& tcoffset, const glm::vec2& tcscale)
{
	for (auto tp : mdata->m_textures)
	{
		auto& t = tp.second;
		auto& tex = m_scmtexturetot2d[t.m_texturefileId];
		shader->bindTex(("u_material." + tp.first + ".tex").c_str(), tex.get());
		shader->setUniform(("u_material." + tp.first + ".scale").c_str(), tcscale);// t.m_size);
		shader->setUniform(("u_material." + tp.first + ".offset").c_str(), tcoffset);// t.m_offset);
	}
}
void GraphicsModule::drawEntity(SCM::ThreeDimEntity * entity, ShaderProgram* shader)
{
	//set per entity uniforms
	const glm::mat4& transformMat = entity->m_transformData.getLocalToWorldMatrix();//parentInfluencedTransform(entity->m_entityId);// = entity->m_transformData.getData()->m_transformMatrix;

	////add parent transforms
	//std::stack<SCM::Entity*> parentents;
	//parentents.push(entity);
	//SCM::Entity* parentent;
	//while (parentents.top() && parentents.top()->m_parent)
	//{
	//	parentents.push(parentents.top()->m_parent);
	//	//transformMat = parentent->m_transformData.getData()->m_transformMatrix* transformMat;
	//	//parentent = parentent->m_parent;
	//}
	//transformMat = parentents.top()->m_transformData.getData()->m_transformMatrix;
	//parentents.pop();
	//while (!parentents.empty())
	//{
	//	transformMat = transformMat * parentents.top()->m_transformData.getData()->m_transformMatrix;
	//	parentents.pop();
	//}


	
	shader->setUniform("u_model_matrix", transformMat, false);	
	//draw all meshes
	
	for (auto m : entity->m_mesheObjects->m_meshes)
	{
		//TODO:: optimization! create batches of meshes with the same material
		auto ctu = shader->getCurrentTU();
		if (entity->m_mesheObjects->meshtomaterial.count(m->m_meshId) > 0)
		{
			auto mat = m_scm->getMaterialById(entity->m_mesheObjects->meshtomaterial[m->m_meshId]);
			setMaterialUniforms(mat, shader);//m->m_texCoordOffset, m->m_texCoordScale);
		}
		if (m->m_isdoublesided)
			glDisable(GL_CULL_FACE);
		else
			glEnable(GL_CULL_FACE);
		drawSCMMesh(m->m_meshId);
		shader->resetTU(ctu);
	}
}
void GraphicsModule::drawEntityShadow(SCM::ThreeDimEntity * entity, ShaderProgram* shader)
{
	//set per entity uniforms
	//const glm::mat4& transformMat = entity->m_transformData.getData()->m_transformMatrix;
	const glm::mat4& transformMat = entity->m_transformData.getLocalToWorldMatrix();//parentInfluencedTransform(entity->m_entityId);
	shader->setUniform("u_model_matrix", transformMat, false);

	//draw all meshes
	for (auto m : entity->m_mesheObjects->m_meshes)
	{
		if (m->m_isdoublesided)
			glDisable(GL_CULL_FACE);
		else
			glEnable(GL_CULL_FACE);
		drawSCMMesh(m->m_meshId);
	}
}
void GraphicsModule::renderDirectionalLightShadowMap(SCM::DirectionalLight& dirLight)
{
	if (m_dirLightShadowTargets.find(dirLight.m_entityId) == m_dirLightShadowTargets.end())
	{
		//create rendertargetset for this light
		FrameBufferDesc sfbd{
			{
				RenderTargetDesc{
					dirLight.shadowResX,
					dirLight.shadowResY,
					GL_RGBA32F,
					GL_COLOR_ATTACHMENT0,
					RenderTargetType::Texture2D
				}
			},
			RenderTargetDesc{
				dirLight.shadowResX,
				dirLight.shadowResY,
				GL_DEPTH24_STENCIL8,
				GL_DEPTH_STENCIL_ATTACHMENT,
				RenderTargetType::RenderBuffer
			}
		};
		m_dirLightShadowTargets[dirLight.m_entityId] = GLUtils::createRenderTargetSet(sfbd);
		m_dirLightShadowTargets[dirLight.m_entityId].colorTargets[0].tex->setBorderColor(
			std::numeric_limits<GLfloat>::infinity(),
			std::numeric_limits<GLfloat>::infinity(),			
			0.0f,
			0.0f
		);
		m_dirLightShadowTargets[dirLight.m_entityId].colorTargets[0].tex->setTexParams(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, m_mtexAniso ? m_mtexMaxAnisoLevel : 0);
	}
	if (m_dirLightMatrices.find(dirLight.m_entityId) == m_dirLightMatrices.end())
	{
		glm::mat4 lightView;
		glm::mat4 lightProj;
		lightMatDirectionalLight(lightView, lightProj, dirLight);
		auto dirLightMat = lightProj * lightView;
		m_dirLightMatrices[dirLight.m_entityId] = dirLightMat;
	}
	
	//render scene into shadow map
	m_fb_shadow->bind(GL_FRAMEBUFFER);
	m_fb_shadow->attachRenderTargetSet(m_dirLightShadowTargets[dirLight.m_entityId]);
	glClearColor(
		std::numeric_limits<GLfloat>::infinity(),
		std::numeric_limits<GLfloat>::infinity(),
		0.0f,
		0.0f
	);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, dirLight.shadowResX, dirLight.shadowResY);
	m_s_shadow->use();
	m_s_shadow->setUniform("u_shadowWarpFactor", dirLight.shadowWarpFactor);
	m_s_shadow->setUniform("u_light_matrix", m_dirLightMatrices[dirLight.m_entityId], false);
	drawSceneShadow(m_s_shadow.get());
	if (dirLight.shadowBlurPasses <= 0)
	{
		m_fb_shadow->unbind(GL_FRAMEBUFFER);
		setDefaultGLState();
		return;
	}
	//blur shadow map
	if (m_dirLightShadowBlurTargets1.find(dirLight.m_entityId) == m_dirLightShadowBlurTargets1.end())
	{
		//create blur rendertargetset
		FrameBufferDesc sfbd{
			{
				RenderTargetDesc{
					dirLight.shadowResX,
					dirLight.shadowResY,
					GL_RGBA32F,
					GL_COLOR_ATTACHMENT0,
					RenderTargetType::Texture2D
				}
			},
		};
		m_dirLightShadowBlurTargets1[dirLight.m_entityId] = GLUtils::createRenderTargetSet(sfbd);
		m_dirLightShadowBlurTargets1[dirLight.m_entityId].colorTargets[0].tex->setBorderColor(
			std::numeric_limits<GLfloat>::infinity(),
			std::numeric_limits<GLfloat>::infinity(),
			0.0f,
			0.0f
		);
		m_dirLightShadowBlurTargets1[dirLight.m_entityId].colorTargets[0].tex->setTexParams(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, m_mtexAniso ? m_mtexMaxAnisoLevel : 0);
	}

	if (m_dirLightShadowBlurTargets2.find(dirLight.m_entityId) == m_dirLightShadowBlurTargets2.end())
	{
		//create blur rendertargetset
		FrameBufferDesc sfbd{
			{
				RenderTargetDesc{
					dirLight.shadowResX,
					dirLight.shadowResY,
					GL_RGBA32F,
					GL_COLOR_ATTACHMENT0,
					RenderTargetType::Texture2D
				}
			},
		};
		m_dirLightShadowBlurTargets2[dirLight.m_entityId] = GLUtils::createRenderTargetSet(sfbd);
		m_dirLightShadowBlurTargets2[dirLight.m_entityId].colorTargets[0].tex->setBorderColor(
			std::numeric_limits<GLfloat>::infinity(),
			std::numeric_limits<GLfloat>::infinity(),
			0.0f,
			0.0f
		);
		m_dirLightShadowBlurTargets2[dirLight.m_entityId].colorTargets[0].tex->setTexParams(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, m_mtexAniso ? m_mtexMaxAnisoLevel : 0);
	}

	m_s_gblur->use();
	glDisable(GL_DEPTH_TEST);

	
	//attach rendertargetset from shadow render pass but deactivate depth target
	auto& blurRT1 = m_dirLightShadowBlurTargets1[dirLight.m_entityId];
	auto& blurRT2 = m_dirLightShadowBlurTargets2[dirLight.m_entityId];
	m_fb_gblur1->bind(GL_FRAMEBUFFER);
	m_fb_gblur1->attachRenderTargetSet(blurRT1);
	m_fb_gblur2->bind(GL_FRAMEBUFFER);
	m_fb_gblur2->attachRenderTargetSet(blurRT2);
	int currentFB = 1;
	for (int i = 0; i < dirLight.shadowBlurPasses; ++i)
	{
		//blur horizontal
		m_fb_gblur1->bind(GL_FRAMEBUFFER);		
		glClear(GL_COLOR_BUFFER_BIT);
		if (i == 0)		
			m_fb_shadow->rtset.colorTargets[0].tex->bind(0);
		else		
			m_fb_gblur2->rtset.colorTargets[0].tex->bind(0);
		m_s_gblur->setUniform("u_input", 0);
		m_s_gblur->setUniform("u_horizontal", 1);
		Primitives::drawNDCQuad();
		//blur vertical
		m_fb_gblur2->bind(GL_FRAMEBUFFER);		
		glClear(GL_COLOR_BUFFER_BIT);
		m_fb_gblur1->rtset.colorTargets[0].tex->bind(0);
		m_s_gblur->setUniform("u_input", 0);
		m_s_gblur->setUniform("u_horizontal", 0);
		Primitives::drawNDCQuad();
	}
	m_fb_gblur2->unbind(GL_FRAMEBUFFER);
	setDefaultGLState();
	GLERR
}
void GraphicsModule::renderEnvMap()
{	
	if (m_display_envmap)
	{
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);		
		m_s_skybox->use();
		m_s_skybox->bindTex("u_skybox", m_cube_envmap.get());
		m_s_skybox->setUniform("u_view_matrix", glm::mat4(glm::mat3(viewmat)), false);
		m_s_skybox->setUniform("u_projection_matrix", projmat, false);
		Primitives::drawNDCCube();
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}	
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
	if (m_scmmeshtovao.count(meshid) < 1)
		return;
	auto vao = m_scmmeshtovao[meshid];
	if (vao && vao->vao != 0)
	{
		glBindVertexArray(vao->vao);GLERR
		glDrawElements(GL_TRIANGLES, vao->indexCount, GL_UNSIGNED_INT, 0);GLERR
		glBindVertexArray(0);GLERR
	}
}
glm::mat4 GraphicsModule::ViewFromTransData(SCM::Transform &transform)
{

	/*	x y z					negative translation	x	y	z
	|x.x	x.y		x.z		-cp.x| camera x achse	1	0	0
	|y.x	y.y		y.z		-cp.y| camera y achse	0	1	0
	|z.x	z.y		z.z		-cp.z| camera z achse   0	0	1
	|0		0		0		1    | */
	//transform.GetTransformMat();
	glm::vec3 pos = transform.getWorldPosition();
	glm::mat4 viewRot = glm::mat4_cast(glm::inverse(transform.getWorldRotation()));
		
	glm::mat4 viewTr = glm::mat4(
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(-pos.x, -pos.y, -pos.z, 1));

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
void GraphicsModule::lightMatDirectionalLight(glm::mat4& view, glm::mat4& proj, SCM::DirectionalLight& dirLight)
{
	//hmm rework later
	//dirLight.m_transformData.setData()->updateTransform();
	proj = glm::ortho(dirLight.shadowMapVolumeMin.x, dirLight.shadowMapVolumeMax.x, dirLight.shadowMapVolumeMin.y, dirLight.shadowMapVolumeMax.y, dirLight.shadowMapVolumeMax.z, dirLight.shadowMapVolumeMin.z);// dirLight.shadowMapVolumeMax.z, dirLight.shadowMapVolumeMin.z);
	view = ViewFromTransData(dirLight.m_transformData);
}
void GraphicsModule::renderBoundingVolumes()
{
	m_s_bvdebug->use();

	if (cameraentity != IPID_INVALID)
	{
		auto cent = m_scm->getEntityById(cameraentity);
		if (cent != nullptr)
		{
			//auto transmatrix = parentInfluencedTransform(cameraentity);//cent->m_transformData.getData();
			//SCM::TransformData transdata;


			//transdata.m_rotation = glm::quat_cast(transmatrix);
			//transdata.m_location = glm::vec3(transmatrix[3][0], transmatrix[3][1], transmatrix[3][2]);
			//transdata.m_isMatrixDirty = true;
			//transdata.updateTransform();
			viewmat = ViewFromTransData(cent->m_transformData);//glm::inverse(transdata->m_transformMatrix);//glm::toMat4(transdata->m_rotation) * translate(glm::mat4(1.0f), -transdata->m_location);
		}
	}
	m_s_bvdebug->setUniform("u_view_matrix", viewmat, false);
	m_s_bvdebug->setUniform("u_projection_matrix", projmat, false);

	auto& drawableEntities = m_scm->getThreeDimEntities();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	for (auto ep : drawableEntities)
	{
		auto entity = ep.second;
		if (!entity->isActive)
			continue;
		drawEntityBV(entity, m_s_bvdebug.get());
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
}
void GraphicsModule::drawEntityBV(SCM::ThreeDimEntity * entity, ShaderProgram * shader)
{
	//set per entity uniforms
	//const glm::mat4& transformMat = entity->m_transformData.getData()->m_transformMatrix;
	if (!entity->shouldCollide())
		return;	
	
	if (entity->isBoundingBox)
	{
		shader->setUniform("u_model_matrix", entity->m_boundingData.box.bdtoworld, false);
		Primitives::drawNDCCube();
	}
	else
	{
		shader->setUniform("u_model_matrix", entity->m_boundingData.sphere.bdtoworld, false);
		Primitives::drawNDCSphere();
	}
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

//glm::mat4 GraphicsModule::parentInfluencedTransform(ipengine::ipid childid)
//{
//	SCM::Entity * entity = m_scm->getEntityById(childid);
//	//set per entity uniforms
//	glm::mat4 transformMat;// = entity->m_transformData.getData()->m_transformMatrix;
//
//						   //add parent transforms
//	std::stack<SCM::Entity*> parentents;
//	parentents.push(entity);
//	SCM::Entity* parentent;
//	while (parentents.top() && parentents.top()->m_parent)
//	{
//		parentents.push(parentents.top()->m_parent);
//		//transformMat = parentent->m_transformData.getData()->m_transformMatrix* transformMat;
//		//parentent = parentent->m_parent;
//	}
//	if (parentents.top()->m_transformData.getData()->m_isMatrixDirty)
//		parentents.top()->m_transformData.setData()->calcTransformMatrix();
//	transformMat = parentents.top()->m_transformData.getData()->m_transformMatrix;
//	parentents.pop();
//	while (!parentents.empty())
//	{
//		if (parentents.top()->m_transformData.getData()->m_isMatrixDirty)
//			parentents.top()->m_transformData.setData()->calcTransformMatrix();
//		transformMat = transformMat * parentents.top()->m_transformData.getData()->m_transformMatrix;
//		parentents.pop();
//	}
//	/*if (entity->m_parent)
//		return  entity->m_parent->m_transformData.getData()->m_transformMatrix*entity->m_transformData.getData()->m_transformMatrix;*/
//	return transformMat;
//}

//glm::mat4 GraphicsModule::parentInfluencedView(ipengine::ipid childid)
//{
//	SCM::Entity * entity = m_scm->getEntityById(childid);
//	//set per entity uniforms
//	glm::mat4 transformMat;// = entity->m_transformData.getData()->m_transformMatrix;
//
//						   //add parent transforms
//	std::stack<SCM::Entity*> parentents;
//	parentents.push(entity);
//	SCM::Entity* parentent;
//	while (parentents.top() && parentents.top()->m_parent)
//	{
//		parentents.push(parentents.top()->m_parent);
//		//transformMat = parentent->m_transformData.getData()->m_transformMatrix* transformMat;
//		//parentent = parentent->m_parent;
//	}
//	transformMat = ViewFromTransData(parentents.top()->m_transformData.getData());// ->m_transformMatrix;
//	parentents.pop();
//	while (!parentents.empty())
//	{
//		transformMat = transformMat * ViewFromTransData(parentents.top()->m_transformData.getData());// ->m_transformMatrix;
//		parentents.pop();
//	}
//	return transformMat;
//}



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