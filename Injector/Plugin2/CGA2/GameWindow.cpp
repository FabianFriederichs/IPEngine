#include "GameWindow.h"

GameWindow* GameWindow::windowHandlerInstance;

GameWindow::GameWindow(const GLint sizex, const GLint sizey, bool fullscreen, bool vsync, const GLint cvmaj, const GLint cvmin, const std::string& title, const GLint msaasamples, const GLboolean uselatestglver, const GLdouble updatefrequency)
{
	this->windowWidth = sizex;
	this->windowHeight = sizey;
	this->m_uselatestglcontext = uselatestglver;
	this->m_cvmaj = cvmaj;
	this->m_cvmin = cvmin;
	this->title = title;
	this->m_samples = msaasamples;
	this->fullscreen = fullscreen;
	this->vsync = vsync;
	this->m_updatefrequency = updatefrequency;
	if (!this->initialize())
		throw std::invalid_argument("Error: Window initialization failed.");
}


GameWindow::~GameWindow()
{
	if(m_window != nullptr)
	{
		glfwDestroyWindow(m_window);
		m_window = nullptr;
	}
	glfwTerminate();
}

GLboolean GameWindow::initialize()
{
	//glfwSetErrorCallback({})

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW\n";
		//getchar();
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, m_samples);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->m_cvmaj);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->m_cvmin);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	this->m_window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), (fullscreen ? glfwGetPrimaryMonitor() : NULL), NULL);
	if (this->m_window == NULL){
		std::cerr << "Failed to open GLFW window. OpenGL Version " << m_cvmaj << "." << m_cvmin << " is not supported on your system.\n";
		//getchar();
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(this->m_window);

	glewExperimental = GL_TRUE;
	// Initialize GLEW
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW\n";
		std::cerr << "Error: " << glewGetErrorString(err) << "\n";
		//getchar();
		glfwTerminate();
		return false;
	}

	err = glGetError(); //dummy readout
	std::cerr << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << "\nOpenGL Version " << m_cvmaj << "." << m_cvmin << " context successfully created.\nExtensions successfully loaded.\n";

	//Setup input instance
	input.reset(new Input(m_window));
	input->setHandlerInstance(input.get());
	input->addInputHandler(this);

	//Setup window/framebuffer resize callback
	GameWindow::windowHandlerInstance = this;
	glfwSetFramebufferSizeCallback(m_window, &GameWindow::frz_callback);
	glfwSetWindowSizeCallback(m_window, &GameWindow::wrz_callback);

	glfwSwapInterval((vsync ? 1 : 0));
	if (m_samples > 0)
	{
		glEnable(GL_MULTISAMPLE);
		if (checkglerror())
		{
			return false;
		}
	}

	return true;
}

GLvoid GameWindow::run()
{
	if(m_window == nullptr)
		throw std::logic_error("Run failed. GameWindow is not initialized.\n");
	init();
	GLdouble timeDelta = 1.0f / m_updatefrequency;
	GLdouble timeAccumulator = 0;
	GLdouble timeAccumulator2 = 0;
	GLdouble startTime;
	GLdouble frametime;
	while (!glfwWindowShouldClose(this->m_window))
	{
		startTime = glfwGetTime();
		glfwPollEvents();
		while (timeAccumulator >= timeDelta)
		{
			update(timeDelta);
			
			timeAccumulator -= timeDelta;
		}
		auto ta = glfwGetTime();
		render(glfwGetTime() - startTime);
		if (m_window != nullptr)
		{
			glfwSwapBuffers(m_window);
		}
		if (timeAccumulator2 >= 1.f / 3.f)
		{
			frametime = (glfwGetTime() - ta);
			//std::cout << "Frame time: " << frametime<< "s FPS: "<<1.f/frametime<<"FPS\n";
			timeAccumulator2 = 0;
		}
		
		timeAccumulator += glfwGetTime() - startTime;
		timeAccumulator2 += glfwGetTime() - startTime;
	}
	shutdown();
}

GLvoid GameWindow::quit()
{
	if(m_window != nullptr)
		glfwSetWindowShouldClose(m_window, GL_TRUE);
}
