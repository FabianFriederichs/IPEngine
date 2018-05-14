// Plugin2.cpp : Defines the exported functions for the DLL application.
//
#define USE_GLTF
#define TINYGLTF_LOADER_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#include "VulkanRenderer.h"
#include <stb_image.h>
#ifdef USE_GLTF
std::string GLTF_VERSION;
std::string GLTF_NAME;
#endif

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
VulkanRenderer::VulkanRenderer()
{
	m_info.identifier = "VulkanRenderer";
	m_info.version = "1.0";
	m_info.iam = "IModule_API.IGraphics_API"; //Change this to your used API
	return;
}

void VulkanRenderer::updateData()
{
	setCameraEntity(m_entrepcam);

	auto& activeentitynames = getActiveEntityNames(*m_scm);
	auto& entities = m_scm->getThreeDimEntities();
	auto& shaders = m_scm->getShaders();
	auto& textures = m_scm->getTextures();
	std::vector<ipengine::ipid> parents;
	auto scene = m_renderer->getScene();

	int modelsadded = 0;
	for (auto eid : activeentitynames)
	{
		auto findent = entities.find(eid);
		if (findent != entities.end())
		{
			auto mO = findent->second;
			
			for (auto mesh : mO->m_mesheObjects->m_meshes)
			{
				//TODO Check for NULL material

				if (scene->meshes.count(mesh->m_meshId)<1)
				{
					//Dynamic VAO's vulkan TODO IMPORTANT!!!
					//auto vao = (mesh->m_dynamic ? GLUtils::createDynamicVAO(*mesh) : GLUtils::createVAO(*mesh));
					//m_scmmeshtovao[mesh->m_meshId] = vao;
					auto verts = scmVertsToVVertex(mesh->m_vertices);
					glm::vec3 minp, maxp;
					minp = glm::vec3(std::numeric_limits<float>::max());
					maxp = glm::vec3(-std::numeric_limits<float>::max());
					for (auto v : verts)
					{
						minp = glm::min(minp, v.pos);
						maxp = glm::max(maxp, v.pos);
					}
					scene->meshes.insert({ mesh->m_meshId, m_renderer->getManager() });
					scene->meshes[mesh->m_meshId].load(verts, mesh->m_indices, minp, maxp);
					scene->meshes[mesh->m_meshId].uPerModelInfo = reinterpret_cast<PerModelUniformBuffer *>(m_renderer->getUniformBlob().alloc(sizeof(PerModelUniformBuffer)));
					modelsadded++;
				}
				for (auto momats : mO->m_mesheObjects->meshtomaterial)
				{
					auto momat = m_scm->getMaterialById(momats.second);
					for (auto texts : momat->m_textures)
					{
						if (m_scmtexturetot2d.count(texts.second.m_texturefileId) < 1)
						{
							auto texfile = m_scm->getTextureById(texts.second.m_texturefileId);

							if (texts.first == "mrar")
							{
								//split mrar texture into 3 channels
								rj::ImageWrapper imgwrap, imgwrapmet, imgwrapro, imgwrapao;
								int width;
								int height;
								int channels;
								//stbi__vertically_flip_on_load = true;
								unsigned char* image = stbi_load(texfile->m_path.c_str(), &width, &height, &channels, 4);
								std::vector<uint8_t> metmap(width*height*channels);
								std::vector<uint8_t> roughmap(width*height*channels);
								std::vector<uint8_t> aomap(width*height*channels);
								for (size_t index = 0; index < width*height*4; index += 4)
								{
									for (int ix2 = 0; ix2 < 4; ++ix2)
									{
										metmap.push_back(image[index]);
										roughmap.push_back(image[index + 1]);
										aomap.push_back(image[index + 2]);
									}
								} 
								channels = 4;
								auto format = channels == 4 ? gli::FORMAT_RGBA8_UNORM_PACK8 : gli::FORMAT_RGB8_UNORM_PACK8;
								rj::loadTexture2DFromBinaryData(&imgwrapmet, m_renderer->getManager(), metmap.data(), width, height, format);
								rj::loadTexture2DFromBinaryData(&imgwrapro, m_renderer->getManager(), roughmap.data(), width, height, format);
								rj::loadTexture2DFromBinaryData(&imgwrapao, m_renderer->getManager(), aomap.data(), width, height, format);
								m_scmtextomrart2d[texts.second.m_texturefileId]["metalness"] = imgwrapmet;
								m_scmtextomrart2d[texts.second.m_texturefileId]["roughness"] = imgwrapmet;
								m_scmtextomrart2d[texts.second.m_texturefileId]["ao"] = imgwrapmet;
								rj::loadTexture2DFromBinaryData(&imgwrap, m_renderer->getManager(), image, width, height, format);

								m_scmtexturetot2d[texts.second.m_texturefileId] = imgwrap;
							}
							else
							{
								rj::ImageWrapper imgwrap;
								int width;
								int height;
								int channels;
								//stbi__vertically_flip_on_load = true;
								unsigned char* image = stbi_load(texfile->m_path.c_str(), &width, &height, &channels, 4);
								rj::loadTexture2DFromBinaryData(&imgwrap, m_renderer->getManager(), image, width, height, gli::FORMAT_RGBA8_UNORM_PACK8);
								stbi_image_free(image);
								m_scmtexturetot2d[texts.second.m_texturefileId] = imgwrap;
							}
							
							//GLUtils::loadGLTexture(textures[])
							//TODO
							//load texture into gpu memory?? 
						}
					}
					if (scene->meshes.count(momats.first)>0)
					{
						auto& mesh = scene->meshes[momats.first];//m_scmtexturetot2d[mat.]

						if (mesh.albedoMap.imageViews.size() == 0 && momat->m_textures.count("albedo")>0 && m_scmtexturetot2d.count(momat->m_textures["albedo"].m_texturefileId)>0)
						{
							mesh.albedoMap = m_scmtexturetot2d[momat->m_textures["albedo"].m_texturefileId];
						}
						if (mesh.emissiveMap.imageViews.size() == 0 && momat->m_textures.count("emissive")>0 && m_scmtexturetot2d.count(momat->m_textures["emissive"].m_texturefileId)>0)
						{
							mesh.emissiveMap = m_scmtexturetot2d[momat->m_textures["emissive"].m_texturefileId];
						}
						if (mesh.normalMap.imageViews.size() == 0 && momat->m_textures.count("normal")>0 && m_scmtexturetot2d.count(momat->m_textures["normal"].m_texturefileId)>0)
						{
							mesh.normalMap = m_scmtexturetot2d[momat->m_textures["normal"].m_texturefileId];
						}
						if (mesh.metalnessMap.imageViews.size() == 0 && momat->m_textures.count("mrar")>0 && m_scmtexturetot2d.count(momat->m_textures["mrar"].m_texturefileId)>0)
						{
							mesh.roughnessMap = m_scmtextomrart2d[momat->m_textures["mrar"].m_texturefileId]["roughness"];
							mesh.metalnessMap = m_scmtextomrart2d[momat->m_textures["mrar"].m_texturefileId]["metalness"];
							mesh.aoMap = m_scmtextomrart2d[momat->m_textures["mrar"].m_texturefileId]["ao"];
						}
					}
				}
				//else if (mesh->m_dynamic && mesh->m_dirty)//update dynamic meshes
				//{
				//	if (m_scmmeshtovao.count(mesh->m_meshId) > 0)
				//	{
				//		auto& vao = m_scmmeshtovao[mesh->m_meshId];
				//		mesh->updateNormals();
				//		mesh->updateTangents();
				//		GLUtils::updateVAO(vao, *mesh);
				//	}
				//}
				
				/*if (mO->m_mesheObjects->meshtomaterial.size()>0 && m_scmshadertoprogram.count(m_scm->getMaterialById(mO->m_mesheObjects->meshtomaterial[mesh->m_meshId])->m_shaderId) < 1)
				{
					auto files = m_scm->getShaderById(m_scm->getMaterialById(mO->m_mesheObjects->meshtomaterial[mesh->m_meshId])->m_shaderId);
					auto prog = GLUtils::createShaderProgram(files->m_shaderFiles[0], files->m_shaderFiles[1]);
					m_scmshadertoprogram[files->m_shaderId] = prog;
				}*/
			}
			/*if (mO->m_transformData.getData()->m_isMatrixDirty)
			{
			mO->m_transformData.setData()->updateTransform();
			}*/
		}
	}
	if (modelsadded > 0)
	{
		m_renderer->recreateSwapChain();
	}
}

bool VulkanRenderer::_startup()
{
#ifdef USE_GLTF
	GLTF_VERSION = "2.0";
	GLTF_NAME = "..//assets//gltf//avocado//avocado.gltf";
#endif
	m_renderer = new DeferredRenderer();
	m_renderer->initialize();

	m_scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>(m_scmID);

	ipengine::Scheduler& sched = m_core->getScheduler();
	handles.push_back(sched.subscribe(ipengine::TaskFunction::make_func<VulkanRenderer, &VulkanRenderer::render>(this),
		0,
		ipengine::Scheduler::SubType::Frame,
		1,
		&m_core->getThreadPool(),
		true)
	);
	//m_renderer->run();
	return true;
}

void VulkanRenderer::render(ipengine::TaskContext & c)
{
	render();
}

bool VulkanRenderer::_shutdown()
{
	m_renderer->shutdown();
	return true;
}

void VulkanRenderer::render()
{
	updateData();
	m_renderer->render();
}

void VulkanRenderer::render(int fbo, int viewportx, int viewporty, bool multisample)
{
	//set stuff
	//...
	m_renderer->render();
}

void VulkanRenderer::setCameraEntity(ipengine::ipid id)
{
	if (id != IPID_INVALID)
	{
		auto ent = m_scm->getEntityById(id);
		if (ent)
		{
			glm::vec3 lookat{ 0,0,0 };
			//calc lookat from entity
			//ent->m_transformData.getData()->updateTransform();
			lookat = ent->m_transformData.getData()->m_location + ent->m_transformData.getData()->m_localZ;
			m_renderer->m_camera = Camera{ ent->m_transformData.getData()->m_location, lookat, m_renderer->m_camera.getFovy(), m_renderer->m_camera.getAspectRatio(), m_renderer->m_camera.getZNear(), m_renderer->m_camera.getZFar(), m_renderer->m_camera.getSegmentCount() };
			m_entrepcam = id;
		}
	}
}

void VulkanRenderer::setFOV(uint32_t fov)
{
	m_renderer->m_camera = Camera{ m_renderer->m_camera.getPosition(), m_renderer->m_camera.getLookAt(), (float)fov, m_renderer->m_camera.getAspectRatio(), m_renderer->m_camera.getZNear(), m_renderer->m_camera.getZFar(), m_renderer->m_camera.getSegmentCount() };
}

void VulkanRenderer::setResolution(uint32_t x, uint32_t y)
{
	m_renderer->m_height = y;
	m_renderer->m_width = x;
}

void VulkanRenderer::setClipRange(float znear, float zfar)
{
	m_renderer->m_camera = Camera{ m_renderer->m_camera.getPosition(), m_renderer->m_camera.getLookAt(), m_renderer->m_camera.getFovy(), m_renderer->m_camera.getAspectRatio(), znear, zfar, m_renderer->m_camera.getSegmentCount() };
}

ipengine::ipid VulkanRenderer::getCameraEntity()
{
	return m_entrepcam;
}

uint32_t VulkanRenderer::getFOV()
{
	//add FOV to camera
	return m_renderer->m_camera.getFovy();
}

void VulkanRenderer::getResolution(uint32_t &width, uint32_t &height)
{
	width = m_renderer->m_width;
	height = m_renderer->m_height;
}

void VulkanRenderer::getClipRange(float &znear, float &zfar)
{
	znear = m_renderer->m_camera.getZNear();
	zfar = m_renderer->m_camera.getZFar();
}

void VulkanRenderer::loadTextureFromMemory(const GrAPI::t2d &, const ipengine::ipid)
{
	//rj::helper_functions::loadTexture2DFromBinaryData();
	//map ipid to vk resource id
}

std::vector<ipengine::ipid> VulkanRenderer::getActiveEntityNames(SCM::ISimpleContentModule_API & scm)
{
	auto& entities = m_scm->getEntities();
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

std::vector<Vertex> VulkanRenderer::scmVertsToVVertex(SCM::VertexVector& verts)
{
	auto data = verts.getData();
	std::vector<Vertex> newverts(data.size());
	for (size_t ix = 0; ix < newverts.size(); ++ix)
	{
		newverts[ix].pos = data[ix].m_position;
		newverts[ix].normal = data[ix].m_normal;
		newverts[ix].texCoord = data[ix].m_uv;
	}
	return newverts;
}
