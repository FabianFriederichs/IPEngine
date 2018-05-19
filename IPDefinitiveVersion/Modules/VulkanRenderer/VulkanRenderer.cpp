// Plugin2.cpp : Defines the exported functions for the DLL application.
//
#define USE_GLTF

#define TINYGLTF_LOADER_IMPLEMENTATION
#define VK_USE_PLATFORM_WIN32_KHR
#define NOMINMAX

// Tell SDL not to mess with main()
#define SDL_MAIN_HANDLED
#include "VulkanRenderer.h"
#ifdef USE_GLTF
std::string GLTF_VERSION;
std::string GLTF_NAME;
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
	//setCameraEntity(m_entrepcam);

	//update camera
	{
		auto cament = m_scm->getEntityById(m_entrepcam);
		if (cament)
		{
			cament->m_transformData.setData()->updateTransform();
			m_renderer->m_camera.forceViewMat(ViewFromTransData(cament->m_transformData.getData()));
			m_renderer->m_camera.setPosition(cament->m_transformData.getData()->m_location);
			m_renderer->m_camera.setLookAt(cament->m_transformData.getData()->m_location + cament->m_transformData.getData()->m_localZ);
		}
	}

	auto& activeentitynames = getActiveEntityNames(*m_scm);
	auto& entities = m_scm->getThreeDimEntities();
	auto& shaders = m_scm->getShaders();
	auto& textures = m_scm->getTextures();
	std::vector<ipengine::ipid> parents;
	auto scene = m_renderer->getScene();

	//load resources currently active 
	for (auto eid : activeentitynames)
	{
		auto findent = entities.find(eid);
		if (findent != entities.end())
		{
			auto mO = findent->second;
			
			for (auto mesh : mO->m_mesheObjects->m_meshes)
			{
				//TODO Check for NULL material

				if (m_allmeshes.count(mesh->m_meshId)<1)
				{
					//Dynamic VAO's vulkan TODO IMPORTANT!!!
					//auto vao = (mesh->m_dynamic ? GLUtils::createDynamicVAO(*mesh) : GLUtils::createVAO(*mesh));
					//m_scmmeshtovao[mesh->m_meshId] = vao;
					int res = -1;
					res = loadMesh(mesh);
					//Update position
					updateVMeshData(mesh->m_meshId, findent->second->m_transformData.getData());
					if (!mesh->m_dynamic)
					{
						
					}
					else
					{
						//Do dyncamic mesh thing
					}
					if (res < 0)
					{
						//Error loading mesh
					}
				}
				else
				{
					//Update position
					updateVMeshData(mesh->m_meshId, findent->second->m_transformData.getData());
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
								rj::ImageWrapper imgwrap;
								int width;
								int height;
								int channels;
								//stbi__vertically_flip_on_load = true;
								unsigned char* image = stbi_load(texfile->m_path.c_str(), &width, &height, &channels, 4);
								std::vector<uint8_t> metmap(width*height*channels);
								std::vector<uint8_t> roughmap(width*height*channels);
								std::vector<uint8_t> aomap(width*height*channels);
#ifdef NDEBUG
								splitChannels(image, width, height, channels, 3, std::vector<std::vector<uint8_t>*>({ &metmap, &roughmap, &aomap }));
#endif
								m_scmtextomrart2d[texts.second.m_texturefileId]["metalness"] = loadTextureBinary(metmap.data(), width, height, channels);
								m_scmtextomrart2d[texts.second.m_texturefileId]["roughness"] = loadTextureBinary(roughmap.data(), width, height, channels);
								m_scmtextomrart2d[texts.second.m_texturefileId]["ao"] = loadTextureBinary(aomap.data(), width, height, channels);
								//rj::loadTexture2DFromBinaryData(&imgwrap, m_renderer->getManager(), image, width, height, format);

								m_scmtexturetot2d[texts.second.m_texturefileId] = imgwrap;
							}
							else
							{
								m_scmtexturetot2d[texts.second.m_texturefileId] = loadTexture(texfile->m_path);
							}
							
							//GLUtils::loadGLTexture(textures[])
							//TODO
							//load texture into gpu memory?? 
						}
					}
					if (m_allmeshes.count(momats.first)>0)
					{
						auto& mesh = m_allmeshes[momats.first];//m_scmtexturetot2d[mat.]

						if (mesh->albedoMap.imageViews.size() == 0 && momat->m_textures.count("albedo")>0 && m_scmtexturetot2d.count(momat->m_textures["albedo"].m_texturefileId)>0)
						{
							mesh->albedoMap = m_scmtexturetot2d[momat->m_textures["albedo"].m_texturefileId];
						}
						if (mesh->emissiveMap.imageViews.size() == 0 && momat->m_textures.count("emissive")>0 && m_scmtexturetot2d.count(momat->m_textures["emissive"].m_texturefileId)>0)
						{
							mesh->emissiveMap = m_scmtexturetot2d[momat->m_textures["emissive"].m_texturefileId];
						}
						if (mesh->normalMap.imageViews.size() == 0 && momat->m_textures.count("normal")>0 && m_scmtexturetot2d.count(momat->m_textures["normal"].m_texturefileId)>0)
						{
							mesh->normalMap = m_scmtexturetot2d[momat->m_textures["normal"].m_texturefileId];
						}
						if (mesh->metalnessMap.imageViews.size() == 0 && momat->m_textures.count("mrar")>0 && m_scmtexturetot2d.count(momat->m_textures["mrar"].m_texturefileId)>0)
						{
							mesh->roughnessMap = m_scmtextomrart2d[momat->m_textures["mrar"].m_texturefileId]["roughness"];
							mesh->metalnessMap = m_scmtextomrart2d[momat->m_textures["mrar"].m_texturefileId]["metalness"];
							mesh->aoMap = m_scmtextomrart2d[momat->m_textures["mrar"].m_texturefileId]["ao"];
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

	//Create differences of entity sets to figure out whether command buffers need to be recreated

	auto &setscene = lastactiveentitites;
	auto &setactive = activeentitynames;

	if (differenceInActiveEntities(setactive, setscene))
	{
		lastactiveentitites.clear();
		scene->meshes.clear();
		for (auto id : setactive)
		{
			if (entities.count(id) > 0)
			{
				int mcount = 0;
				for (auto mo : entities[id]->m_mesheObjects->m_meshes)
				{
					if (m_allmeshes.count(mo->m_meshId) > 0)
					{
						scene->meshes[mo->m_meshId] = m_allmeshes[mo->m_meshId];
						mcount++;
					}
				}
				if (mcount > 0)
					lastactiveentitites.push_back(id);
			}
		}
		updateDrawableRenderStates();
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
	//m_renderer->m_camera = Camera{ m_renderer->m_camera.getPosition(), m_renderer->m_camera.getLookAt(), (float)fov, m_renderer->m_camera.getAspectRatio(), m_renderer->m_camera.getZNear(), m_renderer->m_camera.getZFar(), m_renderer->m_camera.getSegmentCount() };
	m_renderer->m_camera.setFov(fov);
}

void VulkanRenderer::setResolution(uint32_t x, uint32_t y)
{
	m_renderer->m_height = y;
	m_renderer->m_width = x;
}

void VulkanRenderer::setClipRange(float znear, float zfar)
{
	//m_renderer->m_camera = Camera{ m_renderer->m_camera.getPosition(), m_renderer->m_camera.getLookAt(), m_renderer->m_camera.getFovy(), m_renderer->m_camera.getAspectRatio(), znear, zfar, m_renderer->m_camera.getSegmentCount() };
	m_renderer->m_camera.setZnear(znear, zfar);
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
	auto& entities = m_scm->getThreeDimEntities();
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

bool VulkanRenderer::differenceInActiveEntities(std::vector<ipengine::ipid> & first, std::vector<ipengine::ipid>& second)
{
	if (first.size() != second.size())
		return true;
	std::sort(first.begin(), first.end());
	std::sort(second.begin(), second.end());
	size_t c = 0;
	for (auto& id : first)
	{
		if (second[c++]==id)
		{
			continue;
		}
		return true;
	}
	return false;
}

void VulkanRenderer::splitChannels(const unsigned char * input, const int width, const int height, const int channels, const int outchannels, std::vector<std::vector<uint8_t>*>& output)
{
	for (size_t index = 0; index < width*height * channels; index += channels)
	{
		for (int ix2 = 0; ix2 < channels; ++ix2)
		{
			for (int ch = 0; ch < outchannels; ++ch)
			{
				(*output[ch])[index+ix2] = input[index + ch];
			}
		}
	}
}

int VulkanRenderer::loadMesh(SCM::MeshData * data)
{
	auto verts = scmVertsToVVertex(data->m_vertices);
	glm::vec3 minp, maxp;
	minp = glm::vec3(std::numeric_limits<float>::max());
	maxp = glm::vec3(-std::numeric_limits<float>::max());
	for (auto v : verts)
	{
		minp = glm::min(minp, v.pos);
		maxp = glm::max(maxp, v.pos);
	}
	m_allmeshes.insert({ data->m_meshId, std::make_shared<VMesh>(m_renderer->getManager()) });
	m_allmeshes[data->m_meshId]->load(verts, data->m_indices, minp, maxp);
	m_allmeshes[data->m_meshId]->uPerModelInfo = reinterpret_cast<PerModelUniformBuffer *>(m_renderer->getUniformBlob().alloc(sizeof(PerModelUniformBuffer)));
	return 0;
}

rj::ImageWrapper VulkanRenderer::loadTexture(const std::string path, bool flip)
{
	int width;
	int height;
	int channels;
	stbi__vertically_flip_on_load = flip;
	unsigned char* image = stbi_load(path.c_str(), &width, &height, &channels, 4);
	auto imgwrap = loadTextureBinary(image, width, height, channels);
	stbi_image_free(image);

	return imgwrap;
}

rj::ImageWrapper VulkanRenderer::loadTextureBinary(unsigned char * input, int width, int height, int channels)
{
	rj::ImageWrapper imgwrap;
	//!TODO put gli format according to channels
	rj::loadTexture2DFromBinaryData(&imgwrap, m_renderer->getManager(), input, width, height, gli::FORMAT_RGBA8_UNORM_PACK8);
	return imgwrap;
}

void VulkanRenderer::updateDrawableRenderStates()
{
	m_renderer->getManager()->deviceWaitIdle();
	m_renderer->createUniformBuffers();
	m_renderer->createDescriptorSets();
	m_renderer->createCommandBuffers();
}

void VulkanRenderer::updateVMeshData(ipengine::ipid meshid, const SCM::TransformData* data)
{
	auto vmeshtoupdate = m_allmeshes[meshid];
	auto transformdata = data;
	vmeshtoupdate->setPosition(transformdata->m_location);
	vmeshtoupdate->setRotation(transformdata->m_rotation);
	vmeshtoupdate->setScale((transformdata->m_scale.x + transformdata->m_scale.y + transformdata->m_scale.z) / 3);
	vmeshtoupdate->uniformDataChanged = true;
}

glm::mat4 VulkanRenderer::ViewFromTransData(const SCM::TransformData *transform)
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

	auto viewmat = viewRot * viewTr;
	//vdirty = false;
	/*std::cout << "Camera axes: \n";
	std::cout << "X: " << transform.localx.x << " " << transform.localx.y << " " << transform.localx.z << "\n";
	std::cout << "Y: " << transform.localy.x << " " << transform.localy.y << " " << transform.localy.z << "\n";
	std::cout << "Z: " << transform.localz.x << " " << transform.localz.y << " " << transform.localz.z << "\n\n";*/

	return viewmat;
}