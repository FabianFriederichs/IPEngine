#ifndef _IGRAPHICS_API_H_
#define _IGRAPHICS_API_H_
#include <IModule_API.h>

class IGraphics_API : public IModule_API
{
public:
	virtual void render() = 0;
};

#endif // !_IGRAPHICS_API_H_