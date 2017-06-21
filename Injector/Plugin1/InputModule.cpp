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
	m_info.identifier = "Plugin1";
	m_info.version = "1.0";
	m_info.iam = "IInput_API";
	return;
}

bool InputModule::startUp()
{
	
	memes = std::thread([this]()->void{while (1){ pollData(); }});
	return true;
}

void InputModule::pollData()
{
	if (window == nullptr)
	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
			//std::cout << "Could not initialize SDL." << std::endl;
		}

		window = SDL_CreateWindow("Demo Window", SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);

		if (window == NULL) {
			//std::cout << "Could not create SDL window." << std::endl;
			//printDebug("Could not create SDL window.\n");
			//return 1;
		}
		//add polldata as recurrent
	}
	std::multimap<time_t, IInput::Input> tempInput;
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		IInput::Input i;
		switch (event.type) {
		case SDL_KEYUP:
			i.timeStamp = clock.now();
			i.type = IInput::InputType::INPUT_KEY;
			i.data.kd = IInput::keydata{ (IInput::Scancode)((uint16_t)event.key.keysym.scancode), event.key.keysym.mod, IInput::ButtonState::BUTTON_UP };
			//event.key.keysym
			//if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) stillRunning = false;
			//if (event.key.keysym.scancode == SDL_SCANCODE_1) OutputDebugString(std::wstring(L"memes").c_str());
			tempInput.insert({ std::chrono::high_resolution_clock::to_time_t(i.timeStamp), i });
			//inputData.push_back(std::move(i));
			break;
		case SDL_KEYDOWN:
			i.timeStamp = clock.now();
			i.type = IInput::InputType::INPUT_KEY;
			i.data.kd = IInput::keydata{ (IInput::Scancode)((uint16_t)event.key.keysym.scancode), event.key.keysym.mod, IInput::ButtonState::BUTTON_DOWN };
			tempInput.insert({ std::chrono::high_resolution_clock::to_time_t(i.timeStamp), i });
			//inputData.push_back(std::move(i));			
			break;
		case SDL_MOUSEBUTTONDOWN:
			i.timeStamp = clock.now();
			i.type = IInput::InputType::INPUT_KEY;
			i.data.kd = IInput::keydata{ (IInput::Scancode)((uint16_t)std::min<Uint8>(6, event.button.button) + (uint16_t)IInput::Scancode::SCANCODE_MOUSEBUTTON1), event.key.keysym.mod, IInput::ButtonState::BUTTON_DOWN };
			tempInput.insert({ std::chrono::high_resolution_clock::to_time_t(i.timeStamp), i });
			//inputData.push_back(std::move(i));			
			break;
		case SDL_MOUSEBUTTONUP:
			i.timeStamp = clock.now();
			i.type = IInput::InputType::INPUT_KEY;
			i.data.kd = IInput::keydata{ (IInput::Scancode)((uint16_t)std::min<Uint8>(6, event.button.button) + (uint16_t)IInput::Scancode::SCANCODE_MOUSEBUTTON1), event.key.keysym.mod, IInput::ButtonState::BUTTON_UP };
			tempInput.insert({ std::chrono::high_resolution_clock::to_time_t(i.timeStamp), i });
			//inputData.push_back(std::move(i));			
			break;
		case SDL_MOUSEWHEEL:
			i.timeStamp = clock.now();
			i.type = IInput::InputType::INPUT_MOUSESCROLL;
			i.data.md.y = event.wheel.y;
			tempInput.insert({ std::chrono::high_resolution_clock::to_time_t(i.timeStamp), i });
			//inputData.push_back(std::move(i));			
			break;
		case SDL_MOUSEMOTION:
			if (event.motion.xrel == 0 && event.motion.yrel == 0)
				break;
			i.timeStamp = clock.now();
			i.type = IInput::InputType::INPUT_MOUSEMOVE;
			i.data.md.y = event.motion.y;
			i.data.md.x = event.motion.x;
			tempInput.insert({ std::chrono::high_resolution_clock::to_time_t(i.timeStamp), i });
			//inputData.push_back(std::move(i));			
			break;
		default:
			// Do nothing.
			break;
		}
		
	}

	//Poll VR positions and add as event
	auto timestamp = clock.now() - timeToRetainInput;
	auto start = inputData.begin();
	auto end = std::find_if(start, inputData.end(), [timestamp](std::pair<time_t, IInput::Input> i)->bool{if (i.second.timeStamp < timestamp) return true; return false; });
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

const std::vector<IInput::Input> InputModule::getInputBuffered(int millisecondsIntoThePast)
{
	auto timestamp = clock.now() - std::chrono::milliseconds(millisecondsIntoThePast);
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
		start = std::find_if(in.begin(), in.end(), [timestamp](std::pair<time_t, IInput::Input> i)->bool{if (i.second.timeStamp >= timestamp) return true; return false; });
	//auto start = inputData.begin();

	//for ()

	
	//int d = end - startIt;
	std::vector<IInput::Input> n;// (d);
	for (start; start != in.end();++start)
	{
		n.push_back(start->second);
	}
	return n;//std::vector<IInput::Input>(startIt, inputd.end());
}
