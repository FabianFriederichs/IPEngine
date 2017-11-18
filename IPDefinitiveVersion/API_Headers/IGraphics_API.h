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

};

#endif // !_IGRAPHICS_API_H_