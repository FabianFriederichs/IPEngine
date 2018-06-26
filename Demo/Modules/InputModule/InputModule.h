// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN1_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include <IInput_API.h>
#include <boost/config.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <deque>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include <IBasicOpenVRModule_API.h>
#include <math.h>
#include <IPCore/Core/ICore.h>
// This class is exported from the Plugin1.dll
class InputModule : public IInput_API {
public:
	InputModule(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo(){ return &m_info; }
	const std::vector<IInput::Input> getInputBuffered(ipengine::Time, bool vrpositions = false);

	//void printStuffToSomething(std::string text) { std::cout << text << std::endl; }
private:
	std::vector<ipengine::Scheduler::SubHandle> handles;
	ModuleInformation m_info;
	SDL_Window* window;
	std::thread memes;
	std::chrono::system_clock::duration timeToRetainInput = std::chrono::milliseconds(2000);
	std::chrono::system_clock clock;
	std::multimap<time_t, IInput::Input> inputData;
	bool isManipulating = false;
	std::mutex mymutex;
	void pollData();
	void pollDataC(ipengine::TaskContext &c);
	std::shared_ptr<vr::IVRSystem> vrsys;
	bool isVRconnected = false;
	std::shared_ptr<IBasicOpenVRModule_API> openvr;
	vr::TrackedDevicePose_t lastposes[vr::k_unMaxTrackedDeviceCount];
	void hmd34ToPosition(const vr::HmdMatrix34_t& matrix, float position[3])
	{
		//float memes[3];
		position[0] = matrix.m[0][3]; //x
		position[1] = matrix.m[1][3]; //y
		position[2] = matrix.m[2][3]; //z
		//return memes;
	}

	//NOT WORKING CORRECTLY
	void hmd34ToRotation(const vr::HmdMatrix34_t& matrix, float rotation[3])
	{
		//float memes[3];
		rotation[0] = std::atan2(matrix.m[2][3], matrix.m[3][3]); //pitch = x
		rotation[1] = std::atan2(-matrix.m[1][3], std::sqrt( std::powf(matrix.m[2][3],2) + std::powf(matrix.m[3][3],2))); //yaw = y
		rotation[2] = std::atan2(matrix.m[1][2], matrix.m[1][1]); //roll = z
		//return memes;
	}
	void hmd34ToRotationm(const vr::HmdMatrix34_t& matrix, float out[3][3])
	{
		//float memes[3][3];
		out[0][0] = matrix.m[0][0];
		out[0][1] = matrix.m[1][0];
		out[0][2] = matrix.m[2][0];
		out[1][0] = matrix.m[0][1];
		out[1][1] = matrix.m[1][1];
		out[1][2] = matrix.m[2][1];
		out[2][0] = matrix.m[0][2];
		out[2][1] = matrix.m[1][2];
		out[2][2] = matrix.m[2][2];
		//return memes;
	}

	// Inherited via IInput_API
	virtual bool _startup() override;

	// Inherited via IInput_API
	virtual bool _shutdown() override;

	// Inherited via IInput_API
	virtual IInput::VRDevices getDeviceFromIndex(int index) override;
};
extern "C" BOOST_SYMBOL_EXPORT InputModule module;
InputModule module;