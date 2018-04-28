#ifndef _GameLogicModule_H_
#define _GameLogicModule_H_

#include <boost/config.hpp>
#include <IModule_API.h>
#include <ISimpleContentModule_API.h>
#include <IInput_API.h>
#include <IGraphics_API.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <chrono>
#include <IPCore/Messaging/EndpointRegistry.h>
#include <IPCore/DebugMonitoring/ErrorHandler.h>
#include <IPhysicsModule_API.h>

class GameLogicModule : public IModule_API {
public:
	GameLogicModule();
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
private:
	//messaging test
	ipengine::EndpointHandle m_gameLogicEndpoint;
	//error handler
	ipengine::ErrorHandler m_errhandler;
	ipengine::MessageType m_collisionmsgtype;
	ModuleInformation m_info;
	ipengine::Time delta;
	ipengine::Time lastUpdate;
	boost::shared_ptr<IInput_API> inputmodule;
	boost::shared_ptr<SCM::ISimpleContentModule_API> contentmodule;
	boost::shared_ptr<IGraphics_API> graphics;
	std::vector<ipengine::Scheduler::SubHandle> handles;
	ipengine::Time lastInputTimestamp = ipengine::Time(0.f);
	ipengine::Time timing = ipengine::Time(1.f / 60.f, 1);
	glm::vec3 camVelocity = { 0,0,0 };
	glm::vec2 mouseDelta = { 0,0 };
	bool w, a, s, d, yc,vc;
	bool box = false;
	float osc = std::acos(-1);
	float pitch, yaw;
	glm::vec3 lastlx, lastlz;
	IInput::Input lastMouseMove;
	bool initialized=false;
	double modifier = 1;
	void update(ipengine::TaskContext&);
	bool automovebox = false;
	bool mousecamera = true;
	float minimum_y = 0.f;

	void keyUpdate(IInput::Input&);
	void mousemoveUpdate(IInput::Input&);
	void mousescrollUpdate(IInput::Input&);
	void entityUpdate(SCM::Entity*);
	void entity3dUpdate(SCM::ThreeDimEntity*);
	void updateBoundingData(SCM::Entity*, const glm::vec3&, const glm::vec3&, float);

	void messageCallback(ipengine::Message& msg);
	void onError(ipengine::ipex& ex);

	// Inherited via IModule_API
	virtual bool _startup() override;

	// Inherited via IModule_API
	virtual bool _shutdown() override;


	//Move Entity stuff
	std::string holder1Name = "OpenVRControllerLeft";
	std::string holder2Name = "OpenVRControllerRight";
	ipengine::ipid cameraid = IPID_INVALID, hmdid = IPID_INVALID;
	bool isHoldEntityButton1Pressed[2] = { false };
	bool isHoldEntityButton2Pressed[2] = { false };
	bool holder1busy = false;
	bool holder2busy = false;
	ipengine::ipid heldEntity[2] = { IPID_INVALID,IPID_INVALID };
	ipengine::ipid holder[2] = { IPID_INVALID, IPID_INVALID };
	ipengine::ipid getAncestor(ipengine::ipid child);
	//bool inProximity(ipengine::ipid source, ipengine::ipid target, float maxDistance);
	void onHoldStart(ipengine::ipid source, ipengine::ipid target);
	void onHoldStop(ipengine::ipid source, ipengine::ipid target);
};

extern "C" BOOST_SYMBOL_EXPORT GameLogicModule module;
GameLogicModule module;

#endif