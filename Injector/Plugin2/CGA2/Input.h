#ifndef _INPUT_H_
#define _INPUT_H_
#include <libheaders.h>
#include <vector>
#include <algorithm>


//enums for events
enum class Key : int
{
	Unknown = GLFW_KEY_UNKNOWN,
	Space = GLFW_KEY_SPACE,
	Apostrophe = GLFW_KEY_APOSTROPHE,
	Comma = GLFW_KEY_COMMA,
	Minus = GLFW_KEY_MINUS,
	Period = GLFW_KEY_PERIOD,
	Slash = GLFW_KEY_SLASH,
	K0 = GLFW_KEY_0,
	K1 = GLFW_KEY_1,
	K2 = GLFW_KEY_2,
	K3 = GLFW_KEY_3,
	K4 = GLFW_KEY_4,
	K5 = GLFW_KEY_5,
	K6 = GLFW_KEY_6,
	K7 = GLFW_KEY_7,
	K8 = GLFW_KEY_8,
	K9 = GLFW_KEY_9,
	Semicolon = GLFW_KEY_SEMICOLON,
	Equal = GLFW_KEY_EQUAL,
	A = GLFW_KEY_A,
	B = GLFW_KEY_B,
	C = GLFW_KEY_C,
	D = GLFW_KEY_D,
	E = GLFW_KEY_E,
	F = GLFW_KEY_F,
	G = GLFW_KEY_G,
	H = GLFW_KEY_H,
	I = GLFW_KEY_I,
	J = GLFW_KEY_J,
	K = GLFW_KEY_K,
	L = GLFW_KEY_L,
	M = GLFW_KEY_M,
	N = GLFW_KEY_N,
	O = GLFW_KEY_O,
	P = GLFW_KEY_P,
	Q = GLFW_KEY_Q,
	R = GLFW_KEY_R,
	S = GLFW_KEY_S,
	T = GLFW_KEY_T,
	U = GLFW_KEY_U,
	V = GLFW_KEY_V,
	W = GLFW_KEY_W,
	X = GLFW_KEY_X,
	Y = GLFW_KEY_Y,
	Z = GLFW_KEY_Z,
	LeftBracket = GLFW_KEY_LEFT_BRACKET,
	Backslash = GLFW_KEY_BACKSLASH,
	RightBracket = GLFW_KEY_RIGHT_BRACKET,
	GraveAccent = GLFW_KEY_GRAVE_ACCENT,
	World1 = GLFW_KEY_WORLD_1,
	World2 = GLFW_KEY_WORLD_2,
	Escape = GLFW_KEY_ESCAPE,
	Enter = GLFW_KEY_ENTER,
	Tab = GLFW_KEY_TAB,
	Backspace = GLFW_KEY_BACKSPACE,
	Insert = GLFW_KEY_INSERT,
	Delete = GLFW_KEY_DELETE,
	Right = GLFW_KEY_RIGHT,
	Left = GLFW_KEY_LEFT,
	Down = GLFW_KEY_DOWN,
	Up = GLFW_KEY_UP,
	PageUp = GLFW_KEY_PAGE_UP,
	PageDown = GLFW_KEY_PAGE_DOWN,
	Home = GLFW_KEY_HOME,
	End = GLFW_KEY_END,
	CapsLock = GLFW_KEY_CAPS_LOCK,
	ScrollLock = GLFW_KEY_SCROLL_LOCK,
	NumLock = GLFW_KEY_NUM_LOCK,
	PrintScreen = GLFW_KEY_PRINT_SCREEN,
	Pause = GLFW_KEY_PAUSE,
	F1 = GLFW_KEY_F1,
	F2 = GLFW_KEY_F2,
	F3 = GLFW_KEY_F3,
	F4 = GLFW_KEY_F4,
	F5 = GLFW_KEY_F5,
	F6 = GLFW_KEY_F6,
	F7 = GLFW_KEY_F7,
	F8 = GLFW_KEY_F8,
	F9 = GLFW_KEY_F9,
	F10 = GLFW_KEY_F10,
	F11 = GLFW_KEY_F11,
	F12 = GLFW_KEY_F12,
	F13 = GLFW_KEY_F13,
	F14 = GLFW_KEY_F14,
	F15 = GLFW_KEY_F15,
	F16 = GLFW_KEY_F16,
	F17 = GLFW_KEY_F17,
	F18 = GLFW_KEY_F18,
	F19 = GLFW_KEY_F19,
	F20 = GLFW_KEY_F20,
	F21 = GLFW_KEY_F21,
	F22 = GLFW_KEY_F22,
	F23 = GLFW_KEY_F23,
	F24 = GLFW_KEY_F24,
	F25 = GLFW_KEY_F25,
	NumPad0 = GLFW_KEY_KP_0,
	NumPad1 = GLFW_KEY_KP_1,
	NumPad2 = GLFW_KEY_KP_2,
	NumPad3 = GLFW_KEY_KP_3,
	NumPad4 = GLFW_KEY_KP_4,
	NumPad5 = GLFW_KEY_KP_5,
	NumPad6 = GLFW_KEY_KP_6,
	NumPad7 = GLFW_KEY_KP_7,
	NumPad8 = GLFW_KEY_KP_8,
	NumPad9 = GLFW_KEY_KP_9,
	NumPadDecimal = GLFW_KEY_KP_DECIMAL,
	NumPadDivide = GLFW_KEY_KP_DIVIDE,
	NumPadMultiply = GLFW_KEY_KP_MULTIPLY,
	NumPadSubtract = GLFW_KEY_KP_SUBTRACT,
	NumPadAdd = GLFW_KEY_KP_ADD,
	NumPadEnter = GLFW_KEY_KP_ENTER,
	NumPadEqual = GLFW_KEY_KP_EQUAL,
	LeftShift = GLFW_KEY_LEFT_SHIFT,
	LeftCtrl = GLFW_KEY_LEFT_CONTROL,
	LeftAlt = GLFW_KEY_LEFT_ALT,
	LeftSuper = GLFW_KEY_LEFT_SUPER,
	RightShift = GLFW_KEY_RIGHT_SHIFT,
	RightCtrl = GLFW_KEY_RIGHT_CONTROL,
	RightAlt = GLFW_KEY_RIGHT_ALT,
	RightSuper = GLFW_KEY_RIGHT_SUPER,
	Menu = GLFW_KEY_MENU,
	Last = GLFW_KEY_LAST
};

enum class MouseButton : int
{
	MouseButton1 = GLFW_MOUSE_BUTTON_1,
	MouseButton2 = GLFW_MOUSE_BUTTON_2,
	MouseButton3 = GLFW_MOUSE_BUTTON_3,
	MouseButton4 = GLFW_MOUSE_BUTTON_4,
	MouseButton5 = GLFW_MOUSE_BUTTON_5,
	MouseButton6 = GLFW_MOUSE_BUTTON_6,
	MouseButton7 = GLFW_MOUSE_BUTTON_7,
	MouseButton8 = GLFW_MOUSE_BUTTON_8,
	Last = GLFW_MOUSE_BUTTON_8,
	Left = GLFW_MOUSE_BUTTON_1,
	Right = GLFW_MOUSE_BUTTON_2,
	Middle = GLFW_MOUSE_BUTTON_3
};

enum class Action : int
{
	Down = GLFW_PRESS,
	Up = GLFW_RELEASE,
	Repeat = GLFW_REPEAT
};

enum class KeyState : int
{
	Pressed = GLFW_PRESS,
	Released = GLFW_RELEASE
};

enum class MouseButtonState : int
{
	Pressed = GLFW_PRESS,
	Released = GLFW_RELEASE
};

class MousePosition
{
	//ctor!
public:
	MousePosition()
	{}
	MousePosition(double x, double y, double ox, double oy) :	X(x),
																Y(y),
																oldX(ox),
																oldY(oy)
	{}
	double X;
	double Y;
	double oldX;
	double oldY;
};

class Modifier
{
public:
	Modifier() :	Alt(false),
					Ctrl(false),
					Shift(false),
					Super(false)
	{}
	Modifier(int mods) :	Alt(((mods & GLFW_MOD_ALT) != 0)),
							Ctrl(((mods & GLFW_MOD_CONTROL) != 0)),
							Shift(((mods & GLFW_MOD_SHIFT) != 0)),
							Super(((mods & GLFW_MOD_SUPER) != 0))
	{}
	bool Alt;
	bool Ctrl;
	bool Shift;
	bool Super;
};

//Handler Interface
class InputHandler
{
public:
	virtual void onKey(Key key, Action action, Modifier modifier) = 0;
	virtual void onMouseMove(MousePosition mouseposition) = 0;
	virtual void onMouseButton(MouseButton button, Action action, Modifier modifier) = 0;
	virtual void onMouseScroll(double xscroll, double yscroll) = 0;
};



class Input
{
public: //Contructors
	//Don't copy!
	Input(const Input&) = delete;
	//Don't move!
	Input(Input&&) = delete;

	Input(GLFWwindow* window);
	~Input();

public:
	//dispatchers
	static void key_dispatch(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mm_dispatch(GLFWwindow* window, double xpos, double ypos);
	static void mb_dispatch(GLFWwindow* window, int button, int action, int mods);
	static void mscr_dispatch(GLFWwindow* window, double xoffset, double yoffset);

public:
	void setHandlerInstance(Input* instance)
	{
		Input::handlerInstance = instance;
	}

private:
	//old mouse positions
	MousePosition mousepos;

	//InputHandlers
	std::vector<InputHandler*> inputhandlers;

	//glfw window
	GLFWwindow* glfwWindow;

	//instance for dispatching
	static Input* handlerInstance;
	

public: //public interface
	void addInputHandler(InputHandler*);
	void removeInputHandler(InputHandler*);

	KeyState getKeyState(Key key);
	MouseButtonState getMouseButtonState(MouseButton mousebutton);

	MousePosition getMousePosition();

	void setCursorVisible(bool visible);
};

#endif