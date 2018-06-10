#ifndef _I_PHYSICS_MODULE_API_H_
#define _SIMPLE_CONTENT_MODULE_H_

#include <IModule_API.h>
#include <glm/glm.hpp>
#include <ISimpleContentModule_API.h>

//physics message type ids
#define IPMSG_CLOTH_ENTITY_COLLISION 1

class IPhysicsModule_API : public IModule_API
{
public:
	//public types
	class Collision
	{
	public:
		ipengine::ipid e1;
		ipengine::ipid e2;
	};

	class PhysicsContext
	{
	public:
		glm::vec3 gravity;
		float particleMass;
		float particleDistance;
		float struct_springKs;
		float struct_springKd;
		float shear_springKs;
		float shear_springKd;
		float bend_springKs;
		float bend_springKd;
		bool struct_springs;
		bool shear_springs;
		bool bend_springs;
		bool use_constraints;
		float max_stretch;
		int constraint_iterations;
		float airfric;
		bool two_pass_integration;
	};

	struct ParticleCoord
	{
		size_t x;
		size_t y;
	};

	class ClothData
	{
	public:
		ClothData() :
			width(0),
			height(0)
		{}
		size_t width;
		size_t height;
		SCM::Transform* transform;
		PhysicsContext pcontext;
		std::vector<ParticleCoord> fixedParticles;
	};

	//TODO: Optimization: use internalID as array index for cloth instance
	class ClothComponent : public SCM::Component
	{
	public:
		ClothComponent(const ipengine::ipid& iid, const ipengine::ipid& tid, const ipengine::ipid& eid, IPhysicsModule_API* _pmod) :
			Component(iid, tid, eid),
			pmod(_pmod)
		{}

		~ClothComponent()
		{
			pmod->destroyCloth(getEntity());
		}

		ClothData getClothData()
		{
			return pmod->getClothData(getEntity());
		}

	private:
		IPhysicsModule_API* pmod;
	};

	using ClothVertex = SCM::VertexData;


	using ClothIndex = SCM::index;

	virtual ipengine::ipid createCloth(const std::string& name, size_t width,
					 size_t height,
					 SCM::Transform& transform,
					 const PhysicsContext& physicsContext, const ipengine::ipid materialid) = 0;
	virtual void destroyCloth(const ipengine::ipid) = 0;
	virtual void fixParticle(const ipengine::ipid id, size_t x, size_t y, bool fixed) = 0;
	virtual void createcloth() = 0;

	virtual ClothData getClothData(ipengine::ipid entityid) = 0;
	//virtual ClothData getClothData(ipengine::ipid entityid) = 0;
	//virtual ClothData getClothData(SCM::Entity* entity) = 0;

	//intersection tests

	//sphere-sphere
	virtual bool tryIntersectSphereSphere(const glm::vec3& s1pos, float s1rad, const glm::vec3& s2pos, float s2rad, glm::vec4& collisionout) = 0;
	//sphere-box
	virtual bool tryIntersectSphereBox(const glm::vec3& spos, float srad, const glm::vec3& bpos, const glm::quat& brot, const glm::vec3& bscale, glm::vec4& collisionout) = 0;
	//box-box
	virtual bool tryIntersectBoxBox(
		const glm::vec3& b1pos, const glm::quat& b1rot, const glm::vec3& b1scale,
		const glm::vec3& b2pos, const glm::quat& b2rot, const glm::vec3& b2scale, glm::vec4& collisionout) = 0;
};

#endif