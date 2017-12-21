// Plugin2.cpp : Defines the exported functions for the DLL application.
//1

#include "GraphicsModulePreRenderVR.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
GraphicsModulePreRenderVR::GraphicsModulePreRenderVR()
{
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
	mesh->m_dirty = true;
}


void GraphicsModulePreRenderVR::execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args)
{
	int i = 0;
	vr::EVRInitError vrerr;
	if (!init)
	{
		//create frame buffers and memes
		init = true;
		
		
		//ipengine::Scheduler& sched = m_core->getScheduler();
		//auto time = ipengine::Time(1.f, 1);
		scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
		//args[0].cast<IGraphics_API*>()->setCameraEntity(scm->getEntityByName("Camera")->m_entityId);
		ovrmodule = m_info.dependencies.getDep<IBasicOpenVRModule_API>("openvr");
		datastore = m_info.dependencies.getDep<IDataStoreModuleh_API>("datastore");
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
		if (ents.count("OpenVRHMD") == 0)
		{
			ents["OpenVRHMD"] = new SCM::Entity();
			auto hmdent = ents["OpenVRHMD"];
			hmdent->m_entityId = m_core->createID();
			hmdent->m_boundingData = SCM::BoundingData(SCM::BoundingSphere());
			hmdent->isBoundingBox = false;
			hmdent->m_transformData = SCM::Transform();
			hmdent->m_transformData.setData()->m_location.x = 0;
			hmdent->m_transformData.setData()->m_location.y = 0;
			hmdent->m_transformData.setData()->m_location.z = 5;
			hmdent->m_name = "OpenVRHMD";
		}
		//Do the controller model stuff
		if (ents.count("OpenVRControllerLeft") == 0)
		{
			auto cntrid = m_core->createID();
			auto cntrtrans = SCM::Transform();
			auto cntrbounding = SCM::BoundingData(SCM::BoundingSphere());

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
				rendermodelnames[std::string(realname)] =rmit-1;
				delete realname;
			}
			controllermodelindex = rendermodelnames["vr_controller_vive_1_5"];
			vr::EVRRenderModelError rendermodelerr;
			auto origpathsize = rendermodels->GetRenderModelOriginalPath("vr_controller_vive_1_5", nullptr, 0, &rendermodelerr);
			char* origrendermodelpath = new char[origpathsize];
			rendermodels->GetRenderModelOriginalPath("vr_controller_vive_1_5", origrendermodelpath, origpathsize, &rendermodelerr);
			vr::RenderModel_t *controllermodel = new vr::RenderModel_t();
			while(rendermodels->LoadRenderModel_Async("vr_controller_vive_1_5", &controllermodel)!= vr::EVRRenderModelError::VRRenderModelError_None);

			//get rendermodel components
			int cmpit = 0;
			int cmpnamel = 0;
			std::map<std::string, int> componentnames;
			while ((bufsize = rendermodels->GetComponentName("vr_controller_vive_1_5", cmpit, nullptr, 0)) > 0)
			{
				char* realname = new char[bufsize];
				rendermodels->GetComponentName("vr_controller_vive_1_5", cmpit++, realname, bufsize);
				//std::cout << std::string(realname);
				componentnames[std::string(realname)] = cmpit-1;
				delete realname;
			}

			auto &mobs = scm->getMeshes();
			mobs.push_back(SCM::MeshData());
			mobs.back().m_meshId = m_core->createID();
			vrrendermodeltoscmmeshobject(controllermodel, &mobs.back());
			//controller material????
			mobs.back().m_material = &scm->getMaterials().front();
			std::vector<SCM::MeshData*> meshes;
			//For every component?
			meshes.push_back(&mobs.back());
			scm->getMeshedObjects().push_back((SCM::MeshedObject(meshes, m_core->createID())));

			auto &cntrmeshes = scm->getMeshedObjects().back();
			ents["OpenVRControllerLeft"] = new SCM::ThreeDimEntity(cntrid, cntrtrans, cntrbounding, false, true, &cntrmeshes);
			
			auto cntrent = ents["OpenVRControllerLeft"];
			cntrent->m_name = "OpenVRControllerLeft";
			scm->getThreeDimEntities()[cntrent->m_entityId] = static_cast<SCM::ThreeDimEntity*>(cntrent);
		}
		if (ents.count("OpenVRControllerRight") == 0)
		{

		}

		//auto rendermodels = ovrmodule->getRenderModels();
		//rendermodels->LoadRenderModel_Async(rendermodels)

	}

	//Set framebuffer 1 and set igraphics matrixes and call render 
	if (argnames.size() < 1 || argnames[0] != "this")
	{
		return;
	}
	if(argnames.size() <2 || argnames[1] !="rendermatrixes")
	{
		return; 
	}

	auto graphicsmodule = args[0].cast<IGraphics_API*>();
	auto matrices = args[1].cast<IGraphics_API::renderMatrixes>();
	auto preproj = glm::mat4(*matrices.proj);
	auto preview = glm::mat4(*matrices.view);
	SCM::Entity* cam;
	auto cameraentity = graphicsmodule->getCameraEntity();
	datastore->set("cameraid",cameraentity);
	cam = scm->getEntityById(cameraentity);

	if (cam == nullptr)
	{
		return;
	}
	uint32_t prew, preh;
	graphicsmodule->getResolution(prew, preh);
	graphicsmodule->setResolution(renderWidth, renderHeight);

	ovrmodule->getCompositor()->WaitGetPoses(lastposes, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	//TODO update controller states
	auto contr = scm->getEntityByName("OpenVRControllerLeft");
	contr->m_transformData.setData()->m_scale = { 1,1,1 };
	contr->m_transformData.setData()->m_location = { 0,0,5 };
	contr->m_transformData.setData()->m_isMatrixDirty = true;

	auto trans = cam->m_transformData.getData()->m_location;
	auto hmdView = glm::mat4(convert(lastposes[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking));

	hmdView[3][0] += trans.x;
	hmdView[3][1] += trans.y;
	hmdView[3][2] += trans.z;
	hmdView = inverse(hmdView);

	graphicsmodule->setCameraEntity(IPID_INVALID);

	//LeftEye
	glEnable(GL_MULTISAMPLE); //GLERR;
	glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId); //GLERR;
	glViewport(0, 0, renderWidth, renderHeight);

	float znear, zfar;
	graphicsmodule->getClipRange(znear, zfar);

	auto proj = convert(ovrmodule->getSystem()->GetProjectionMatrix(vr::EVREye::Eye_Left, znear,zfar));
	*matrices.view= hmdView*glm::inverse(glm::mat4(convert(ovrmodule->getSystem()->GetEyeToHeadTransform(vr::EVREye::Eye_Left))));
	*matrices.proj= proj;
	graphicsmodule->render();
	//renderer->render(_scene, nullptr, &tview, &proj);

	resolveFB(leftEyeDesc.m_nRenderFramebufferId, leftEyeDesc.m_nResolveFramebufferId, renderWidth, renderHeight);

	//Set framebuffer 2 and set graphics matrixes
	glEnable(GL_MULTISAMPLE); //GLERR;
	glBindFramebuffer(GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId); //GLERR;
	glViewport(0, 0, renderWidth, renderHeight);
	proj = convert(ovrmodule->getSystem()->GetProjectionMatrix(vr::EVREye::Eye_Right, znear, zfar));
	*matrices.view = hmdView*glm::inverse(glm::mat4(convert(ovrmodule->getSystem()->GetEyeToHeadTransform(vr::EVREye::Eye_Right))));
	*matrices.proj = proj;
	graphicsmodule->render();

	resolveFB(rightEyeDesc.m_nRenderFramebufferId, rightEyeDesc.m_nResolveFramebufferId, renderWidth, renderHeight);

	vr::EVRCompositorError comperr;

	vr::Texture_t leftEyeTexture = { (void*)leftEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	comperr=vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);

	if (comperr != vr::EVRCompositorError::VRCompositorError_None)
	{
		throw;
	}

	vr::Texture_t rightEyeTexture = { (void*)rightEyeDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
	comperr= vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
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
	glDisable(GL_MULTISAMPLE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, prew, preh);
	graphicsmodule->setCameraEntity(cameraentity);
	*matrices.proj = preproj;
	*matrices.view = preview;
	graphicsmodule->setResolution(prew, preh);
	datastore->set("renderw", renderWidth);
	datastore->set("renderh", renderHeight);
	datastore->set("righteyeFB", rightEyeDesc);
	datastore->set("lefteyeFB", leftEyeDesc);
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
