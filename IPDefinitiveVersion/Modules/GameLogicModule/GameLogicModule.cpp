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
	BEGINEX
	if (!initialized)
	{
		initialized = true;
		
		osc = std::acos(-1);
	}
	if (cameraid == IPID_INVALID)
	{
		auto caments = contentmodule->getEntitiesByName("Camera");
		if (!caments.empty())
		{
			graphics = m_info.dependencies.getDep<IGraphics_API>("graphics");

			cameraid = caments.front()->m_entityId;
			graphics->setCameraEntity(cameraid);
		}
	}
	else if (!contentmodule->getEntityById(cameraid)->isActive)
	{
	auto caments = contentmodule->getEntitiesByName("Camera");
	for (auto e : caments)
	{
		if (e->isActive)
		{
			cameraid = e->m_entityId;
			graphics = m_info.dependencies.getDep<IGraphics_API>("graphics");

			graphics->setCameraEntity(cameraid);
		}
	}
	}
	if (hmdid == IPID_INVALID)
	{
		auto caments = contentmodule->getEntitiesByName("OpenVRHMD");
		if (!caments.empty())
			hmdid = caments.front()->m_entityId;
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
			if(mousecamera)
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

	m_gameLogicEndpoint->dispatch();
	ENDEX(m_errhandler)
}

void GameLogicModule::keyUpdate(IInput::Input &i)
{
	auto cam = contentmodule->getEntityById(cameraid);
	static bool trythis = true;
	if (i.data.kd.keycode == IInput::SCANCODE_Q && i.data.kd.state == IInput::ButtonState::BUTTON_UP &&!i.data.kd.isrepeat)
	{
		trythis = false;
		auto pitchquat = glm::quat(glm::vec3(0, 0, 0));
		auto yawquat = glm::quat(glm::vec3(0, glm::radians(45.f), 0));
		cam->m_transformData.rotate(yawquat);
		cam->m_transformData.rotateLocal(pitchquat);
		/*auto newrot = glm::normalize(yawquat*cam->m_transformData.getData()->m_rotation *pitchquat);
		cam->m_transformData.setData()->m_rotation = newrot;
		cam->m_transformData.setData()->m_isMatrixDirty = true;*/

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
		cam->m_transformData.rotate(yawquat);
		cam->m_transformData.rotateLocal(pitchquat);
		/*auto newrot = glm::normalize(yawquat*cam->m_transformData.getData()->m_rotation *pitchquat);
		cam->m_transformData.setData()->m_rotation = newrot;
		cam->m_transformData.setData()->m_isMatrixDirty = true;*/

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
		auto& data = contentmodule->getEntityById(hmdid)->m_transformData;
		std::cout << data.getLocalPosition().x << " | " << data.getLocalPosition().y << " | " << data.getLocalPosition().z;
		std::cout << "\n";
		auto &data2 = contentmodule->getEntityById(cameraid)->m_transformData;
		std::cout << data.getLocalPosition().x << " | " << data.getLocalPosition().y << " | " << data.getLocalPosition().z;
		std::cout << "\n";
		/*std::cout << data->m_localX.x << " | " << data->m_localX.y << " | " << data->m_localX.z;
		std::cout << "\n";
		std::cout << data->m_localY.x << " | " << data->m_localY.y << " | " << data->m_localY.z;
		std::cout << "\n";
		std::cout << data->m_localZ.x << " | " << data->m_localZ.y << " | " << data->m_localZ.z;
		std::cout << "\n=\n";*/

	}
	if (i.data.kd.state == IInput::ButtonState::BUTTON_DOWN && !i.data.kd.isrepeat)
	{
		if (i.data.kd.keycode == IInput::SCANCODE_W || i.data.kd.keycode==IInput::SCANCODE_VRBUTTON_DPAD_UP)
		{
			camVelocity -= glm::vec3(0,0,1);
			if (camVelocity.z < -1)
			{
				camVelocity.z = -1;
			}
			w = true;
		}
		else if (i.data.kd.keycode == IInput::SCANCODE_S || i.data.kd.keycode == IInput::SCANCODE_VRBUTTON_DPAD_DOWN)
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
		else if (i.data.kd.keycode == IInput::SCANCODE_A || i.data.kd.keycode == IInput::SCANCODE_VRBUTTON_DPAD_LEFT)
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
		else if (i.data.kd.keycode == IInput::SCANCODE_D || i.data.kd.keycode == IInput::SCANCODE_VRBUTTON_DPAD_RIGHT)
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
		else if (i.data.kd.keycode == IInput::SCANCODE_VRBUTTON_TRIGGER)
		{
			IInput::VRDevices role = IInput::VRDevices::NONE;
			if ((role = inputmodule->getDeviceFromIndex(i.data.kd.deviceIndex)) == IInput::VRDevices::CONTROLLER_LEFT)
			{
				isHoldEntityButton1Pressed[0] = isHoldEntityButton1Pressed[1];
				isHoldEntityButton1Pressed[1] = true;
			}
			else if (role == IInput::VRDevices::CONTROLLER_RIGHT)
			{
				isHoldEntityButton2Pressed[0] = isHoldEntityButton2Pressed[1];
				isHoldEntityButton2Pressed[1] = true;
			}
		}
	}
	if (i.data.kd.state == IInput::ButtonState::BUTTON_UP)
	{
		if (i.data.kd.keycode == IInput::SCANCODE_W || i.data.kd.keycode == IInput::SCANCODE_VRBUTTON_DPAD_UP)
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
		else if (i.data.kd.keycode == IInput::SCANCODE_S || i.data.kd.keycode == IInput::SCANCODE_VRBUTTON_DPAD_DOWN)
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
		else if (i.data.kd.keycode == IInput::SCANCODE_A || i.data.kd.keycode == IInput::SCANCODE_VRBUTTON_DPAD_LEFT)
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
		else if (i.data.kd.keycode == IInput::SCANCODE_D || i.data.kd.keycode == IInput::SCANCODE_VRBUTTON_DPAD_RIGHT)
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
		else if (i.data.kd.keycode == IInput::SCANCODE_VRBUTTON_TRIGGER)
		{
			IInput::VRDevices role = IInput::VRDevices::NONE;
			if ((role = inputmodule->getDeviceFromIndex(i.data.kd.deviceIndex)) == IInput::VRDevices::CONTROLLER_LEFT)
			{
				isHoldEntityButton1Pressed[0] = isHoldEntityButton1Pressed[1];
				isHoldEntityButton1Pressed[1] = false;
				onHoldStop(holder[0], heldEntity[0]);
				holder1busy = false;
				heldEntity[0] = IPID_INVALID;
			}
			else if (role == IInput::VRDevices::CONTROLLER_RIGHT)
			{
				isHoldEntityButton2Pressed[0] = isHoldEntityButton2Pressed[1];
				isHoldEntityButton2Pressed[1] = false;
				onHoldStop(holder[1], heldEntity[1]);
				holder2busy = false;
				heldEntity[1] = IPID_INVALID;
			}
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
		e->m_transformData.rotate(yawquat);
		e->m_transformData.rotateLocal(pitchquat);
		/*auto newrot = glm::normalize(yawquat * e->m_transformData.getData()->m_rotation * pitchquat);
		e->m_transformData.setData()->m_rotation = newrot;
		e->m_transformData.setData()->m_isMatrixDirty = true;*/
		e->boundingDataDirty = true;
	}
	

	if (e->m_name == "Camera" && !box)//&& camVelocity != glm::vec3(0, 0, 0))
	{

		//auto x = e->m_transformData.getData()->m_localX*(float)camVelocity.x*(float)modifier;
		//auto y = e->m_transformData.getData()->m_localZ*camVelocity.z*(float)modifier;
		SCM::Transform* trans;

		
		trans = &e->m_transformData;
		auto 	x = (float)(((a ? -1 : 0 )+(d ? 1 : 0))*modifier);
		auto	z = (float)(((w ? -1 : 0 )+ (s ? 1 : 0))*modifier);
		auto y = (float)(((vc ? -1 : 0) + (yc ? 1 : 0))*modifier);

			

		if (glm::length(x) != 0 || glm::length(y) !=0 || glm::length(z) != 0)
		{
			e->m_transformData.translateLocal(glm::vec3(x, y, z) * 0.1f);
			if (e->m_transformData.getWorldPosition().y < minimum_y)
				e->m_transformData.translateWorld(glm::vec3(0.0f, minimum_y - e->m_transformData.getWorldPosition().y, 0.0f));
			//e->m_transformData.setData()->m_isMatrixDirty = true;
			e->boundingDataDirty = true;
		}
	}

	


	if (e->shouldCollide() && e->boundingDataDirty)
	{
	}
}

void GameLogicModule::entity3dUpdate(SCM::ThreeDimEntity *e)
{

	std::string tar = "MemeEntity1";

	if (e->m_name == tar && box)//&& camVelocity != glm::vec3(0, 0, 0))
	{

		//auto x = e->m_transformData.getData()->m_localX*(float)camVelocity.x*(float)modifier;
		//auto y = e->m_transformData.getData()->m_localZ*camVelocity.z*(float)modifier;
		auto cam = contentmodule->getEntityById(cameraid);
		auto& camtrans = cam->m_transformData;
		auto 	x = camtrans.getLocalXAxis()*(float)(((a ? -1 : 0) + (d ? 1 : 0))*modifier);
		auto	z = camtrans.getLocalZAxis()*(float)(((w ? -1 : 0) + (s ? 1 : 0))*modifier);
		auto y = camtrans.getLocalYAxis()*(float)(((vc ? -1 : 0) + (yc ? 1 : 0))*modifier);

		if (glm::length(x) != 0 || glm::length(y) != 0 || glm::length(z) != 0)
		{
			e->m_transformData.translateWorld(x * 0.1f + y * 0.1f+ z * 0.1f);
			/*e->m_transformData.setData()->m_isMatrixDirty = true;*/
			e->boundingDataDirty = true;
		}
	}

	if (e->m_name == "MemeEntity2" && automovebox)//&& camVelocity != glm::vec3(0, 0, 0))
	{
		auto mod = std::sin(osc);
		//std::cout << mod <<  "\n";
		auto 	x = e->m_transformData.getLocalXAxis()*(float)((mod*modifier*1.f));
		auto	y = e->m_transformData.getLocalZAxis()*(float)((mod*modifier*1.f));
		//auto z = e->m_transformData.getData()->m_localY*(float)(((vc ? -1 : 0) + (yc ? 1 : 0))*modifier);

		if (glm::length(x) != 0 || glm::length(y) != 0)
		{
			e->m_transformData.translateWorld(y * 0.1f);// +y * 0.1f;
			//e->m_transformData.setData()->m_isMatrixDirty = true;
			e->boundingDataDirty = true;
		}
	}

	if (e->m_name == holder1Name)
	{
		if (holder[0] == IPID_INVALID)
			holder[0] = e->m_entityId;
	}
	else if (e->m_name == holder2Name)
	{
		if (holder[1] == IPID_INVALID)
			holder[1] = e->m_entityId;
	}
	else
	{
		if (isHoldEntityButton1Pressed[0] != isHoldEntityButton1Pressed[1] && !holder1busy)
		{
			if (isHoldEntityButton1Pressed[1])
			{
				/*if (inProximity(e->m_entityId, holder[0], 0.1f))
				{
					holder1busy = true;
					heldEntity[0] = e->m_entityId;
					onHoldStart(holder[0], e->m_entityId);
				}*/
			}
		}
		if (isHoldEntityButton2Pressed[0] != isHoldEntityButton2Pressed[1] && !holder2busy)
		{
			if (isHoldEntityButton2Pressed[1])
			{
				/*if (inProximity(e->m_entityId, holder[1], 0.1f))
				{
					holder2busy = true;
					heldEntity[1] = e->m_entityId;
					onHoldStart(holder[1], e->m_entityId);
				}*/
			}
		}
	}

	if (e->shouldCollide() && e->boundingDataDirty)
	{
		//glm::vec3 oldpos = glm::vec3(e->m_transformData.getData()->m_transformMatrix[3]);
		//auto transdata = e->m_transformData.getData();
		//e->m_transformData.setData()->updateTransform();
		//glm::mat4 tmat = glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
		//auto data = e->m_transformData.setData();
		//data->m_transformMatrix = tmat;//glm::translate(transdata->m_location) * glm::toMat4(transdata->m_rotation) * glm::scale(transdata->m_scale);
		//data->m_isMatrixDirty = false;
		//data->m_localX = glm::normalize(glm::vec3(tmat[0][0], tmat[0][1], tmat[0][2]));
		//data->m_localY = glm::normalize(glm::vec3(tmat[1][0], tmat[1][1], tmat[1][2]));
		//data->m_localZ = glm::normalize(glm::vec3(tmat[2][0], tmat[2][1], tmat[2][2]));
		////e->swap();
		//e->m_transformData.setData()->m_isMatrixDirty = false;
		//
		//e->updateBoundingData(oldpos, e->m_transformData.getData()->m_location, static_cast<float>(delta.sec()));
		e->updateBoundingData(static_cast<float>(delta.sec()));
	}
}

//void GameLogicModule::updateBoundingData(SCM::Entity * entity, const glm::vec3& oldpos, const glm::vec3& newpos, float deltasecs)
//{
//	if (entity->shouldCollide())
//	{
//		if (entity->isBoundingBox)
//		{
//			glm::mat4 bbtoentity = glm::translate(entity->m_boundingData.box.m_center) * glm::mat4(entity->m_boundingData.box.m_rotation) * glm::scale(entity->m_boundingData.box.m_size * 0.5f);
//			entity->m_boundingData.box.bdtoworld = entity->m_transformData.getLocalToWorldMatrix() * bbtoentity;
//			entity->m_boundingData.box.m_velocity = (newpos - oldpos) / deltasecs;
//		}
//		else
//		{
//			glm::mat4 bstoentity = glm::translate(entity->m_boundingData.sphere.m_center);
//			entity->m_boundingData.sphere.bdtoworld = entity->m_transformData.getLocalToWorldMatrix() * bstoentity;
//			entity->m_boundingData.sphere.m_velocity = (newpos - oldpos) / deltasecs;
//		}
//	}
//}

void GameLogicModule::messageCallback(ipengine::Message & msg)
{
	if (msg.type == m_collisionmsgtype)
	{
		IPhysicsModule_API::Collision col = msg.payload;
		std::cout << "Collision! Cloth " << col.e1 << " collided with " << col.e2 << "\n";
	}
}

void GameLogicModule::onError(ipengine::ipex & ex)
{
	//do stuff
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
	if (cameraid == IPID_INVALID)
	{
		auto caments = contentmodule->getEntitiesByName("Camera");
		if (!caments.empty())
			cameraid = caments.front()->m_entityId;
	}
	if (cameraid != IPID_INVALID)
	{
		graphics->setCameraEntity(contentmodule->getEntityById(cameraid)->m_entityId);
	}
	lastUpdate = ipengine::Time(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	//messaging test
	m_gameLogicEndpoint = m_core->getEndpointRegistry().createEndpoint("GAME_LOGIC_ENDPOINT");
	//m_gameLogicEndpoint->connectTo(m_core->getEndpointRegistry().getEndpoint("PHYSICS_MODULE_ENDPOINT"));
	m_gameLogicEndpoint->registerCallback(ipengine::MessageCallback::make_func<GameLogicModule, &GameLogicModule::messageCallback>(this));
	m_collisionmsgtype = m_core->getEndpointRegistry().getMessageTypeByName("CLOTH_OBJECT_COLLISION");

	m_errhandler = m_core->getErrorManager().createHandlerInstance();
	m_errhandler.registerCustomHandler(ipengine::ErrorHandlerFunc::make_func<GameLogicModule, &GameLogicModule::onError>(this));
	mousecamera = m_core->getConfigManager().getBool("GameLogic.Camera.mouse_move");
	minimum_y = m_core->getConfigManager().getFloat("GameLogic.Camera.minimum_y");
	return true;
}

bool GameLogicModule::_shutdown()
{
	//!TODO
	for (auto& handle : handles)
	{
		handle.~SubHandle();
	}
	initialized = false;
	return true;
}

ipengine::ipid GameLogicModule::getAncestor(ipengine::ipid child)
{
	auto ent = contentmodule->getEntityById(child);
	if (ent)
	{
		while (ent->m_parent)
		{
			ent = ent->m_parent;
		}
		return ent->m_entityId;
	}
	return IPID_INVALID;
}

//bool GameLogicModule::inProximity(ipengine::ipid source, ipengine::ipid target, float maxDistance)
//{
//	auto entsource = contentmodule->getEntityById(source);
//	auto enttarget = contentmodule->getEntityById(target);
//	if (entsource && enttarget && entsource->shouldCollide() && enttarget->shouldCollide())
//	{
//		float distance,minx,miny,maxx,maxy, minz, maxz;
//		if (enttarget->isBoundingBox)
//		{
//			auto box = entsource->m_boundingData.box;
//			auto targetcenter = enttarget->m_boundingData.box.m_center + enttarget->m_transformData.getData()->m_location;
//			auto sourcecenter = entsource->m_boundingData.sphere.m_center + enttarget->m_transformData.getData()->m_location;
//			auto halfx = box.m_size.x / 2;
//			auto halfy = box.m_size.y / 2;
//			auto halfz = box.m_size.z / 2;
//			minx = sourcecenter.x - halfx;
//			maxx = sourcecenter.x + halfx;
//			miny = sourcecenter.y - halfy;
//			maxy = sourcecenter.y + halfy;
//			minz = sourcecenter.z - halfz;
//			maxz = sourcecenter.z + halfz;
//
//			auto dx = glm::max(minx - targetcenter.x, targetcenter.x - maxx);
//			auto dy = glm::max(miny - targetcenter.y, targetcenter.y - maxy);
//			auto dz = glm::max(minz - targetcenter.z, targetcenter.z - maxz);
//			distance = glm::sqrt(dx*dx + dy*dy + dz*dz);
//			//https://stackoverflow.com/a/18157551 source for this formula
//			return distance < maxDistance ? true : false; //! this has to be tested likely wrong
//		}
//		else
//		{
//			distance = glm::distance(entsource->m_boundingData.sphere.m_center + entsource->m_transformData.getData()->m_location,enttarget->m_boundingData.sphere.m_center + enttarget->m_transformData.getData()->m_location) - entsource->m_boundingData.sphere.m_radius;
//			return distance < maxDistance ? true : false;
//		}
//	}
//	return false;
//}

void GameLogicModule::onHoldStart(ipengine::ipid source, ipengine::ipid target)
{
	auto enttarget = contentmodule->getEntityById(target);
	auto entsource = contentmodule->getEntityById(source);
	if (enttarget && entsource && !enttarget->m_parent)
	{
		enttarget->setParent(entsource);

		//set location to be relative to parent
		/*enttarget->m_transformData.setData()->m_location = enttarget->m_transformData.getData()->m_location - entsource->m_transformData.getData()->m_location;
		enttarget->m_transformData.setData()->m_rotation = glm::normalize(glm::inverse(entsource->m_transformData.getData()->m_rotation) * enttarget->m_transformData.setData()->m_rotation);
		enttarget->m_transformData.setData()->m_isMatrixDirty = true;*/
		//enttarget->m_transformData.setData()->setTransformMatrix(entsource->m_transformData.getData()->getInverseTransform() * enttarget->m_transformData.getData()->m_transformMatrix);
	}
}

void GameLogicModule::onHoldStop(ipengine::ipid source, ipengine::ipid target)
{
	auto enttarget = contentmodule->getEntityById(target);
	auto entsource = contentmodule->getEntityById(source);
	if (enttarget && entsource && enttarget->m_parent)
	{
		/*auto targettrans = enttarget->m_transformData.setData();
		auto sourcetrans = entsource->m_transformData.getData();
		targettrans->m_location = targettrans->m_location + sourcetrans->m_location;
		targettrans->m_rotation = glm::normalize(sourcetrans->m_rotation*targettrans->m_rotation);
		targettrans->m_isMatrixDirty = true;
		auto newtargettrans = sourcetrans->m_transformMatrix * targettrans->m_transformMatrix;
		targettrans->m_location = glm::vec3(newtargettrans[3]);
		targettrans->m_rotation = glm::toQuat(glm::mat3(
			glm::normalize(glm::vec3(newtargettrans[0])),
			glm::normalize(glm::vec3(newtargettrans[1])),
			glm::normalize(glm::vec3(newtargettrans[2]))
		));
		targettrans->m_isMatrixDirty = true;*/
		//! todo adjust target transform rotation. Apply source rotation to target rotaiton

		enttarget->orphane();//;->m_parent = nullptr;
	}
}
