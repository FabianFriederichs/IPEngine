// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "GameLogicModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
GameLogicModule::GameLogicModule()
{
	m_info.identifier = "GameLogicModule";
	m_info.version = "1.0";
	m_info.iam = "IModule_API"; //Change this to your used API
	return;
}


bool GameLogicModule::startUp()
{
	//Initialize your module
	ipengine::Scheduler& sched = m_core->getScheduler();
	handles.push_back(sched.subscribe(ipengine::TaskFunction::make_func<GameLogicModule, &GameLogicModule::update>(this),timing.nano(), ipengine::Scheduler::SubType::Interval, 1, &m_core->getThreadPool(), false));
	contentmodule = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
	if (m_info.dependencies.exists("input")) 
	{
		inputmodule = m_info.dependencies.getDep<IInput_API>("input");
	}
	graphics = m_info.dependencies.getDep<IGraphics_API>("graphics");
	if(contentmodule->getEntityByName("Camera"))
		graphics->setCameraEntity(contentmodule->getEntityByName("Camera")->m_entityId);
	lastUpdate = ipengine::Time(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	return true;
}

void GameLogicModule::update(ipengine::TaskContext& c)
{
	if (!initialized)
	{
		initialized = true;
		if (contentmodule->getEntityByName("Camera"))
			graphics->setCameraEntity(contentmodule->getEntityByName("Camera")->m_entityId);
	}
	modifier = delta.mic() / timing.mic();
	auto now = std::chrono::high_resolution_clock::now();
	auto nowc = now.time_since_epoch().count();
	delta = ipengine::Time(nowc - lastUpdate.nano());
	//std::cout << delta.mil(); std::cout << "   ";
	auto past = lastInputTimestamp.nano() > 1000 ? lastInputTimestamp : lastUpdate.nano();
	std::vector<IInput::Input> input = inputmodule->getInputBuffered(past);
	lastUpdate = ipengine::Time(now.time_since_epoch().count());

	auto& extrec = m_info.expoints;
	std::vector<ipengine::any> anyvector;
	anyvector.push_back(static_cast<IModule_API*>(this));
	anyvector.push_back(&delta);
	extrec.execute("preInputHandling", { "this"	, "delta" }, anyvector);
	anyvector.clear();
	for (auto i : input)
	{
		
		anyvector.push_back(static_cast<IModule_API*>(this));
		anyvector.push_back(&i);
		anyvector.push_back(&delta);
		std::vector<std::string> paras = { "this", "input", "delta" };
		extrec.execute("input", paras, anyvector);
		switch (i.type)
		{
		case IInput::InputType::INPUT_KEY:
			extrec.execute("key", paras, anyvector);
			keyUpdate(i);
			break;
		case IInput::InputType::INPUT_MOUSEMOVE:
			extrec.execute("mousemove", paras, anyvector);
			mousemoveUpdate(i);
			break;
		case IInput::InputType::INPUT_MOUSESCROLL:
			extrec.execute("mousescroll", paras, anyvector);
			mousescrollUpdate(i);
			break;
		
		}
		anyvector.clear();
	}
	if (input.size() > 0)
	{
		lastInputTimestamp = input.back().timeStamp.nano()+1;
	}
	anyvector.push_back(static_cast<IModule_API*>(this));
	anyvector.push_back(&delta);
	extrec.execute("postInputHandling", { "this", "delta" }, anyvector);
	anyvector.clear();

	auto& threde = contentmodule->getThreeDimEntities();
	for (auto &e : contentmodule->getEntities())
	{
		if (threde.count(e.second->m_entityId))
		{
			entity3dUpdate(static_cast<SCM::ThreeDimEntity*>(e.second));
		}
		else
		{
			entityUpdate(e.second);
		}
	}
}

void GameLogicModule::keyUpdate(IInput::Input &i)
{
	auto cam = contentmodule->getEntityByName("Camera");
	if (i.data.kd.state == IInput::ButtonState::BUTTON_DOWN && !i.data.kd.isrepeat)
	{
		if (i.data.kd.keycode == IInput::SCANCODE_W)
		{
			camVelocity -= cam->m_transformData.setData()->m_localZ;
			//cam->m_transformData.setData()->m_location -= cam->m_transformData.setData()->m_localZ*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_S)
		{
			camVelocity += cam->m_transformData.setData()->m_localZ;

			//cam->m_transformData.setData()->m_location += cam->m_transformData.setData()->m_localZ*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;

		}
		else if (i.data.kd.keycode == IInput::SCANCODE_A)
		{
			camVelocity -= cam->m_transformData.setData()->m_localX;

			//cam->m_transformData.setData()->m_location -= cam->m_transformData.setData()->m_localX*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;

		}
		else if (i.data.kd.keycode == IInput::SCANCODE_D)
		{
			camVelocity += cam->m_transformData.setData()->m_localX;

			//cam->m_transformData.setData()->m_location += cam->m_transformData.setData()->m_localX*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;

		}
	}
	else if (i.data.kd.state == IInput::ButtonState::BUTTON_UP)
	{
		if (i.data.kd.keycode == IInput::SCANCODE_W)
		{
			camVelocity += cam->m_transformData.setData()->m_localZ;
			//cam->m_transformData.setData()->m_location -= cam->m_transformData.setData()->m_localZ*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_S)
		{
			camVelocity -= cam->m_transformData.setData()->m_localZ;

			//cam->m_transformData.setData()->m_location += cam->m_transformData.setData()->m_localZ*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;

		}
		else if (i.data.kd.keycode == IInput::SCANCODE_A)
		{
			camVelocity += cam->m_transformData.setData()->m_localX;

			//cam->m_transformData.setData()->m_location -= cam->m_transformData.setData()->m_localX*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;

		}
		else if (i.data.kd.keycode == IInput::SCANCODE_D)
		{
			camVelocity -= cam->m_transformData.setData()->m_localX;

			//cam->m_transformData.setData()->m_location += cam->m_transformData.setData()->m_localX*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;

		}
	}
}

void GameLogicModule::mousemoveUpdate(IInput::Input &i)
{
}

void GameLogicModule::mousescrollUpdate(IInput::Input &i)
{
}

void GameLogicModule::entityUpdate(SCM::Entity *e)
{
	if (e->m_name == "Camera"&& camVelocity != glm::vec3(0, 0, 0))
	{
		e->m_transformData.setData()->m_location += camVelocity*(float)modifier;
		e->m_transformData.setData()->m_isMatrixDirty = true;
	}
	if (e->m_transformData.setData()->m_isMatrixDirty)
	{
		
		auto transdata = e->m_transformData.getData();
		glm::mat4 tmat = glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
		auto data = e->m_transformData.setData();
		data->m_transformMatrix = tmat;//glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
		data->m_isMatrixDirty = false;
		data->m_localX = glm::normalize(glm::vec3(tmat[0][0], tmat[0][1], tmat[0][2]));
		data->m_localY = glm::normalize(glm::vec3(tmat[1][0], tmat[1][1], tmat[1][2]));
		data->m_localZ = glm::normalize(glm::vec3(tmat[2][0], tmat[2][1], tmat[2][2]));
	}
}

void GameLogicModule::entity3dUpdate(SCM::ThreeDimEntity *e)
{
	if (e->m_transformData.setData()->m_isMatrixDirty)
	{
		auto transdata = e->m_transformData.getData();
		glm::mat4 tmat = glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
		auto data = e->m_transformData.setData();
		data->m_transformMatrix = tmat;//glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
		data->m_isMatrixDirty = false;
		data->m_localX = glm::normalize(glm::vec3(tmat[0][0], tmat[0][1], tmat[0][2]));
		data->m_localY = glm::normalize(glm::vec3(tmat[1][0], tmat[1][1], tmat[1][2]));
		data->m_localZ = glm::normalize(glm::vec3(tmat[2][0], tmat[2][1], tmat[2][2]));
	}
}
