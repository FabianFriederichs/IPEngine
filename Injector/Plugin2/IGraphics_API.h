#include "IModule_API.h"

class IGraphics_API : public IModule_API
{
public:
	virtual void render() = 0;
};