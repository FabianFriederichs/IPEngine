#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#ifdef _MSC_VER
#ifdef EXPORT_APPLICATION
#define APP_API __declspec(dllexport)
#else //
#define APP_API __declspec(dllimport)
#endif
#else
#define APP_API
#endif

#include <IPCore/RuntimeControl/Time.h>
#include <Injector/Injector.h>
#include <IPCore/Core/ICore.h>


namespace ipengine
{

	class APP_API Application
	{
	public:
		Application();
		virtual ~Application();

		Application(const Application&) = delete;
		Application(Application&&);
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&);

		void init(const iprstr configPath);
		void run();
		Injector& getInjector();
		Core& getCore();

		virtual void initialize() = 0;
		virtual void preTick() {};
		virtual void postTick(const ipengine::Time& t) {};
		virtual void onShutdown() = 0;
		virtual void onConsole() {};
	private:
		class ApplicationImpl; ApplicationImpl* m_pimpl; 
	};

}
#endif