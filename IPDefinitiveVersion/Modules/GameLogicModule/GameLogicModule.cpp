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

void GameLogicModule::update(ipengine::TaskContext& c)
{
	if (!initialized)
	{
		initialized = true;
		if (contentmodule->getEntityByName("Camera"))
			graphics->setCameraEntity(contentmodule->getEntityByName("Camera")->m_entityId);
		if (contentmodule->getEntityByName("MemeEntity2"))
			contentmodule->getEntityByName("MemeEntity2")->m_transformData.setData()->m_location = { 4,1,0 };
		osc = std::acos(-1);
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
	auto pi = std::acos(-1);
	osc += (pi/120.f)*modifier;
	if (osc > 2.f*pi)
	{
		osc -= 2.f*pi;
	}
	mouseDelta = glm::vec2(0, 0);
}

void GameLogicModule::keyUpdate(IInput::Input &i)
{
	auto cam = contentmodule->getEntityByName("Camera");
	static bool trythis = true;
	if (i.data.kd.keycode == IInput::SCANCODE_Q && i.data.kd.state == IInput::ButtonState::BUTTON_UP &&!i.data.kd.isrepeat)
	{
		trythis = false;
		auto pitchquat = glm::quat(glm::vec3(0, 0, 0));
		auto yawquat = glm::quat(glm::vec3(0, glm::radians(45.f), 0));
		auto newrot = glm::normalize(yawquat*cam->m_transformData.getData()->m_rotation *pitchquat);
		cam->m_transformData.setData()->m_rotation = newrot;
		cam->m_transformData.setData()->m_isMatrixDirty = true;

		//auto transdata = cam->m_transformData.getData();
		//glm::mat4 tmat = glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
		//data->m_transformMatrix = tmat;//glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
		//data->m_isMatrixDirty = true;
		//data->m_localX = glm::normalize(glm::vec3(tmat[0][0], tmat[0][1], tmat[0][2]));
		//data->m_localY = glm::normalize(glm::vec3(tmat[1][0], tmat[1][1], tmat[1][2]));
		//data->m_localZ = glm::normalize(glm::vec3(tmat[2][0], tmat[2][1], tmat[2][2]));	
	}
	if (i.data.kd.keycode == IInput::SCANCODE_R && i.data.kd.state == IInput::ButtonState::BUTTON_UP &&!i.data.kd.isrepeat)
	{
		trythis = false;
		auto pitchquat = glm::quat(glm::vec3(0, 0, 0));
		auto yawquat = glm::quat(glm::vec3(0, glm::radians(-45.f), 0));
		auto newrot = glm::normalize(yawquat*cam->m_transformData.getData()->m_rotation *pitchquat);
		cam->m_transformData.setData()->m_rotation = newrot;
		cam->m_transformData.setData()->m_isMatrixDirty = true;

		//auto transdata = cam->m_transformData.getData();
		//glm::mat4 tmat = glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
		//data->m_transformMatrix = tmat;//glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
		//data->m_isMatrixDirty = true;
		//data->m_localX = glm::normalize(glm::vec3(tmat[0][0], tmat[0][1], tmat[0][2]));
		//data->m_localY = glm::normalize(glm::vec3(tmat[1][0], tmat[1][1], tmat[1][2]));
		//data->m_localZ = glm::normalize(glm::vec3(tmat[2][0], tmat[2][1], tmat[2][2]));	
	}
	if (i.data.kd.keycode == IInput::SCANCODE_E && i.data.kd.state == IInput::ButtonState::BUTTON_UP &&!i.data.kd.isrepeat)
	{
		auto data = cam->m_transformData.getData();

		std::cout << data->m_localX.x << " | " << data->m_localX.y << " | " << data->m_localX.z;
		std::cout << "\n";
		std::cout << data->m_localY.x << " | " << data->m_localY.y << " | " << data->m_localY.z;
		std::cout << "\n";
		std::cout << data->m_localZ.x << " | " << data->m_localZ.y << " | " << data->m_localZ.z;
		std::cout << "\n=\n";

	}
	if (i.data.kd.state == IInput::ButtonState::BUTTON_DOWN && !i.data.kd.isrepeat)
	{
		if (i.data.kd.keycode == IInput::SCANCODE_W)
		{
			camVelocity -= glm::vec3(0,0,1);
			if (camVelocity.z < -1)
			{
				camVelocity.z = -1;
			}
			w = true;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_S)
		{
			camVelocity += glm::vec3(0, 0, 1);
			if (camVelocity.z > 1)
			{
				camVelocity.z = 1;
			}
			s = true;
			//cam->m_transformData.setData()->m_location += cam->m_transformData.setData()->m_localZ*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_A)
		{
			camVelocity -= glm::vec3(1, 0, 0);
			if (camVelocity.x < -1)
			{
				camVelocity.x = -1;
			}
			a = true;
			//cam->m_transformData.setData()->m_location -= cam->m_transformData.setData()->m_localX*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_D)
		{
			camVelocity += glm::vec3(1, 0, 0);
			if (camVelocity.x > 1)
			{
				camVelocity.x = 1;
			}
			d = true;
			//cam->m_transformData.setData()->m_location += cam->m_transformData.setData()->m_localX*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_SPACE)
		{
			box = !box;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_F)
		{
			yc = true;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_V)
		{
			vc = true;
		}
	}
	if (i.data.kd.state == IInput::ButtonState::BUTTON_UP)
	{
		if (i.data.kd.keycode == IInput::SCANCODE_W)
		{
			camVelocity += glm::vec3(0, 0, 1);
			if (camVelocity.z > 1)
			{
				camVelocity.z = 1;
			}
			w = false;
			//cam->m_transformData.setData()->m_location -= cam->m_transformData.setData()->m_localZ*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_S)
		{
			camVelocity -= glm::vec3(0, 0, 1);
			if (camVelocity.z < -1)
			{
				camVelocity.z = -1;
			}
			s = false;
			//cam->m_transformData.setData()->m_location += cam->m_transformData.setData()->m_localZ*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;

		}
		else if (i.data.kd.keycode == IInput::SCANCODE_A)
		{
			camVelocity += glm::vec3(1, 0, 0);
			if (camVelocity.x > 1)
			{
				camVelocity.x = 1;
			}
			a = false;
			//cam->m_transformData.setData()->m_location -= cam->m_transformData.setData()->m_localX*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;

		}
		else if (i.data.kd.keycode == IInput::SCANCODE_D)
		{
			camVelocity -= glm::vec3(1, 0, 0);
			if (camVelocity.x < -1)
			{
				camVelocity.x = -1;
			}
			d = false;
			//cam->m_transformData.setData()->m_location += cam->m_transformData.setData()->m_localX*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;

		}
		else if (i.data.kd.keycode == IInput::SCANCODE_SPACE)
		{
			box = !box;
			//cam->m_transformData.setData()->m_location += cam->m_transformData.setData()->m_localX*(float)modifier;
			//cam->m_transformData.setData()->m_isMatrixDirty = true;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_F)
		{
			yc = false;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_V)
		{
			vc = false;
		}
	}
}

void GameLogicModule::mousemoveUpdate(IInput::Input &i)
{
	//mouseDelta += glm::vec2(lastMouseMove.data.md.x, lastMouseMove.data.md.y) - glm::vec2(i.data.md.x, i.data.md.y);
	mouseDelta += glm::vec2(i.data.md.rx, i.data.md.ry);
	//std::cout << mouseDelta.x;
	lastMouseMove = IInput::Input(i);
}

void GameLogicModule::mousescrollUpdate(IInput::Input &i)
{
}

void GameLogicModule::entityUpdate(SCM::Entity *e)
{
	if (e->m_name == "Camera" && mouseDelta!=glm::vec2(0,0))
	{
		//yaw = std::fmod(yaw + mouseDelta.x*0.01, 360);
		//yaw = (int)(yaw + mouseDelta.x*0.1) % 360;

		auto pitchquat = glm::quat(glm::vec3(0, 0, 0));
		auto ydelta = mouseDelta.y*0.1;
		if (pitch + ydelta> 90)
		{
			ydelta = 90 - pitch;
			pitch = 90;
		}
		else if (pitch + ydelta < -90)
		{
			ydelta = -90 - pitch;
			pitch = -90;
		}
		else
		{
			pitch += ydelta;
		}
		pitchquat = glm::quat(glm::vec3(glm::radians(-ydelta), 0, 0));
		//pitchquat = glm::quat(glm::vec3(0, 0, 0));
		auto yawquat = glm::quat(glm::vec3(0, glm::radians(-mouseDelta.x*0.1), 0));
		auto newrot = glm::normalize(yawquat * e->m_transformData.getData()->m_rotation * pitchquat);
		e->m_transformData.setData()->m_rotation = newrot;
		e->m_transformData.setData()->m_isMatrixDirty = true;
	}
	

	if (e->m_name == "Camera" && !box)//&& camVelocity != glm::vec3(0, 0, 0))
	{

		//auto x = e->m_transformData.getData()->m_localX*(float)camVelocity.x*(float)modifier;
		//auto y = e->m_transformData.getData()->m_localZ*camVelocity.z*(float)modifier;

		auto 	x = e->m_transformData.getData()->m_localX*(float)(((a ? -1 : 0 )+(d ? 1 : 0))*modifier);
		auto	y = e->m_transformData.getData()->m_localZ*(float)(((w ? -1 : 0 )+ (s ? 1 : 0))*modifier);
		auto z = e->m_transformData.getData()->m_localY*(float)(((vc ? -1 : 0) + (yc ? 1 : 0))*modifier);
		if (glm::length(x) != 0 || glm::length(y) !=0 || glm::length(z) != 0)
		{
			e->m_transformData.setData()->m_location += x * 0.1f + y * 0.1f + 0.1f*z;
			e->m_transformData.setData()->m_isMatrixDirty = true;
		}
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
		e->swap();
		e->m_transformData.setData()->m_isMatrixDirty = false;
	}
}

void GameLogicModule::entity3dUpdate(SCM::ThreeDimEntity *e)
{

	std::string tar = "MemeEntity1";

	if (e->m_name == tar && box)//&& camVelocity != glm::vec3(0, 0, 0))
	{

		//auto x = e->m_transformData.getData()->m_localX*(float)camVelocity.x*(float)modifier;
		//auto y = e->m_transformData.getData()->m_localZ*camVelocity.z*(float)modifier;
		auto cam = contentmodule->getEntityByName("Camera");
		auto camtrans = cam->m_transformData.getData();
		auto 	x = camtrans->m_localX*(float)(((a ? -1 : 0) + (d ? 1 : 0))*modifier);
		auto	y = camtrans->m_localZ*(float)(((w ? -1 : 0) + (s ? 1 : 0))*modifier);
		auto z = camtrans->m_localY*(float)(((vc ? -1 : 0) + (yc ? 1 : 0))*modifier);

		if (glm::length(x) != 0 || glm::length(y) != 0 || glm::length(z) != 0)
		{
			e->m_transformData.setData()->m_location += x * 0.1f + y * 0.1f+ z * 0.1f;
			e->m_transformData.setData()->m_isMatrixDirty = true;
		}
	}

	if (e->m_name == "MemeEntity2")//&& camVelocity != glm::vec3(0, 0, 0))
	{
		auto mod = std::sin(osc);
		//std::cout << mod <<  "\n";
		auto 	x = e->m_transformData.getData()->m_localX*(float)((mod*modifier*1.f));
		auto	y = e->m_transformData.getData()->m_localZ*(float)((mod*modifier*1.f));
		//auto z = e->m_transformData.getData()->m_localY*(float)(((vc ? -1 : 0) + (yc ? 1 : 0))*modifier);

		if (glm::length(x) != 0 || glm::length(y) != 0)
		{
			e->m_transformData.setData()->m_location += y * 0.1f;// +y * 0.1f;
			e->m_transformData.setData()->m_isMatrixDirty = true;
		}
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
		e->swap();
		e->m_transformData.setData()->m_isMatrixDirty = false;

	}
}

bool GameLogicModule::_startup()
{
	//Initialize your module
	ipengine::Scheduler& sched = m_core->getScheduler();
	handles.push_back(sched.subscribe(ipengine::TaskFunction::make_func<GameLogicModule, &GameLogicModule::update>(this), timing.nano(), ipengine::Scheduler::SubType::Interval, 1, &m_core->getThreadPool(), true));
	contentmodule = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
	if (m_info.dependencies.exists("input"))
	{
		inputmodule = m_info.dependencies.getDep<IInput_API>("input");
	}
	graphics = m_info.dependencies.getDep<IGraphics_API>("graphics");
	if (contentmodule->getEntityByName("Camera"))
		graphics->setCameraEntity(contentmodule->getEntityByName("Camera")->m_entityId);
	lastUpdate = ipengine::Time(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	return true;
}
