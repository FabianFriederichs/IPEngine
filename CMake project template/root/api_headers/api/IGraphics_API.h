#ifndef _IGRAPHICS_API_H_
#define _IGRAPHICS_API_H_
#include <IModule_API.h>

namespace GrAPI
{
	struct t2d
	{
		//t2d() {};
		t2d(uint32_t w, uint32_t h, uint32_t c, const uint8_t* d): width(w), height(h), channels(c), data(d){}
		const uint32_t width, height, channels;
		const uint8_t* data;
	};
}

class IGraphics_API : public IModule_API
{
public:
	virtual void render() = 0;
	virtual void render(int fbo, int viewportx, int viewporty, bool multisample) = 0;
	virtual void setCameraEntity(ipengine::ipid) = 0;
	virtual void setFOV(uint32_t) = 0;
	virtual void setResolution(uint32_t, uint32_t) = 0;
	virtual void setClipRange(float, float) = 0;

	virtual ipengine::ipid getCameraEntity() = 0;
	virtual uint32_t getFOV() = 0;
	virtual void getResolution(uint32_t&, uint32_t&) = 0;
	virtual void getClipRange(float&, float&) = 0;
	virtual void loadTextureFromMemory(const GrAPI::t2d&, const ipengine::ipid) = 0;
	virtual void setMaterialTexDefaultParams(ipengine::ipid, bool) = 0;
	struct renderMatrixes
	{
		glm::mat4* proj;
		glm::mat4* view;
	};
};

#endif // !_IGRAPHICS_API_H_