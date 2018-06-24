#ifndef _ExtensionTest_H_
#define _ExtensionTest_H_

#include <boost/config.hpp>
#include <IModule_API.h>
#include <ISimpleContentModule_API.h>
#include <IBasicOpenVRModule_API.h>
#include <ISimpleSceneModule_API.h>
#include <glm/glm.hpp>
#include <glew\include\GL\glew.h>
#include <IDataStoreModuleh_API.h>
#include "glerror.h"
class VREntitySceneLoader : public IExtension {
public:
	VREntitySceneLoader();
	// TODO: add your methods here.
	virtual void execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args) override;


	// Inherited via IExtensionPoint
	virtual ExtensionInformation * getInfo() override;


	GLuint quadVAO, quadVBO;
	vr::TrackedDevicePose_t lastposes[vr::k_unMaxTrackedDeviceCount];
private:

	static const GLfloat QuadVerts[];

	glm::mat4 convert(const vr::HmdMatrix44_t &m) {
		return glm::mat4(
			m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
			m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
			m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
			m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3]);
	}

	glm::mat4x3 convert(const vr::HmdMatrix34_t &m) {
		return glm::mat4x3(
			m.m[0][0], m.m[1][0], m.m[2][0],
			m.m[0][1], m.m[1][1], m.m[2][1],
			m.m[0][2], m.m[1][2], m.m[2][2],
			m.m[0][3], m.m[1][3], m.m[2][3]);
	}

	std::shared_ptr<SCM::ISimpleContentModule_API> scm;
	std::shared_ptr<IBasicOpenVRModule_API> ovrmodule;
	std::shared_ptr<ISimpleSceneModule_API> scenemodule;
	//std::shared_ptr<IDataStoreModuleh_API> datastore;
	ExtensionInformation m_info;
	bool init = false;
};

extern "C" BOOST_SYMBOL_EXPORT VREntitySceneLoader extension;
VREntitySceneLoader extension;

#endif