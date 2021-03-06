// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLUGIN2_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLUGIN2_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifndef _PHYSICS_MODULE_H_
#define _PHYSICS_MODULE_H_

#include <boost/config.hpp>
#include <IPhysicsModule_API.h>
#include <ISimpleSceneModule_API.h>
#include <core/memory/memory_utils.h>
#include <core/threading/TaskLib.h>
// This class is exported from the Plugin2.dll

#define MAX_SPRINGS_PER_PARTICLE 12
#define PBUF1 1
#define PBUF2 2
#define PARTICLES_PER_TASK 100

class PhysicsModule : public IPhysicsModule_API
{
public:
	PhysicsModule(void);
	// TODO: add your methods here.
	ModuleInformation* getModuleInfo()
	{
		return &m_info;
	}
	
	void createcloth()
	{
		/*contentmodule = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
		scenemodule = m_info.dependencies.getDep<ISimpleSceneModule_API>("SSM");
		auto clothtr = SCM::Transform(
			glm::vec3(0.0f, 0.0f, -3.0f),
			glm::quat(),
			glm::vec3(1.0f, 1.0f, 1.0f)
		);
		auto id = createCloth(
			"testcloth",
			50,
			50,
			clothtr,
			IPhysicsModule_API::PhysicsContext{
				glm::vec3(0.0f, -5.0f, 0.0f),
				0.3f,
				0.02f,
				40.0f,
				0.12f,
				40.0f,
				0.12f,
				30.0f,
				0.12f,
				true,
				true,
				true,
				true,
				0.01f,
				5,
				0.32f,
				true
			},
			11
		);

		fixParticle(id, 0, 49, true);
		fixParticle(id, 49, 49, true);
		scenemodule->AddEntity(id);
		contentmodule->getEntityById(id)->isActive = true;*/

	}
private:
	float m_pencm;
	bool m_doVelocityCollisionResponse;
	float m_collisionfric;
	ModuleInformation m_info;
	//scheduler subscription
	std::vector<ipengine::Scheduler::SubHandle> schedulerSubscriptionHandle;
	std::vector<ipengine::ipid> clothentities;
	boost::shared_ptr<SCM::ISimpleContentModule_API> contentmodule;
	boost::shared_ptr<ISimpleSceneModule_API> scenemodule;

	//private types
	class Particle
	{
	public:
		glm::vec3 m_position;
		glm::vec3 m_velocity;
		glm::vec3 m_acceleration;
		size_t m_fixdistance;
		size_t m_vertexIndex;
		size_t m_springIndex;
		size_t m_springCount;
		float m_mass;
		float m_radius;
		bool m_fixed;
		bool m_mark;
	};	

	class Spring
	{
	public:
		float m_ks;
		float m_kd;
		float m_restlength;
		size_t m_thisParticle;
		size_t m_connectedParticleIndex;
	};

	class Cloth
	{
	public:
		Cloth();
		Cloth(const Cloth& other) = delete;  //no copying! would be unnecessary complicated
		Cloth(Cloth&& other);
		Cloth& operator=(const Cloth& other) = delete;
		Cloth& operator=(Cloth&& other);
		~Cloth();		

		size_t particleCount();
		void swapBuffers();
		ipengine::aligned_ptr<Particle>& oldBuf();
		ipengine::aligned_ptr<Particle>& newBuf();

		ipengine::aligned_ptr<Particle> m_particles_buf1;
		ipengine::aligned_ptr<Particle> m_particles_buf2;
		ipengine::aligned_ptr<Spring> m_springs;
		PhysicsContext m_pctx;
		size_t m_width;
		size_t m_height;
		float m_distance;
		char m_current_old; //either 1 or 2

		ipengine::ipid id;
		SCM::Transform m_initialTransform;

		std::vector<size_t> m_csidx;
		moodycamel::ConcurrentQueue<ipengine::ipid> m_collisionqueue;
		std::atomic<ipengine::ipid> m_currentCollision;
		std::vector<ipengine::ipid> m_collidedEntities;
	};

	class UpdateBatch //32 bytes
	{
	public:
		Cloth* m_cloth;
		size_t from;
		size_t to;
		double dt;
	};

	class ClothUpdateInfo
	{
	public:
		Cloth* cloth;
		double dt;
	};

	class Plane
	{
	public:
		glm::vec3 n;
		glm::vec3 p;
	};

	typedef struct idx2d
	{
		size_t x;
		size_t y;
	} idx2d;

	//private member functions
	//main task function for this module
	void update(ipengine::TaskContext& context);
	//simulation functions
	void updateCloth(Cloth* cloth, double dt, ipengine::TaskContext & parentContext); //parallelize this!
	void updateCloth(ipengine::TaskContext & context);
	void satisfyConstraintBatch(ipengine::TaskContext& context);
	void updateParticleBatchPass1(ipengine::TaskContext& context);	//context is update batch
	void updateParticleBatchPass2(ipengine::TaskContext& context);	//context is update batch
	void handleCollisions(ipengine::TaskContext& context);

														//Put SCM types here
	glm::vec3 tryCollide(Cloth* cloth, Particle& particle, SCM::Entity* entity, float dt, bool&);
	/*glm::vec3 tryCollide(Cloth* cloth, Particle& particle, SCM::BoundingBox& collider, const glm::vec3& wpos, float dt, bool&);
	glm::vec3 tryCollide(Cloth* cloth, Particle& particle, SCM::BoundingSphere& collider, const glm::vec3& wpos, float dt, bool&);*/
	//void updateParticleBatch(ipengine::TaskContext& context);	//context is update batch
	glm::vec3 accumulateForces(Cloth* cloth, Particle& particle);
	inline glm::vec3 externalForces(Cloth* cloth, Particle& particle);
	//graphics stuff
	void updateMesh(Cloth* cloth);
	//helpers
	inline size_t index2D(size_t x, size_t y, size_t width)
	{
		return y * width + x;
	}

	//Should eventually go in another "Math" module or something
	float pointPlaneDistance(const glm::vec3& p, const Plane& plane);
	bool isOutsidePlane(const glm::vec3& p, const Plane& plane);
	bool intersectsPlane(const glm::vec3& p, float radius, const Plane& plane);

	Cloth* getCLothFromEID(const ipengine::ipid id);
	Cloth* getClothFromComponent(ClothComponent* ccomp);

	void clear();

public:
	Particle& getParticle(const ipengine::ipid name, size_t x, size_t y);
	//creation
	ipengine::ipid createCloth(const std::string& name, size_t width,
					 size_t height,
					 SCM::Transform& transform,
					 const PhysicsContext& physicsContext, const ipengine::ipid materialid) override;
	void destroyCloth(const ipengine::ipid id) override;
	void fixParticle(const ipengine::ipid id, size_t x, size_t y, bool fixed) override;

	ClothData getClothData(ipengine::ipid entityid)	override;

private:
	//private data
	std::vector<Cloth> clothInstances;
	ipengine::EndpointHandle collisionMessageEp;
	ipengine::MessageType collisionMessageType;
	int particles_per_task;
	ipengine::ipid clothComponentType;

	// Inherited via IPhysicsModule_API
	virtual bool _startup() override;

	// Inherited via IPhysicsModule_API
	virtual bool _shutdown() override;

	virtual bool tryIntersectSphereSphere(const glm::vec3 & s1pos, float s1rad, const glm::vec3 & s2pos, float s2rad, glm::vec4 & collisionout) override;

	virtual bool tryIntersectSphereBox(const glm::vec3 & spos, float srad, const glm::vec3 & bpos, const glm::quat & brot, const glm::vec3 & bscale, glm::vec4 & collisionout) override;

	virtual bool tryIntersectBoxBox(const glm::vec3 & b1pos, const glm::quat & b1rot, const glm::vec3 & b1scale, const glm::vec3 & b2pos, const glm::quat & b2rot, const glm::vec3 & b2scale, glm::vec4 & collisionout) override;

	float projectOntoAxis(const std::vector<glm::vec3>& points, size_t start, size_t n, const glm::vec3& axis);
};



extern "C" BOOST_SYMBOL_EXPORT PhysicsModule module;
PhysicsModule module;

#endif