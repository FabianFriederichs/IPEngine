#ifndef _GAME_WINDOW_H_
#define _GAME_WINDOW_H_
#include <libheaders.h>
#include <glerror.h>
#include <Input.h>
#include <memory>



class GameWindow : public InputHandler
{
public:
	//ctors/dtor
	//Don't copy!
	GameWindow(const GameWindow&) = delete;
	//Don't move!
	GameWindow(GameWindow&&) = delete;
	GameWindow(const GLint width, const GLint height, bool fullscreen, bool vsync, const GLint cvmaj, const GLint cvmin, const std::string& title, const GLint msaasamples = 0, const GLboolean uselatestglver = false, GLdouble updatefrequency = 120.0);
	virtual ~GameWindow();

	//public interface
	GLvoid run();	//starts game loop
	GLvoid quit();	//stops the game loop and closes the application

	//override these in your application
	virtual GLvoid update(GLdouble dtime) {};	//physics and logic updates here
	virtual GLvoid render(GLdouble dtime) {};	//rendering a frame goes here
	virtual GLvoid init() {};					//initialization tasks
	virtual GLvoid shutdown() {};				//clean everything up

	//override these to i.e. call glViewport on a window resize
	virtual void onWindowResize(int width, int height) {};
	virtual void onFrameBufferResize(int width, int height) {};

	//empty input event handlers, override if needed
	virtual void onKey(Key key, Action action, Modifier modifier) override {};
	virtual void onMouseMove(MousePosition mouseposition) override {
		int um = 0;
	};
	virtual void onMouseButton(MouseButton button, Action action, Modifier modifier) override {};
	virtual void onMouseScroll(double xscroll, double yscroll) override {};


protected:
	//window width/height
	GLint windowWidth;
	GLint windowHeight;
	//window title
	std::string title;
	bool fullscreen;
	bool vsync;
	//access this to query input states
	std::unique_ptr<Input> input;
private:
	GLboolean initialize();
	GLboolean m_uselatestglcontext;
	GLint m_cvmaj;
	GLint m_cvmin;
	GLint m_samples;
	GLdouble m_updatefrequency;
	GLFWwindow* m_window;
	static GameWindow* windowHandlerInstance;

	//glfw callback wrappers
	static void wrz_callback(GLFWwindow* window, int width, int height)
	{
		windowHandlerInstance->windowWidth = width;
		windowHandlerInstance->windowHeight = height;
		windowHandlerInstance->onWindowResize(width, height);
	}
	static void frz_callback(GLFWwindow* window, int width, int height)
	{
		windowHandlerInstance->windowWidth = width;
		windowHandlerInstance->windowHeight = height;
		windowHandlerInstance->onFrameBufferResize(width, height);
	}
};

#endif
