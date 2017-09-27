#include "IModule_API.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
namespace SCM
{
	class TransformData
	{
	public:
		TransformData();
		~TransformData();
		glm::vec3 m_location;
		glm::quat m_rotation;
		glm::vec3 m_scale;
		glm::vec3 m_localY;
		glm::vec3 m_localX;
		glm::vec3 m_localZ;
		glm::mat4 m_transformMatrix;
		bool m_isMatrixDirty;
	private:
		
	};

	TransformData::TransformData()
	{
	}

	TransformData::~TransformData()
	{
	}

	class ShaderData
	{

	};

	class ISimpleContentModule_API : public IModule_API
	{
	public:









	};
}