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
		SCM::EntityId e1;
		SCM::EntityId e2;
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

	using ClothVertex = SCM::VertexData;


	using ClothIndex = SCM::index;

	virtual SCM::EntityId createCloth(const std::string& name, size_t width,
					 size_t height,
					 const SCM::TransformData& transform,
					 const PhysicsContext& physicsContext, const SCM::IdType materialid) = 0;
	virtual void destroyCloth(const SCM::EntityId) = 0;
	virtual void fixParticle(const SCM::EntityId id, size_t x, size_t y, bool fixed) = 0;
};

#endif