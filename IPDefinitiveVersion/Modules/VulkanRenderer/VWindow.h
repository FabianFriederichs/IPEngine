#ifndef VWINDOW_H
#define VWINDOW_H
#ifndef VK_USE_PLATFORM_WIN32_KHR
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <vector>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>
#include "VDeleter.h"


namespace rj
{
	class VWindow
	{
	public:
		/*VWindow(
			const VDeleter<VkInstance> &inst,
			uint32_t width, uint32_t height,
			const std::string &title = "",
			void *app = nullptr,
			GLFWkeyfun onKeyPressed = nullptr,
			GLFWmousebuttonfun onMouseClicked = nullptr,
			GLFWcursorposfun onCursorMoved = nullptr,
			GLFWscrollfun onScroll = nullptr,
			GLFWwindowsizefun onWindowResized = nullptr)
			:
			m_instance(inst),
			m_width(width), m_height(height),
			m_windowTitle(title),
			m_app(app),
			m_onWindowResized(onWindowResized),
			m_onMouseClicked(onMouseClicked),
			m_onCursorMoved(onCursorMoved),
			m_onKeyPressed(onKeyPressed),
			m_onScroll(onScroll),
			m_surface{ m_instance, vkDestroySurfaceKHR }
		{
			initWindow();
			createSurface();
		}
*/
		VWindow(const VDeleter<VkInstance> &inst,
			uint32_t width, uint32_t height,
			const std::string &title = "",
			void *app = nullptr)
			:
			m_instance(inst),
			m_width(width), m_height(height),
			m_windowTitle(title),
			m_app(app),
			m_surface{ m_instance, vkDestroySurfaceKHR }
		{
			initWindow();
			createSurface();
		}

		operator const VDeleter<VkSurfaceKHR> &() const
		{
			return m_surface;
		}

		operator VkSurfaceKHR() const
		{
			return static_cast<VkSurfaceKHR>(m_surface);
		}

		SDL_Window *getWindow() const
		{
			assert(m_window);
			return m_window;
		}

		void getExtent(uint32_t *pWidth, uint32_t *pHeight) const
		{
			*pWidth = m_width;
			*pHeight = m_height;
		}

		void setWindowTitle(const std::string &title)
		{
			m_windowTitle = title;
			SDL_SetWindowTitle(m_window, m_windowTitle.c_str());
		}

		static std::vector<const char *> getRequiredExtensions()
		{
			//std::vector<const char *> extensions;

			//unsigned int glfwExtensionCount = 0;
			////glfwInit();
			////glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			//if (!SDL_Vulkan_GetInstanceExtensions(m_window, &glfwExtensionCount, NULL))
			//{
			//	throw std::runtime_error("failed to retrieve instance extensions");

			//}
			//
			//const char** glfwExtensions = (const char**)malloc(sizeof(const char *) * glfwExtensionCount);
			//if (!glfwExtensions)
			//	throw std::runtime_error("failed to retrieve instance extensions");

			//// get names of required extensions
			//if (!SDL_Vulkan_GetInstanceExtensions(m_window, &glfwExtensionCount, glfwExtensions))
			//	throw std::runtime_error("failed to retrieve instance extensions");

			//for (unsigned int i = 0; i < glfwExtensionCount; i++)
			//{
			//	extensions.push_back(glfwExtensions[i]);
			//}
			std::vector<const char*> extensions;
			extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
			extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
			return extensions;
		}

		void createSurface()
		{
			/*if (glfwCreateWindowSurface(m_instance, m_window, nullptr, m_surface.replace()) != VK_SUCCESS)
			{
			throw std::runtime_error("failed to create window surface!");
			}*/
			//m_surface = { m_instance, vkDestroySurfaceKHR };

			SDL_SysWMinfo windowInfo;
			SDL_VERSION(&windowInfo.version);
			if (!SDL_GetWindowWMInfo(m_window, &windowInfo)) {
				throw std::system_error(std::error_code(), "SDK window manager info is not available.");
			}
			VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
			surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			surfaceInfo.hinstance = GetModuleHandle(NULL);
			surfaceInfo.hwnd = windowInfo.info.win.window;

			VkResult result = vkCreateWin32SurfaceKHR(m_instance, &surfaceInfo, NULL, m_surface.replace());
			if (result != VK_SUCCESS) {
				throw std::runtime_error("failed to create window surface!");
			}
			//if (SDL_Vulkan_CreateSurface(m_window, inst, m_surface.replace()) == SDL_FALSE)
			//{
			//	throw std::runtime_error("failed to create window surface!");

			//}
		}

	protected:
		void initWindow()
		{
			auto res = SDL_Init(SDL_INIT_VIDEO);
			if (res != 0)
			{
				//std::cout << "Could not initialize SDL." << std::endl;
				throw std::runtime_error("failed to create window surface!");
			}
			//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // not to create OpenGL context
			m_window = SDL_CreateWindow(m_windowTitle.c_str(), SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED, static_cast<int>(m_width), static_cast<int>(m_height), SDL_WINDOW_OPENGL);
			auto err = SDL_GetError();
			std::string error(err);
			if (m_window == NULL)
			{
				throw std::runtime_error("failed to create window surface!");
			}
			SDL_SysWMinfo info;
			SDL_VERSION(&info.version);
			res = SDL_GetWindowWMInfo(m_window, &info);
			err = SDL_GetError();

			if (res != SDL_TRUE)
			{
				//std::cout << "Could not initialize SDL." << std::endl;
				throw std::runtime_error("failed to create window surface!");
			}
			//m_window = glfwCreateWindow(m_width, m_height, m_windowTitle.c_str(), nullptr, nullptr);
			/*if (m_app) glfwSetWindowUserPointer(m_window, m_app);
			if (m_onWindowResized) glfwSetWindowSizeCallback(m_window, m_onWindowResized);
			if (m_onMouseClicked) glfwSetMouseButtonCallback(m_window, m_onMouseClicked);
			if (m_onCursorMoved) glfwSetCursorPosCallback(m_window, m_onCursorMoved);
			if (m_onKeyPressed) glfwSetKeyCallback(m_window, m_onKeyPressed);
			if (m_onScroll) glfwSetScrollCallback(m_window, m_onScroll);*/
		}

		

		const VDeleter<VkInstance> &m_instance;

		uint32_t m_width, m_height;
		std::string m_windowTitle;
		SDL_Window *m_window = nullptr;

		// Application provide implementations for these
		void *m_app;
		/*sdl m_onWindowResized;
		GLFWmousebuttonfun m_onMouseClicked;
		GLFWcursorposfun m_onCursorMoved;
		GLFWkeyfun m_onKeyPressed;
		GLFWscrollfun m_onScroll;*/

		VDeleter<VkSurfaceKHR> m_surface;
	};
}

#endif