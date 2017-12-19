#ifndef _IGRAPHICS_API_H_
#define _IGRAPHICS_API_H_
#include <IModule_API.h>

class IGraphics_API : public IModule_API
{
public:
	virtual void render() = 0;
	virtual void setCameraEntity(uint32_t) = 0;
	virtual void setFOV(uint32_t) = 0;
	virtual void setResolution(uint32_t, uint32_t) = 0;
	virtual void setClipRange(uint32_t, uint32_t) = 0;

	virtual uint32_t getCameraEntity() = 0;
	virtual uint32_t getFOV() = 0;
	virtual void getResolution(uint32_t&, uint32_t&) = 0;
	virtual void getClipRange(uint32_t&, uint32_t&) = 0;

	struct renderMatrixes
	{
		glm::mat4* proj;
		glm::mat4* view;
	};
};

#endif // !_IGRAPHICS_API_H_