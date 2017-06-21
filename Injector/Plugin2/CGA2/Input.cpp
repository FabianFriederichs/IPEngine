#include "Input.h"
Input* Input::handlerInstance;

Input::Input(GLFWwindow* window) : glfwWindow(window)
{
	

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

	glfwSetKeyCallback(window, Input::key_dispatch);
	glfwSetCursorPosCallback(window, Input::mm_dispatch);
	glfwSetMouseButtonCallback(window, Input::mb_dispatch);
	glfwSetScrollCallback(window, Input::mscr_dispatch);
}

Input::~Input()
{
	handlerInstance = nullptr;
}

void Input::key_dispatch(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (Input::handlerInstance == nullptr)
		return;
	for (auto it : Input::handlerInstance->inputhandlers)
	{
		it->onKey(Key(key), Action(action), Modifier(mods));
	}
}


void Input::mm_dispatch(GLFWwindow* window, double xpos, double ypos)
{
	if (Input::handlerInstance == nullptr)
		return;
	Input::handlerInstance->mousepos.X = xpos;
	Input::handlerInstance->mousepos.Y = ypos;
	for (auto it : Input::handlerInstance->inputhandlers)
	{
		it->onMouseMove(Input::handlerInstance->mousepos);
	}
	Input::handlerInstance->mousepos.oldX = xpos;
	Input::handlerInstance->mousepos.oldY = ypos;
}


void Input::mb_dispatch(GLFWwindow* window, int button, int action, int mods)
{
	if (Input::handlerInstance == nullptr)
		return;
	for (auto it : Input::handlerInstance->inputhandlers)
	{
		it->onMouseButton(MouseButton(button), Action(action), Modifier(mods));
	}
}


void Input::mscr_dispatch(GLFWwindow* window, double xoffset, double yoffset)
{
	if (Input::handlerInstance == nullptr)
		return;
	for (auto it : Input::handlerInstance->inputhandlers)
	{
		it->onMouseScroll(xoffset, yoffset);
	}
}

void Input::addInputHandler(InputHandler* handler)
{
	inputhandlers.push_back(handler);
}

void Input::removeInputHandler(InputHandler* handler)
{
	inputhandlers.erase(std::remove(inputhandlers.begin(), inputhandlers.end(), handler), inputhandlers.end());
}

KeyState Input::getKeyState(Key key)
{
	return static_cast<KeyState>(glfwGetKey(glfwWindow, static_cast<int>(key)));
}

MouseButtonState Input::getMouseButtonState(MouseButton mousebutton)
{
	return static_cast<MouseButtonState>(glfwGetMouseButton(glfwWindow, static_cast<int>(mousebutton)));
}

MousePosition Input::getMousePosition()
{
	return mousepos;
}

void Input::setCursorVisible(bool visible)
{
	glfwSetInputMode(glfwWindow, GLFW_CURSOR, (visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED));
}