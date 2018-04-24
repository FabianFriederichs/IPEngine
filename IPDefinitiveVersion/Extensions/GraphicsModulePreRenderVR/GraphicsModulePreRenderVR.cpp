// Plugin2.cpp : Defines the exported functions for the DLL application.
//1

#include "GraphicsModulePreRenderVR.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
GraphicsModulePreRenderVR::GraphicsModulePreRenderVR()
{
	m_info.identifier = "GraphicsModulePreRenderVR";

	//m_info.
}

const GLfloat GraphicsModulePreRenderVR::QuadVerts[] = {   // Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
									   // Positions   // TexCoords
	-1.0f, 1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,

	-1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f
};

glm::vec3 hmdvec3toglm(const vr::HmdVector3_t& v)
{
	return { v.v[0], v.v[1], v.v[2] };
}

void vrrendermodeltoscmmeshobject(vr::RenderModel_t* model, SCM::MeshData* mesh)
{
	for (int iv = 0; iv < model->unVertexCount; ++iv)
	{
		auto vert = model->rVertexData + iv;
		auto pos = hmdvec3toglm(vert->vPosition);
		auto norm = hmdvec3toglm(vert->vNormal);
		SCM::VertexData dat(pos, { vert->rfTextureCoord[0], vert->rfTextureCoord[1] }, norm, { 0,0,0 });
		mesh->m_vertices.setData().push_back(dat);
	}
	mesh->m_dynamic = false;
	for (int ii = 0; ii < 3*model->unTriangleCount; ++ii)
	{
		mesh->m_indices.push_back(*(model->rIndexData + ii));
	}
	//TODO: calc tangents for normal mapping
	mesh->m_dirty = true;
}


void GraphicsModulePreRenderVR::execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args)
{
	int i = 0;
	vr::EVRInitError vrerr;
	if (argnames.size() < 1 || argnames[0] != "this")
	{
		return;
	}
	if (argnames.size() <2 || argnames[1] != "rendermatrixes")
	{
		return;
	}

	auto graphicsmodule = args[0].cast<IGraphics_API*>();
	auto matrices = args[1].cast<IGraphics_API::renderMatrixes>();
	auto preproj = glm::mat4(*matrices.proj);
	auto preview = glm::mat4(*matrices.view);

	if (!init)
	{
		//create frame buffers and memes
		init = true;
		
		
		//ipengine::Scheduler& sched = m_core->getScheduler();
		//auto time = ipengine::Time(1.f, 1);
		scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
		//args[0].cast<IGraphics_API*>()->setCameraEntity(scm->getEntityByName("Camera")->m_entityId);
		ovrmodule = m_info.dependencies.getDep<IBasicOpenVRModule_API>("openvr");
		//datastore = m_info.dependencies.getDep<IDataStoreModuleh_API>("datastore");
		scenemodule = m_info.dependencies.getDep<ISimpleSceneModule_API>("scene");

		//glGenVertexArrays(1, &quadVAO); //GLERR;
		//glGenBuffers(1, &quadVBO); //GLERR;
		//glBindVertexArray(quadVAO); //GLERR;
		//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);// GLERR;
		//glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVerts), &QuadVerts, GL_STATIC_DRAW);// GLERR;
		//glEnableVertexAttribArray(0);// GLERR;
		//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0); //GLERR;
		//glEnableVertexAttribArray(1);// GLERR;
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat))); //GLERR;
		//glBindVertexArray(0); //GLERR;
		//vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &vrerr);

		ovrmodule->getSystem()->GetRecommendedRenderTargetSize(&renderWidth, &renderHeight);


		if (!CreateFrameBuffer(renderWidth, renderHeight, leftEyeDesc))
		{
			throw;
		}
		if (!CreateFrameBuffer(renderWidth, renderHeight, rightEyeDesc))
		{
			throw;
		}

		//Add HMD and COntroller entities to SCM
		
		//HMD 
		auto &ents = scm->getEntities();
		auto entbyname = scm->getEntitiesByName("OpenVRHMD");
		if (entbyname.empty())
		{
			auto hmdent = new SCM::Entity();
			hmdent->m_entityId = m_core->createID();
			hmdid = hmdent->m_entityId;
			ents[hmdent->m_entityId] =hmdent;
			hmdent->m_boundingData = SCM::BoundingData(SCM::BoundingSphere());
			hmdent->isBoundingBox = false;
			hmdent->m_transformData = SCM::Transform();
			hmdent->m_transformData.setData()->m_location.x = 0;
			hmdent->m_transformData.setData()->m_location.y = 0;
			hmdent->m_transformData.setData()->m_location.z = 0;
			hmdent->m_name = "OpenVRHMD";
			//if (ents.count("Camera") > 0)
			//{
			//	//ents["Camera"]->m_parent = ents["OpenVRHMD"];
			//}
			/*if (scm->getEntityByName("Camera") != nullptr)
				hmdent->m_parent = scm->getEntityByName("Camera");*/
		}
		//Do the controller model stuff
		entbyname = scm->getEntitiesByName("OpenVRControllerLeft");
		if (entbyname.empty())
		{
			auto cntrid = m_core->createID();
			auto cntrtrans = SCM::Transform();
			SCM::BoundingSphere sph;
			sph.m_center = { 0.f,0.f,0.f };
			sph.m_radius = controllersphereradius;
			auto cntrbounding = SCM::BoundingData(sph);

			//Create mesh from VR render model
			auto rendermodels = ovrmodule->getRenderModels();
			auto modelc = rendermodels->GetRenderModelCount();
			int rmit = 0;
			int rmnamel = 0;
			size_t bufsize;
			int controllermodelindex = -1;
			std::map< std::string, int> rendermodelnames;
			while ((bufsize = rendermodels->GetRenderModelName(rmit, nullptr, 0)) > 0)
			{
				char* realname = new char[bufsize];
				rendermodels->GetRenderModelName(rmit++, realname, bufsize);
				//std::cout << std::string(realname);
				rendermodelnames[std::string(realname)] = rmit - 1;
				delete realname;
			}
			controllermodelindex = rendermodelnames["vr_controller_vive_1_5"];
			vr::EVRRenderModelError rendermodelerr;
			auto origpathsize = rendermodels->GetRenderModelOriginalPath("vr_controller_vive_1_5", nullptr, 0, &rendermodelerr);
			char* origrendermodelpath = new char[origpathsize];
			rendermodels->GetRenderModelOriginalPath("vr_controller_vive_1_5", origrendermodelpath, origpathsize, &rendermodelerr);
			vr::RenderModel_t *controllermodel;// = new vr::RenderModel_t();
			while (rendermodels->LoadRenderModel_Async("vr_controller_vive_1_5", &controllermodel) != vr::EVRRenderModelError::VRRenderModelError_None);
			
			//get rendermodel components
			int cmpit = 0;
			int cmpnamel = 0;
			std::map<std::string, int> componentnames;
			while ((bufsize = rendermodels->GetComponentName("vr_controller_vive_1_5", cmpit, nullptr, 0)) > 0)
			{
				char* realname = new char[bufsize];
				rendermodels->GetComponentName("vr_controller_vive_1_5", cmpit++, realname, bufsize);
				//std::cout << std::string(realname);
				componentnames[std::string(realname)] = cmpit - 1;
				delete realname;
			}

			
			auto &scmmeshes = scm->getMeshes();
			scmmeshes.push_back(SCM::MeshData());
			auto& newmesh = scmmeshes.back();
			newmesh.m_meshId = m_core->createID();
			vrrendermodeltoscmmeshobject(controllermodel, &newmesh);
			newmesh.updateTangents();
			//newmesh.updateNormals();

			//controller material????
			//newmesh.m_material = &mats.back();
			std::vector<SCM::MeshData*> meshes;
			//For every component?
			meshes.push_back(&newmesh);
			auto &mobs = scm->getMeshedObjects();
			mobs.push_back((SCM::MeshedObject(meshes, m_core->createID())));
			setupControllerMat(rendermodels, controllermodel, graphicsmodule);


			mobs.back().meshtomaterial[newmesh.m_meshId] = contrmatid;
			auto &cntrmeshes = scm->getMeshedObjects().back();
			//cntrtrans.setData()->m_rotation = { 1,0,0,0 };
			cntrtrans.setData()->m_scale = { 1,1,1 };
			cntrtrans.setData()->m_localX = { 1,0,0 };
			cntrtrans.setData()->m_localY = { 0,1,0 };
			cntrtrans.setData()->m_localZ = { 0,0,1 };
			
			rendermodels->FreeRenderModel(controllermodel);
			auto lctde = new SCM::ThreeDimEntity(cntrid, cntrtrans, cntrbounding, false, false, &cntrmeshes);
			//lctde->generateBoundingBox();
			ents[cntrid] = lctde;// new SCM::ThreeDimEntity(cntrid, cntrtrans, cntrbounding, false, true, &cntrmeshes);
			/*if(scm->getEntityByName("Camera")!=nullptr)
				ents["OpenVRControllerLeft"]->m_parent = scm->getEntityByName("Camera");*/
			lctde->m_name = "OpenVRControllerLeft";
			scm->getThreeDimEntities()[cntrid] = static_cast<SCM::ThreeDimEntity*>(lctde);
			scm->getThreeDimEntities()[cntrid]->generateBoundingSphere();
			lctrlid = cntrid;
		}
		entbyname = scm->getEntitiesByName("OpenVRControllerRight");
		if (entbyname.empty())
		{
			auto cntrid = m_core->createID();
			auto cntrtrans = SCM::Transform();
			SCM::BoundingSphere sph;
			sph.m_center = { 0.f,0.f,0.f };
			sph.m_radius = controllersphereradius;
			auto cntrbounding = SCM::BoundingData(sph);
			
			//Create mesh from VR render model
			auto rendermodels = ovrmodule->getRenderModels();
			auto modelc = rendermodels->GetRenderModelCount();
			int rmit = 0;
			int rmnamel = 0;
			size_t bufsize;
			int controllermodelindex = -1;
			std::map< std::string, int> rendermodelnames;
			while ((bufsize = rendermodels->GetRenderModelName(rmit, nullptr, 0)) > 0)
			{
				char* realname = new char[bufsize];
				rendermodels->GetRenderModelName(rmit++, realname, bufsize);
				//std::cout << std::string(realname);
				rendermodelnames[std::string(realname)] = rmit - 1;
				delete realname;
			}
			controllermodelindex = rendermodelnames["vr_controller_vive_1_5"];
			vr::EVRRenderModelError rendermodelerr;
			auto origpathsize = rendermodels->GetRenderModelOriginalPath("vr_controller_vive_1_5", nullptr, 0, &rendermodelerr);
			char* origrendermodelpath = new char[origpathsize];
			rendermodels->GetRenderModelOriginalPath("vr_controller_vive_1_5", origrendermodelpath, origpathsize, &rendermodelerr);
			vr::RenderModel_t *controllermodel = new vr::RenderModel_t();
			while (rendermodels->LoadRenderModel_Async("vr_controller_vive_1_5", &controllermodel) != vr::EVRRenderModelError::VRRenderModelError_None);

			//get rendermodel components
			int cmpit = 0;
			int cmpnamel = 0;
			std::map<std::string, int> componentnames;
			while ((bufsize = rendermodels->GetComponentName("vr_controller_vive_1_5", cmpit, nullptr, 0)) > 0)
			{
				char* realname = new char[bufsize];
				rendermodels->GetComponentName("vr_controller_vive_1_5", cmpit++, realname, bufsize);
				//std::cout << std::string(realname);
				componentnames[std::string(realname)] = cmpit - 1;
				delete realname;
			}

			//generate new SCM texture for diffuse model texture
			
			//SCM::TextureData td{ tid};
			//td.m_size = { controllerdiffuse->unWidth, controllerdiffuse->unHeight };
			//Generate material with this texture
			//SCM::MaterialData md{ m_core->createID(), scm->getShaders().back().m_shaderId,{ { "albedo", td } } };
			auto& mats = scm->getMaterials();
			//mats.push_back(md);
			auto &scmmeshes = scm->getMeshes();
			scmmeshes.push_back(SCM::MeshData());
			auto& newmesh = scmmeshes.back();
			newmesh.m_meshId = m_core->createID();
			vrrendermodeltoscmmeshobject(controllermodel, &newmesh);
			newmesh.updateTangents();
			//newmesh.updateNormals();
			//controller material????
			//newmesh.m_material = &mats.back();
			std::vector<SCM::MeshData*> meshes;
			//For every component?
			meshes.push_back(&newmesh);
			auto &mobs = scm->getMeshedObjects();
			mobs.push_back((SCM::MeshedObject(meshes, m_core->createID())));

			setupControllerMat(rendermodels, controllermodel, graphicsmodule);
			mobs.back().meshtomaterial[newmesh.m_meshId] = contrmatid;
			auto &cntrmeshes = scm->getMeshedObjects().back();
			//cntrtrans.setData()->m_rotation = { 1,0,0,0 };
			cntrtrans.setData()->m_scale = { 1,1,1 };
			cntrtrans.setData()->m_localX = { 1,0,0 };
			cntrtrans.setData()->m_localY = { 0,1,0 };
			cntrtrans.setData()->m_localZ = { 0,0,1 };

			rendermodels->FreeRenderModel(controllermodel);
			auto rctde = new SCM::ThreeDimEntity(cntrid, cntrtrans, cntrbounding, false, false, &cntrmeshes);
			//rctde->generateBoundingBox();
			ents[cntrid] = rctde;//new SCM::ThreeDimEntity(cntrid, cntrtrans, cntrbounding, false, true, &cntrmeshes);
			/*if (scm->getEntityByName("Camera") != nullptr)
				ents["OpenVRControllerRight"]->m_parent = scm->getEntityByName("Camera");*/
			rctde->m_name = "OpenVRControllerRight";
			scm->getThreeDimEntities()[cntrid] = static_cast<SCM::ThreeDimEntity*>(rctde);
			rctrlid = cntrid;
			//scm->getThreeDimEntities()[cntrent->m_entityId]->generateBoundingSphere();
		}

		//auto rendermodels = ovrmodule->getRenderModels();
		//rendermodels->LoadRenderModel_Async(rendermodels)
		return;
	}

	//Set framebuffer 1 and set igraphics matrixes and call render 
	

	
	//SCM::Entity* cam;
	auto cameraentity = scm->getEntityById(graphicsmodule->getCameraEntity());
	//datastore->set("cameraid",cameraentity);
	//cam = scm->getEntityById(cameraentity);
	if(hmdid!= IPID_INVALID)
		graphicsmodule->setCameraEntity(hmdid);

	/*if (cam == nullptr)
	{
		return;
	}*/
	uint32_t prew, preh;
	graphicsmodule->getResolution(prew, preh);
	graphicsmodule->setResolution(renderWidth, renderHeight);

	ovrmodule->getCompositor()->WaitGetPoses(lastposes, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	//TODO update controller/hmd states
	if (cameraid == IPID_INVALID)
	{
		auto caments = scm->getEntitiesByName("Camera");
		if (!caments.empty())
			cameraid = caments.front()->m_entityId;
	}
	else if (!scm->getEntityById(cameraid)->isActive)
	{
		auto caments = scm->getEntitiesByName("Camera");
		for (auto e : caments)
		{
			if (e->isActive)
				cameraid = e->m_entityId;
		}
	}
	auto lefthandcontrollerindex = ovrmodule->getSystem()->GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole::TrackedControllerRole_LeftHand);
	auto righthandcontrollerindex = ovrmodule->getSystem()->GetTrackedDeviceIndexForControllerRole(vr::ETrackedControllerRole::TrackedControllerRole_RightHand);
	if (ovrmodule->getSystem()->IsTrackedDeviceConnected(lefthandcontrollerindex))
	{
		auto contr = scm->getEntityById(lctrlid);
		cameraentity = scm->getEntityById(cameraid);
		auto leftcontrollerpose = lastposes[lefthandcontrollerindex];
		auto convertedpose = convert(leftcontrollerpose.mDeviceToAbsoluteTracking);
		auto trans = contr->m_transformData.setData();
		trans->m_location.x = convertedpose[3][0];
		trans->m_location.y = convertedpose[3][1];
		trans->m_location.z = convertedpose[3][2];
		if (cameraentity)//contr->m_parent != nullptr)
		{
			auto parenttrans = cameraentity->m_transformData.getData();//contr->m_parent->m_transformData.getData();
			//trans->m_location += parenttrans->m_location;
			trans->m_location.x += parenttrans->m_location.x;
			trans->m_location.y += parenttrans->m_location.y;
			trans->m_location.z += parenttrans->m_location.z;
		}


		//Extract orientation quat
		trans->m_rotation = glm::quat_cast(glm::mat4(convertedpose));
		/*if (contr->m_parent != nullptr)
		{
			contr->m_parent->m_transformData.setData()->m_rotation = trans->m_rotation;
			contr->m_parent->m_transformData.setData()->m_isMatrixDirty = true;
		}*/
		//trans->m_isMatrixDirty = true;
		trans->updateTransform();
		contr->isActive = (leftcontrollerpose.bDeviceIsConnected && leftcontrollerpose.bPoseIsValid && leftcontrollerpose.eTrackingResult == vr::ETrackingResult::TrackingResult_Running_OK);
	}
	if (ovrmodule->getSystem()->IsTrackedDeviceConnected(righthandcontrollerindex))
	{
		auto contr = scm->getEntityById(rctrlid);
		cameraentity = scm->getEntityById(cameraid);

		auto rightcontrollerpose = lastposes[righthandcontrollerindex];
		auto convertedpose = convert(rightcontrollerpose.mDeviceToAbsoluteTracking);
		auto trans = contr->m_transformData.setData();
		trans->m_location.x = convertedpose[3][0];
		trans->m_location.y = convertedpose[3][1];
		trans->m_location.z = convertedpose[3][2];
		if (cameraentity)//contr->m_parent != nullptr)
		{
			auto parenttrans = cameraentity->m_transformData.getData();//contr->m_parent->m_transformData.getData();
			//trans->m_location += parenttrans->m_location;

			trans->m_location.x += parenttrans->m_location.x;
			trans->m_location.y += parenttrans->m_location.y;
			trans->m_location.z += parenttrans->m_location.z;
		}


		//Extract orientation quat
		trans->m_rotation = glm::quat_cast(glm::mat4(convertedpose));
		/*if (contr->m_parent != nullptr)
		{
			contr->m_parent->m_transformData.setData()->m_rotation = trans->m_rotation;
			contr->m_parent->m_transformData.setData()->m_isMatrixDirty = true;
		}*/
		trans->updateTransform();
		contr->isActive = (rightcontrollerpose.bDeviceIsConnected && rightcontrollerpose.bPoseIsValid && rightcontrollerpose.eTrackingResult== vr::ETrackingResult::TrackingResult_Running_OK);
		//trans->m_isMatrixDirty = true;
	}
	if (ovrmodule->getSystem()->IsTrackedDeviceConnected(vr::k_unTrackedDeviceIndex_Hmd))
	{
		auto hmd = scm->getEntityById(hmdid);
		cameraentity = scm->getEntityById(cameraid);

		auto hmdpose = lastposes[vr::k_unTrackedDeviceIndex_Hmd];
		auto convertedpose = convert(hmdpose.mDeviceToAbsoluteTracking);
		auto trans = hmd->m_transformData.setData();
		trans->m_location.x = convertedpose[3][0];
		trans->m_location.y = convertedpose[3][1];
		trans->m_location.z = convertedpose[3][2];
		if (cameraentity)
		{
			auto parenttrans = cameraentity->m_transformData.getData();
			//trans->m_location += parenttrans->m_location;

			trans->m_location.x += parenttrans->m_location.x;
			trans->m_location.y += parenttrans->m_location.y;
			trans->m_location.z += parenttrans->m_location.z;
		}


		//Extract orientation quat
		trans->m_rotation = glm::quat_cast(glm::mat4(convertedpose));
		if (cameraentity)
		{
			cameraentity->m_transformData.setData()->m_rotation = glm::normalize(trans->m_rotation);
			//cameraentity->m_transformData.setData()->m_location = trans->m_location;
			cameraentity->m_transformData.setData()->m_isMatrixDirty = true;
			cameraentity->m_transformData.setData()->updateTransform();
		}
		trans->m_isMatrixDirty = true;
		trans->updateTransform();

	}
	//Check if controller is active


	//contr->m_transformData.setData()->m_scale = { 1,1,1 };
	//contr->m_transformData.setData()->m_location = { 0,0,5 };
	//contr->m_transformData.setData()->m_isMatrixDirty = true;
	auto hmdentity = scm->getEntityById(hmdid);
	auto trans = hmdentity->m_transformData.getData()->m_location;
	auto hmdView = /*scm->getEntityByName("OpenVRHMD")->m_transformData.getData()->m_transformMatrix;*/glm::mat4(convert(lastposes[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking));

	hmdView[3][0] = trans.x;
	hmdView[3][1] = trans.y;
	hmdView[3][2] = trans.z;
	hmdView = inverse(hmdView);

	graphicsmodule->setCameraEntity(IPID_INVALID);

	//LeftEye
	glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId); //GLERR;
	glViewport(0, 0, renderWidth, renderHeight);

	float znear, zfar;
	graphicsmodule->getClipRange(znear, zfar);

	auto proj = convert(ovrmodule->getSystem()->GetProjectionMatrix(vr::EVREye::Eye_Left, znear,zfar));
	*matrices.view= glm::inverse(glm::mat4(convert(ovrmodule->getSystem()->GetEyeToHeadTransform(vr::EVREye::Eye_Left)))) * hmdView;
	*matrices.proj= proj;
	graphicsmodule->render(leftEyeDesc.m_nRenderFramebufferId, renderWidth, renderHeight, true);

	resolveFB(leftEyeDesc.m_nRenderFramebufferId, leftEyeDesc.m_nResolveFramebufferId, renderWidth, renderHeight);

	//Set framebuffer 2 and set graphics matrixes
	glBindFramebuffer(GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId); //GLERR;
	glViewport(0, 0, renderWidth, renderHeight);
	proj = convert(ovrmodule->getSystem()->GetProjectionMatrix(vr::EVREye::Eye_Right, znear, zfar));
	*matrices.view = glm::inverse(glm::mat4(convert(ovrmodule->getSystem()->GetEyeToHeadTransform(vr::EVREye::Eye_Right)))) * hmdView;
	*matrices.proj = proj;
	graphicsmodule->render(rightEyeDesc.m_nRenderFramebufferId, renderWidth, renderHeight, true);

	resolveFB(rightEyeDesc.m_nRenderFramebufferId, rightEyeDesc.m_nResolveFramebufferId, renderWidth, renderHeight);

	vr::EVRCompositorError comperr;
	//TODO: switch between gamma and linear color space if gpu supports srgb framebuffers
	vr::Texture_t leftEyeTexture = { (void*)leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
	
	comperr=vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	glGetError();

	if (comperr != vr::EVRCompositorError::VRCompositorError_None)
	{
		throw;
	}
	
	vr::Texture_t rightEyeTexture = { (void*)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	comperr= vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
	glGetError();
	if (comperr != vr::EVRCompositorError::VRCompositorError_None)
	{
		throw;
	}
	//vrcomp->PostPresentHandoff();

	//vr::TrackedDevicePose_t pose[vr::k_unMaxTrackedDeviceCount];
	//ovrmodule->getCompositor()->WaitGetPoses(pose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	float fSecondsSinceLastVsync;
	ovrmodule->getSystem()->GetTimeSinceLastVsync(&fSecondsSinceLastVsync, NULL);

	float fDisplayFrequency = ovrmodule->getSystem()->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_DisplayFrequency_Float);
	float fFrameDuration = 1.f / fDisplayFrequency;
	float fVsyncToPhotons = ovrmodule->getSystem()->GetFloatTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SecondsFromVsyncToPhotons_Float);

	float fPredictedSecondsFromNow = fFrameDuration - fSecondsSinceLastVsync + fVsyncToPhotons;

	ovrmodule->getSystem()->GetDeviceToAbsoluteTrackingPose(vr::ETrackingUniverseOrigin::TrackingUniverseStanding, fPredictedSecondsFromNow, lastposes, vr::k_unMaxTrackedDeviceCount);
	//glDisable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, prew, preh);
	graphicsmodule->setCameraEntity(hmdentity->m_entityId);//scm->getEntityByName("OpenVRHMD")->m_parent->m_entityId);

	*matrices.proj = preproj;
	*matrices.view = preview;
	graphicsmodule->setResolution(prew, preh);
	/*datastore->set("renderw", renderWidth);
	datastore->set("renderh", renderHeight);
	datastore->set("righteyeFB", rightEyeDesc);
	datastore->set("lefteyeFB", leftEyeDesc);*/
	//for (auto name : argnames)
	//{
	//	if (name == "this")
	//	{
	//		//Argument is calling module instance
	//		if (args.size() > i)
	//		{
	//			//(args[i].cast<GraphicsModule*>())
	//		}
	//	}
	//	if (name == "test")
	//	{
	//		if (args.size() > i)
	//		{
	//			auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
	//			auto mmeme = args[i].cast<std::string*>();
	//			args[i].cast<std::string*>()->append("Success!!");
	//		}
	//	}
	//	++i;
	//}

}

ExtensionInformation * GraphicsModulePreRenderVR::getInfo()
{
	return &m_info;
}

void GraphicsModulePreRenderVR::setupControllerMat(boost::shared_ptr<vr::IVRRenderModels> rendermodels, vr::RenderModel_t * cntrmodel, IGraphics_API* graphicsmodule)
{
	if (!iscontrmadsetup)
	{
		vr::RenderModel_TextureMap_t *controllerdiffuse;
		while (rendermodels->LoadTexture_Async(cntrmodel->diffuseTextureId, &controllerdiffuse) != vr::EVRRenderModelError::VRRenderModelError_None);
		//generate new SCM texture for diffuse model texture
		SCM::TextureFile tf{ m_core->createID(), "controllerdiff", false };
		SCM::TextureFile tf2{ m_core->createID(), "controllernorm", false };
		SCM::TextureFile tf3{ m_core->createID(), "controllermrar", false };
		SCM::TextureFile tf4{ m_core->createID(), "controlleremissive", false };
		auto& texts = scm->getTextures();
		texts.push_back(tf);
		texts.push_back(tf2);
		texts.push_back(tf3);
		texts.push_back(tf4);

		SCM::TextureData td{ tf.m_textureId };
		td.m_size = { controllerdiffuse->unWidth, controllerdiffuse->unHeight };
		SCM::TextureData td2{ tf2.m_textureId };
		td2.m_size = { 1,1 };
		SCM::TextureData td3{ tf3.m_textureId };
		td3.m_size = { 1, 1 };
		SCM::TextureData td4{ tf4.m_textureId };
		td4.m_size = { 1, 1 };

		//Generate material with this texture
		SCM::MaterialData md{ m_core->createID(), scm->getShaders().back().m_shaderId,{ { "albedo", td },{ "mrar", td3 },{ "normal", td2 },{ "emissive", td4 } } };
		auto& mats = scm->getMaterials();
		mats.push_back(md);

		uint8_t emissivevalues[]{ 0,0,0,1 };
		uint8_t mrarvalues[]{ 0, 255 * 0.65, 255, 10 };
		uint8_t normalvalues[]{ 0, 0, 0 };
		//Load data into graphicsmodule because texture data is not in a file
		graphicsmodule->loadTextureFromMemory({ controllerdiffuse->unWidth, controllerdiffuse->unHeight, 4, controllerdiffuse->rubTextureMapData }, tf.m_textureId);
		graphicsmodule->setMaterialTexDefaultParams(tf.m_textureId, true);
		graphicsmodule->loadTextureFromMemory({ 1, 1, 3, normalvalues }, tf2.m_textureId);
		graphicsmodule->setMaterialTexDefaultParams(tf2.m_textureId, true);
		graphicsmodule->loadTextureFromMemory({ 1, 1, 4, mrarvalues }, tf3.m_textureId);
		graphicsmodule->setMaterialTexDefaultParams(tf3.m_textureId, true);
		graphicsmodule->loadTextureFromMemory({ 1, 1, 4, emissivevalues }, tf4.m_textureId);
		graphicsmodule->setMaterialTexDefaultParams(tf4.m_textureId, true);
		rendermodels->FreeTexture(controllerdiffuse);

		contrmatid = mats.back().m_materialId;
		iscontrmadsetup = true;
	}
}
