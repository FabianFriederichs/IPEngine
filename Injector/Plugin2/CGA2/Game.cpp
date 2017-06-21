#include "Game.h"


Game::Game() : 
GameWindow(	1200,				//width
			900,				//height
			false,				//fullscreen
			true,				//vsync
			3,					//OpenGL Version Major
			3,					//OpenGL Version Minor	=> Here the OpenGL Version is 3.3
			"Testgame"),			//Tile of the window
m_scene(1200, 900)
{

}

Game::~Game()
{

}

//Initialization here. (i.e. load a scene, load textures ...)
void Game::init()
{
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	m_scene.init();
	input->setCursorVisible(false);
}

//cleanup. Free resources here.
void Game::shutdown()
{

}

//Update Game Logic here
void Game::update(GLdouble dtime)
{
	static int oldX = input->getMousePosition().X;
	static int oldY = input->getMousePosition().Y;
	if (input->getMouseButtonState(MouseButton::Left) == MouseButtonState::Pressed)
	{
		m_scene.phctx.globalColliders[0].center += glm::vec3((input->getMousePosition().X - oldX) * 0.001f, -(input->getMousePosition().Y - oldY) * 0.001f, 0.0f);
	}
	oldX = input->getMousePosition().X;
	oldY = input->getMousePosition().Y;
	m_scene.update(dtime);
}

//Render a frame
void Game::render(GLdouble dtime)
{
	glClear(GL_COLOR_BUFFER_BIT);
	m_scene.render(dtime);
}

//Keyboard events
void Game::onKey(Key key, Action action, Modifier modifier)
{
	if (key == Key::Escape && action == Action::Down)
		quit();

	if (key == Key::W && action == Action::Down)
	{
		m_scene.renderWireframe = !m_scene.renderWireframe;
	}
}

//Mouse move events
void Game::onMouseMove(MousePosition mouseposition)
{
	//m_scene.camera.Rotate(glm::vec3(0.1f * (mouseposition.X - mouseposition.oldX), 0.1f * (mouseposition.Y - mouseposition.oldY), 0.0f));
	int dummy = 0;
}

//Mouse Button events
void Game::onMouseButton(MouseButton button, Action action, Modifier modifier)
{

}

//Mouse scroll events
void Game::onMouseScroll(double xscroll, double yscroll)
{

}

//Window resize events
void Game::onFrameBufferResize(int width, int height)
{
	
}
