#pragma once
#include <vector>
#include <Utils.h>
#include <glerror.h>
#include <memory>



class CVertex
{
public:
	CVertex(glm::vec3 _position = glm::vec3(0.0f), glm::vec3 _normal = glm::vec3(0.0f), glm::vec2 _uv = glm::vec2(0.0f)) :
		position(_position),
		normal(_normal),
		uv(_uv)
	{
	}
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

class SphereCollider
{
public:
	SphereCollider() = default;
	SphereCollider(const glm::vec3& center, float radius, bool fixed) :
		center(center), radius(radius), fixed(fixed)
	{}
	bool collide(std::vector<SphereCollider>& colliders);

	glm::vec3 center;
	float radius;
	bool fixed;
};

class Spring;
class ClothParticle
{
public:
	ClothParticle(CVertex* vertex, float mass, glm::vec3 velocity) :
		vertex(vertex), mass(mass), velocity(velocity)
	{

	}

	CVertex* vertex;
	float mass;
	glm::vec3 velocity;
	SphereCollider collider;
	std::vector<Spring*> springs;
	bool fixed;
};

class PhysicsContext
{
public:
	glm::vec3 gravity;
	std::vector<SphereCollider> globalColliders;

	float particleMass;
	float springKs;
	float springKd;

	float airfric;
};


class Spring
{
public:
	float ks;
	float kd;
	float restlength;
	ClothParticle* p1;
	ClothParticle* p2;
};

class Cloth
{
public:
	Cloth(int _width, int _height, glm::vec3 _position, float _distance);
	void render(double dt, bool wireframe);
	void update(double dt, PhysicsContext& context);
	void create(PhysicsContext& context);

private: //functions	
	void prepareForRender();
	void updateRenderData();
	void updateNormals();

	//physics stuff
	glm::vec3 accumulateForces(const ClothParticle& p, const PhysicsContext& context);

public:
	inline int width() { return m_width; }
	inline int height() { return m_height; }
	inline void fixParticle(size_t x, size_t y, bool fix)
	{
		m_particles[index2d(x, y, m_width)].fixed = fix;
	}

private:	//data
	int m_width;
	int m_height;
	float m_distance;
	glm::vec3 m_position;

	std::vector<CVertex> m_vertices;
	std::vector<ClothParticle> m_particles;
	std::vector<Spring> m_springs;
	std::vector<Index> m_indices;
	std::shared_ptr<VAO> m_vao;
};