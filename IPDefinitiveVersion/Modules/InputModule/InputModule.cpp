// Plugin1.cpp : Defines the exported functions for the DLL application.
//
#include "InputModule.h"

//
//// This is an example of an exported variable
//PLUGIN1_API int nPlugin1=0;
//
//// This is an example of an exported function.
//PLUGIN1_API int fnPlugin1(void)
//{
//	return 42;
//}

// This is the constructor of a class that has been exported.
// see Plugin1.h for the class definition
InputModule::InputModule()
{
	m_info.identifier = "InputModule";
	m_info.version = "1.0";
	m_info.iam = "IInput_API";
	return;
}

void InputModule::pollDataC(ipengine::TaskContext& c)
{
	pollData();
}

bool InputModule::_startup()
{
	if (m_info.dependencies.exists("openvr"))
	{
		openvr = m_info.dependencies.getDep<IBasicOpenVRModule_API>("openvr");
		if (openvr->isConnected())
		{
			vrsys = openvr->getSystem();
			isVRconnected = true;
		}
	}
	//memes = std::thread([this]()->void{while (1){ pollData(); }});
	ipengine::Scheduler& sched = m_core->getScheduler();
	handles.push_back(sched.subscribe(ipengine::TaskFunction::make_func<InputModule, &InputModule::pollDataC>(this), 0, ipengine::Scheduler::SubType::Frame, 1, &m_core->getThreadPool(), true));
	SDL_SetRelativeMouseMode(SDL_TRUE);
	//SDL_CaptureMouse(SDL_TRUE);
	return true;
}

bool InputModule::_shutdown()
{
	for (auto& handle : handles)
	{
		handle.~SubHandle();
	}
	isVRconnected = false;
	inputData.clear();
	isManipulating = false;

	return true;
}

IInput::VRDevices InputModule::getDeviceFromIndex(int index)
{
	if (isVRconnected)
	{
		vr::ETrackedControllerRole role = vr::ETrackedControllerRole::TrackedControllerRole_Invalid;
		if((role = openvr->getSystem()->GetControllerRoleForTrackedDeviceIndex(index)) == vr::ETrackedControllerRole::TrackedControllerRole_LeftHand)
		{
			return IInput::VRDevices::CONTROLLER_LEFT;
		}
		else if (role == vr::ETrackedControllerRole::TrackedControllerRole_RightHand)
		{
			return IInput::VRDevices::CONTROLLER_RIGHT;
		}
	}
	return IInput::VRDevices::NONE;
}

void InputModule::pollData()
{
	//if (window == nullptr)
	//{
	//	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
	//		//std::cout << "Could not initialize SDL." << std::endl;
	//	}

	//	window = SDL_CreateWindow("Demo Window", SDL_WINDOWPOS_CENTERED,
	//		SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);

	//	if (window == NULL) {
	//		//std::cout << "Could not create SDL window." << std::endl;
	//		//printDebug("Could not create SDL window.\n");
	//		//return 1;
	//	}
	//	//add polldata as recurrent
	//}
	auto& extrec = m_info.expoints;
	std::vector<ipengine::any> anyvector;
	std::multimap<time_t, IInput::Input> tempInput;
	SDL_Event event;
	auto lasttiming = inputData.size() > 0 ? (inputData.rbegin())->first:0;
	while (SDL_PollEvent(&event)==1) {
		IInput::Input i;
		switch (event.type) {
		case SDL_KEYUP:
			i.timeStamp = ipengine::Time(clock.now().time_since_epoch().count());
			i.type = IInput::InputType::INPUT_KEY;
			i.data.kd = IInput::keydata{ (IInput::Scancode)((uint16_t)event.key.keysym.scancode), event.key.keysym.mod, IInput::ButtonState::BUTTON_UP };
			i.data.kd.isrepeat = event.key.repeat;
			//event.key.keysym
			//if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) stillRunning = false;
			//if (event.key.keysym.scancode == SDL_SCANCODE_1) OutputDebugString(std::wstring(L"memes").c_str());
			

			//inputData.push_back(std::move(i));
			break;
		case SDL_KEYDOWN:
			i.timeStamp = ipengine::Time(clock.now().time_since_epoch().count());
			i.type = IInput::InputType::INPUT_KEY;
			i.data.kd = IInput::keydata{ (IInput::Scancode)((uint16_t)event.key.keysym.scancode), event.key.keysym.mod, IInput::ButtonState::BUTTON_DOWN };
			i.data.kd.isrepeat = event.key.repeat;

			//tempInput.insert({ i.timeStamp.nano(), i });
			//inputData.push_back(std::move(i));			
			break;
		case SDL_MOUSEBUTTONDOWN:
			i.timeStamp = ipengine::Time(clock.now().time_since_epoch().count());
			i.type = IInput::InputType::INPUT_KEY;
			i.data.kd = IInput::keydata{ (IInput::Scancode)((uint16_t)std::min<Uint8>(6, event.button.button) + (uint16_t)IInput::Scancode::SCANCODE_MOUSEBUTTON1), event.key.keysym.mod, IInput::ButtonState::BUTTON_DOWN };
			//tempInput.insert({ i.timeStamp.nano(), i });
			//inputData.push_back(std::move(i));			
			break;
		case SDL_MOUSEBUTTONUP:
			i.timeStamp = ipengine::Time(clock.now().time_since_epoch().count());
			i.type = IInput::InputType::INPUT_KEY;
			i.data.kd = IInput::keydata{ (IInput::Scancode)((uint16_t)std::min<Uint8>(6, event.button.button) + (uint16_t)IInput::Scancode::SCANCODE_MOUSEBUTTON1), event.key.keysym.mod, IInput::ButtonState::BUTTON_UP };
			//tempInput.insert({ i.timeStamp.nano(), i });
			//inputData.push_back(std::move(i));			
			break;
		case SDL_MOUSEWHEEL:
			i.timeStamp = ipengine::Time(clock.now().time_since_epoch().count());
			i.type = IInput::InputType::INPUT_MOUSESCROLL;
			i.data.md.y = event.wheel.y;
			//tempInput.insert({ i.timeStamp.nano(), i });
			//inputData.push_back(std::move(i));			
			break;
		case SDL_MOUSEMOTION:
			if (event.motion.xrel == 0 && event.motion.yrel == 0)
				break;
			i.timeStamp = ipengine::Time(clock.now().time_since_epoch().count());
			i.type = IInput::InputType::INPUT_MOUSEMOVE;
			i.data.md.y = event.motion.y;
			i.data.md.x = event.motion.x;
			i.data.md.rx = event.motion.xrel;
			i.data.md.ry = event.motion.yrel;
			//tempInput.insert({ i.timeStamp.nano(), i });
			//inputData.push_back(std::move(i));			
			break;
		case SDL_WINDOWEVENT:
			anyvector.push_back(static_cast<IModule_API*>(this));
			anyvector.push_back(event.window);
			extrec.execute("SDL_WindowEvent", { "this", "windowevent" }, anyvector);
			switch (event.window.event) {
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					SDL_SetRelativeMouseMode(SDL_TRUE);
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					SDL_SetRelativeMouseMode(SDL_FALSE);
					break;
			}
			break;
		default:
			// Do nothing.
			break;
		}
		if (i.timeStamp.nano() == lasttiming)
		{
			i.timeStamp = ipengine::Time(lasttiming + 1);
		}
		tempInput.insert({ i.timeStamp.nano(), i });
	}

	//Poll VR positions and add as event
	if (isVRconnected)
	{
		vr::VREvent_t vreve;
		while (vrsys->PollNextEvent(&vreve, sizeof(vr::VREvent_t)))
		{
			IInput::Input i;
			vr::VRControllerState_t state;

			switch (vreve.eventType)
			{
			case vr::VREvent_ButtonPress:
				i.type = IInput::InputType::INPUT_KEY;
				i.timeStamp = ipengine::Time(clock.now().time_since_epoch().count());
				i.data.kd.state = IInput::ButtonState::BUTTON_DOWN;
				i.data.kd.deviceIndex = vreve.trackedDeviceIndex;
				i.data.kd.isrepeat = false;
				vrsys->GetControllerState(vreve.trackedDeviceIndex, &state, sizeof(state));
				switch (vreve.data.controller.button)
				{
				case vr::EVRButtonId::k_EButton_ApplicationMenu:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_MENU;
					break;
				case vr::EVRButtonId::k_EButton_System:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_SYSTEM;
					break;
				case vr::EVRButtonId::k_EButton_Grip:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_GRIP;
					break;
				case vr::EVRButtonId::k_EButton_SteamVR_Touchpad:
				{if (state.rAxis[0].y > 0.7f)
				{
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_UP;
				}
				else if (state.rAxis[0].y < -0.7f)
				{
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_DOWN;
				}
				IInput::Scancode checksecondinput = IInput::Scancode::NUM_SCANCODES;
				if (state.rAxis[0].x > 0.7f)
				{
					checksecondinput = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_RIGHT;
				}
				else if (state.rAxis[0].x < -0.7f)
				{
					checksecondinput = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_LEFT;
				}
				if (checksecondinput != IInput::Scancode::NUM_SCANCODES)
				{
					IInput::Input second(i);
					second.data.kd.keycode = checksecondinput;
					if (second.timeStamp.nano() == lasttiming)
					{
						second.timeStamp = ipengine::Time(lasttiming + 1);
					}
					tempInput.insert({ second.timeStamp.nano(), second });
				}}
					break;
				case vr::EVRButtonId::k_EButton_SteamVR_Trigger:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_TRIGGER;
					break;
				case vr::EVRButtonId::k_EButton_Axis2:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_AXIS2;
					break;
				case vr::EVRButtonId::k_EButton_Axis3:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_AXIS3;
					break;
				case vr::EVRButtonId::k_EButton_Axis4:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_AXIS4;
					break;
				case vr::EVRButtonId::k_EButton_DPad_Left:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_LEFT;
					break;
				case vr::EVRButtonId::k_EButton_DPad_Up:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_UP;
					break;
				case vr::EVRButtonId::k_EButton_DPad_Right:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_RIGHT;
					break;
				case vr::EVRButtonId::k_EButton_DPad_Down:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_DOWN;
					break;
				}
				break;
			case vr::VREvent_ButtonUnpress:
				i.type = IInput::InputType::INPUT_KEY;
				i.timeStamp = ipengine::Time(clock.now().time_since_epoch().count()+10);
				i.data.kd.state = IInput::ButtonState::BUTTON_UP;
				i.data.kd.deviceIndex = vreve.trackedDeviceIndex;
				vrsys->GetControllerState(vreve.trackedDeviceIndex, &state, sizeof(state));
				switch (vreve.data.controller.button)
				{
				case vr::EVRButtonId::k_EButton_ApplicationMenu:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_MENU;
					break;
				case vr::EVRButtonId::k_EButton_System:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_SYSTEM;
					break;
				case vr::EVRButtonId::k_EButton_Grip:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_GRIP;
					break;
				case vr::EVRButtonId::k_EButton_SteamVR_Touchpad:
				{if (state.rAxis[0].y > 0.7f)
				{
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_UP;
				}
				else if (state.rAxis[0].y < -0.7f)
				{
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_DOWN;
				}
				IInput::Scancode checksecondinput = IInput::Scancode::NUM_SCANCODES;
				if (state.rAxis[0].x > 0.7f)
				{
					checksecondinput = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_RIGHT;
				}
				else if (state.rAxis[0].x < -0.7f)
				{
					checksecondinput = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_LEFT;
				}
				if (checksecondinput != IInput::Scancode::NUM_SCANCODES)
				{
					IInput::Input second(i);
					second.data.kd.keycode = checksecondinput;
					if (second.timeStamp.nano() == lasttiming)
					{
						second.timeStamp = ipengine::Time(lasttiming + 10);
					}
					tempInput.insert({ second.timeStamp.nano(), second });
				}}
					break;
				case vr::EVRButtonId::k_EButton_SteamVR_Trigger:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_TRIGGER;
					break;
				case vr::EVRButtonId::k_EButton_Axis2:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_AXIS2;
					break;
				case vr::EVRButtonId::k_EButton_Axis3:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_AXIS3;
					break;
				case vr::EVRButtonId::k_EButton_Axis4:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_AXIS4;
					break;
				case vr::EVRButtonId::k_EButton_DPad_Left:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_LEFT;
					break;
				case vr::EVRButtonId::k_EButton_DPad_Up:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_UP;
					break;
				case vr::EVRButtonId::k_EButton_DPad_Right:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_RIGHT;
					break;
				case vr::EVRButtonId::k_EButton_DPad_Down:
					i.data.kd.keycode = IInput::Scancode::SCANCODE_VRBUTTON_DPAD_DOWN;
					break;
				}
				break;
			case vr::VREvent_ButtonTouch:
				break;
			case vr::VREvent_ButtonUntouch:
				break;
			}
			if (i.timeStamp.nano() == lasttiming)
			{
				i.timeStamp = ipengine::Time(lasttiming + 1);
			}
			tempInput.insert({ i.timeStamp.nano(), i });
		}
	}



	auto timestamp = ipengine::Time((clock.now() - timeToRetainInput).time_since_epoch().count());
	auto start = inputData.begin();
	auto end = std::find_if(start, inputData.end(), [timestamp](std::pair<time_t, IInput::Input> i)->bool{if (i.first < timestamp.nano()) return false; return true; });
	try{
		/*while (isManipulating)
		{
			if (!isManipulating)
			{
				isManipulating = true;
				break;
			}
		}*/
		mymutex.lock();
		inputData.insert(tempInput.begin(), tempInput.end());
		
		
		inputData.erase(start, end);
		mymutex.unlock();
		//std::cout << tempInput.size(); std::cout << "      ";
		/*while (start != inputData.end() && start++->timeStamp < timestamp)
		{
			inputData.pop_front();
			if (inputData.empty())
				break;

		}*/
	}
	catch (std::exception e)
	{
		isManipulating = false;
	}
	isManipulating = false;
}

const std::vector<IInput::Input> InputModule::getInputBuffered(ipengine::Time timestamp, bool vrpositions)
{
	//auto timestamp = clock.now() - std::chrono::milliseconds(millisecondsIntoThePast);
	//create copy
	/*while (isManipulating)
	{
		if (!isManipulating)
		{
			isManipulating = true;
			break;
		}
	}*/
	mymutex.lock();
	auto in = std::multimap<time_t, IInput::Input>(inputData);
	mymutex.unlock();
	isManipulating = false;

	auto end= in.end();
	auto start = end;
	//auto inputd = std::deque<IInput::Input>(inputData.begin(), inputData.end());
	if (!inputData.empty())
		start = std::find_if(in.begin(), in.end(), [timestamp](std::pair<time_t, IInput::Input> i)->bool{if (i.second.timeStamp.nano() >= timestamp.nano()) return true; return false; });
	//auto start = inputData.begin();

	//for ()

	
	//int d = end - startIt;
	std::vector<IInput::Input> n;// (d);
	for (start; start != in.end();++start)
	{
		n.push_back(start->second);
	}

	//add VR position data to the end
	if (isVRconnected && vrpositions)
	{
		IInput::Input inputVR;
		vr::TrackedDevicePose_t poseArray[vr::k_unMaxTrackedDeviceCount];
		uint32_t count = vr::k_unMaxTrackedDeviceCount;
		if (!vrsys->IsTrackedDeviceConnected(vr::k_unTrackedDeviceIndex_Hmd))
		{
			IInput::Input inputDC;
			inputVR.timeStamp = ipengine::Time(clock.now().time_since_epoch().count());
			inputDC.type = IInput::InputType::INPUT_DEVICE_DISCONNECTED;
			inputDC.data.i3dmd.deviceIndex = vr::k_unTrackedDeviceIndex_Hmd;
			inputDC.data.i3dmd.deviceType = IInput::VRDevices::HMD;
			n.push_back(inputDC);
		}
		vrsys->GetDeviceToAbsoluteTrackingPose(vr::TrackingUniverseOrigin::TrackingUniverseStanding, 0, poseArray, count);
		for (int index = 0; index < vr::k_unMaxTrackedDeviceCount; ++index)
		{
			if (vrsys->IsTrackedDeviceConnected(index))
			{
				IInput::Input inputVR;
				float temp[3];
				//float tempM[3][3];
				switch (vrsys->GetTrackedDeviceClass(index))
				{
				case vr::ETrackedDeviceClass::TrackedDeviceClass_HMD:
					inputVR.timeStamp = ipengine::Time(clock.now().time_since_epoch().count());
					inputVR.type = IInput::InputType::INPUT_3DMOVE;
					inputVR.data.i3dmd.deviceIndex = index;
					inputVR.data.i3dmd.deviceType = IInput::VRDevices::HMD;
					inputVR.data.i3dmd.velocity[0] = poseArray[index].vVelocity.v[0];
					inputVR.data.i3dmd.velocity[1] = poseArray[index].vVelocity.v[1];
					inputVR.data.i3dmd.velocity[2] = poseArray[index].vVelocity.v[2];
					inputVR.data.i3dmd.angularVelocity[0] = poseArray[index].vAngularVelocity.v[0];
					inputVR.data.i3dmd.angularVelocity[1] = poseArray[index].vAngularVelocity.v[1];
					inputVR.data.i3dmd.angularVelocity[2] = poseArray[index].vAngularVelocity.v[2];
					hmd34ToPosition(poseArray[index].mDeviceToAbsoluteTracking, temp);
					inputVR.data.i3dmd.x = temp[0];
					inputVR.data.i3dmd.y = temp[1];
					inputVR.data.i3dmd.z = temp[2];
					hmd34ToRotation(poseArray[index].mDeviceToAbsoluteTracking, temp);
					inputVR.data.i3dmd.yaw = temp[0];
					inputVR.data.i3dmd.pitch = temp[1];
					inputVR.data.i3dmd.roll = temp[2];
					hmd34ToRotationm(poseArray[index].mDeviceToAbsoluteTracking, inputVR.data.i3dmd.rotation);
					//inputVR.data.i3dmd.rotation = std::copy(tempM);
					n.push_back(std::move(inputVR));
					break;
				case vr::ETrackedDeviceClass::TrackedDeviceClass_Controller:
					inputVR.timeStamp = ipengine::Time(clock.now().time_since_epoch().count());
					inputVR.type = IInput::InputType::INPUT_3DMOVE;
					inputVR.data.i3dmd.deviceIndex = index;
					inputVR.data.i3dmd.deviceType = vrsys->GetControllerRoleForTrackedDeviceIndex(index) == vr::ETrackedControllerRole::TrackedControllerRole_LeftHand ? IInput::VRDevices::CONTROLLER_LEFT : IInput::VRDevices::CONTROLLER_RIGHT;
					inputVR.data.i3dmd.velocity[0] = poseArray[index].vVelocity.v[0];
					inputVR.data.i3dmd.velocity[1] = poseArray[index].vVelocity.v[1];
					inputVR.data.i3dmd.velocity[2] = poseArray[index].vVelocity.v[2];
					inputVR.data.i3dmd.angularVelocity[0] = poseArray[index].vAngularVelocity.v[0];
					inputVR.data.i3dmd.angularVelocity[1] = poseArray[index].vAngularVelocity.v[1];
					inputVR.data.i3dmd.angularVelocity[2] = poseArray[index].vAngularVelocity.v[2];
					hmd34ToPosition(poseArray[index].mDeviceToAbsoluteTracking, temp);
					inputVR.data.i3dmd.x = temp[0];
					inputVR.data.i3dmd.y = temp[1];
					inputVR.data.i3dmd.z = temp[2];
					hmd34ToRotation(poseArray[index].mDeviceToAbsoluteTracking, temp);
					inputVR.data.i3dmd.yaw = temp[0];
					inputVR.data.i3dmd.pitch = temp[1];
					inputVR.data.i3dmd.roll = temp[2];
					hmd34ToRotationm(poseArray[index].mDeviceToAbsoluteTracking, inputVR.data.i3dmd.rotation);

					n.push_back(std::move(inputVR));

					break;
				case vr::ETrackedDeviceClass::TrackedDeviceClass_GenericTracker:
					inputVR.timeStamp = ipengine::Time(clock.now().time_since_epoch().count());
					inputVR.type = IInput::InputType::INPUT_3DMOVE;
					inputVR.data.i3dmd.deviceIndex = index;
					inputVR.data.i3dmd.deviceType = IInput::VRDevices::HMD;
					inputVR.data.i3dmd.velocity[0] = poseArray[index].vVelocity.v[0];
					inputVR.data.i3dmd.velocity[1] = poseArray[index].vVelocity.v[1];
					inputVR.data.i3dmd.velocity[2] = poseArray[index].vVelocity.v[2];
					inputVR.data.i3dmd.angularVelocity[0] = poseArray[index].vAngularVelocity.v[0];
					inputVR.data.i3dmd.angularVelocity[1] = poseArray[index].vAngularVelocity.v[1];
					inputVR.data.i3dmd.angularVelocity[2] = poseArray[index].vAngularVelocity.v[2];
					hmd34ToPosition(poseArray[index].mDeviceToAbsoluteTracking, temp);
					inputVR.data.i3dmd.x = temp[0];
					inputVR.data.i3dmd.y = temp[1];
					inputVR.data.i3dmd.z = temp[2];
					hmd34ToRotation(poseArray[index].mDeviceToAbsoluteTracking, temp);
					inputVR.data.i3dmd.yaw = temp[1];
					inputVR.data.i3dmd.pitch = temp[0];
					inputVR.data.i3dmd.roll = temp[2];
					hmd34ToRotationm(poseArray[index].mDeviceToAbsoluteTracking, inputVR.data.i3dmd.rotation);

					n.push_back(std::move(inputVR));

					break;
				}
				
			}
			
		}
	}
	return n;//std::vector<IInput::Input>(startIt, inputd.end());
}
