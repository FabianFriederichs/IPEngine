#ifndef _SCENE_H_
#define _SCENE_H_
#include "Cloth.h"
#include "Utils.h"
#include <memory>
#include <libheaders.h>
#include "Cloth.h"
#include "Camera.h"
class Scene
{
public:
	Scene(int width, int height);

	void init();
	void update(double dt);
	void render(double dt);

private:
	int windowWidth;
	int windowHeight;

	Cloth m_cloth;
	std::shared_ptr<ShaderProgram> m_shader;

	glm::mat4 worldmat;
	glm::mat4 viewmat;
	glm::mat4 projmat;
	glm::vec3 lightdir;
	glm::vec3 lightcolor;

public:
	PhysicsContext phctx;


	
public:
	//render settings
	bool renderWireframe;
};



#endif