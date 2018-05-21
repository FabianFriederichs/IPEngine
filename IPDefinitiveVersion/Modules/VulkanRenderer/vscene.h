#ifndef VSCENE_H
#define VSCENE_H
#include "vmesh.h"
#include "directional_light.h"
#include <IPCore\Core\ICoreTypes.h>

class VScene
{
public:
	Skybox skybox;
	DirectionalLight shadowLight;
	std::vector<std::shared_ptr<VMesh>> meshes;

	BBox aabbWorldSpace;

	VScene(rj::VManager *pManager);

	void computeAABBWorldSpace();
};

#endif