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
#include <IPCore/Memory/memory_utils.h>
#include <IPCore/ThreadingServices/TaskLib.h>
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
	bool startUp()
	{
		contentmodule = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
		scenemodule = m_info.dependencies.getDep<ISimpleSceneModule_API>("SSM");
		auto id = createCloth(
			"testcloth",
			50,
			50,
			SCM::TransformData{
				glm::vec3(0.0f, 0.0f, 0.0f),
				glm::quat(),
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(0.0f, 0.0f, 1.0f),
				glm::mat4(1.0f),
				false
			},
			IPhysicsModule_API::PhysicsContext{
				glm::vec3(0.0f, -9.81f, 0.2f),
				1.45f,
				0.6f,
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
				0.02f,
				5,
				0.12f,
				true
			},
			0
		);

		clothentities.push_back(id);
		scenemodule->AddEntity(id);

		/*	m_info.dependencies.getDep<IPrinter_API>("printer")->printStuffToSomething(m_info.identifier + " successfully started up as " + m_info.iam); return true; */
		schedulerSubscriptionHandle.push_back(
			m_core->getScheduler().subscribe(
				ipengine::TaskFunction::make_func<PhysicsModule, &PhysicsModule::update>(this),
				8e6,
				ipengine::Scheduler::SubType::Interval,
				1.0f,
				&m_core->getThreadPool()
		));
		return true;
	} //do stuff?
private:
	ModuleInformation m_info;
	//scheduler subscription
	std::vector<ipengine::Scheduler::SubHandle> schedulerSubscriptionHandle;
	std::vector<SCM::EntityId> clothentities;
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
		~Cloth();

		SCM::EntityId id;
		size_t m_width;
		size_t m_height;
		size_t particleCount();
		float m_distance;
		//pingpong between these two particle buffers
		//use cache aligned storage to prevent false sharing
		ipengine::aligned_ptr<Particle> m_particles_buf1;
		ipengine::aligned_ptr<Particle> m_particles_buf2;
		ipengine::aligned_ptr<Spring> m_springs;
		PhysicsContext m_pctx;
		char m_current_old; //either 1 or 2

		void swapBuffers();
		ipengine::aligned_ptr<Particle>& oldBuf();
		ipengine::aligned_ptr<Particle>& newBuf();

		std::vector<size_t> m_csidx;
		//std::vector<size_t> m_fixedparticles;
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
	glm::vec3 tryCollide(Cloth* cloth, Particle& particle, SCM::BoundingBox& collider, float dt);
	glm::vec3 tryCollide(Cloth* cloth, Particle& particle, SCM::BoundingSphere& collider, float dt);
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

public:
	Particle& getParticle(const SCM::EntityId name, size_t x, size_t y);
	//creation
	SCM::EntityId createCloth(const std::string& name, size_t width,
					 size_t height,
					 const  SCM::TransformData& transform,
					 const PhysicsContext& physicsContext, const SCM::IdType materialid) override;
	void destroyCloth(const SCM::EntityId id) override;
	void fixParticle(const SCM::EntityId id, size_t x, size_t y, bool fixed) override;

private:
	//private data
	std::vector<Cloth> clothInstances;
	
};



extern "C" BOOST_SYMBOL_EXPORT PhysicsModule module;
PhysicsModule module;

#endif