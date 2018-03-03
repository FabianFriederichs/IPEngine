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

class GameLogicModule : public IModule_API {
public:
	GameLogicModule();
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
private:
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

	void keyUpdate(IInput::Input&);
	void mousemoveUpdate(IInput::Input&);
	void mousescrollUpdate(IInput::Input&);
	void entityUpdate(SCM::Entity*);
	void entity3dUpdate(SCM::ThreeDimEntity*);
	void updateBoundingData(SCM::Entity*, const glm::vec3&, const glm::vec3&, float);

	// Inherited via IModule_API
	virtual bool _startup() override;

	// Inherited via IModule_API
	virtual bool _shutdown() override;
};

extern "C" BOOST_SYMBOL_EXPORT GameLogicModule module;
GameLogicModule module;

#endif