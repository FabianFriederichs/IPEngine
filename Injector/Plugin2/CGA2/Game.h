#ifndef _GAME_H_
#define _GAME_H_

#include <GameWindow.h>
#include "Scene.h"

class Game : public GameWindow
{
public:
	Game();
	~Game();

	void init() override;
	void shutdown() override;

	void update(GLdouble dtime) override;
	void render(GLdouble dtime) override;

	void onKey(Key key, Action action, Modifier modifier) override;
	void onMouseMove(MousePosition mouseposition) override;
	void onMouseButton(MouseButton button, Action action, Modifier modifier) override;
	void onMouseScroll(double xscroll, double yscroll) override;

	void onFrameBufferResize(int width, int height) override;

private:
	Scene m_scene;
};

#endif
