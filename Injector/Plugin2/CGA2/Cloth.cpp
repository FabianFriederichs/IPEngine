#include <Cloth.h>

Cloth::Cloth(int _width, int _height, glm::vec3 _position, float _distance) :
	m_width(_width), m_height(_height), m_position(_position), m_distance(_distance)
{
	
}

void Cloth::render(double dt, bool wireframe)
{
	m_vao->bind();
	glPolygonMode(GL_FRONT_AND_BACK, (wireframe ? GL_LINE : GL_FILL));
	glDrawElements(GL_TRIANGLES, m_vao->indexCount, GL_UNSIGNED_INT, reinterpret_cast<const void*>(0));
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_vao->unbind();
}

void Cloth::update(double dt, PhysicsContext& context)
{
	//Velocity-Verlet pass 1
	std::vector<glm::vec3> old_accelerations;
	old_accelerations.reserve(m_particles.size());
	for (int i = 0; i < m_particles.size(); i++)	//Update position vectors
	{
		glm::vec3 an = accumulateForces(m_particles[i], context) / m_particles[i].mass;
		old_accelerations.push_back(an);
		m_particles[i].vertex->position = m_particles[i].vertex->position + m_particles[i].velocity * static_cast<float>(dt) + 0.5f * an * static_cast<float>(dt) * static_cast<float>(dt);
		m_particles[i].collider.center = m_particles[i].vertex->position;
	}

	//Velocity-Verlet pass 2
	for (int i = 0; i < m_particles.size(); i++)	//Update velocity vectors
	{
		glm::vec3 an1 = accumulateForces(m_particles[i], context) / m_particles[i].mass;
		m_particles[i].velocity = m_particles[i].velocity + ((an1 + old_accelerations[i]) / 2.0f) * static_cast<float>(dt);
	}

	for (int i = 0; i < m_particles.size(); i++)
	{
		bool isCollided = m_particles[i].collider.collide(context.globalColliders);
		if (isCollided)
		{
			m_particles[i].vertex->position = m_particles[i].collider.center;
			m_particles[i].velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}

	//Update other stuff


	//Semi-implicit euler method
	/*for (size_t i = 0; i < m_particles.size(); ++i)
	{
		if (m_particles[i].fixed)
			continue;
		glm::vec3 force = accumulateForces(m_particles[i], context);
		m_particles[i].velocity = m_particles[i].velocity + (force / m_particles[i].mass) * static_cast<float>(dt);
		m_particles[i].vertex->position = m_particles[i].vertex->position + m_particles[i].velocity * static_cast<float>(dt);
		m_particles[i].collider.center = m_particles[i].vertex->position;

		
	}*/
	updateRenderData();
}

void Cloth::create(PhysicsContext& context)
{
	m_vertices.reserve(m_width * m_height);
	m_particles.reserve(m_width * m_height);
	m_springs.reserve(m_width * m_height * 12);
	glm::vec3 temp_position((m_position.x - (((m_width - 1) * m_distance)/2)),m_position.y, (m_position.z + (((m_height - 1) * m_distance) / 2)));
	//m_vertices
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			CVertex vert = CVertex(
				glm::vec3(temp_position + glm::vec3(x * m_distance, 0, -y * m_distance)),
				glm::vec3(0.0f),
				glm::vec2((1.0f / m_width) * static_cast<float>(x), (1.0f / m_height) * static_cast<float>(y))
				);
			m_vertices.push_back(vert);
			ClothParticle p(&m_vertices[m_vertices.size() - 1], context.particleMass , glm::vec3(0.0f, 0.0f, 0.0f));
			p.collider = SphereCollider(vert.position, m_distance, false);
			m_particles.push_back(p);
		}
	}
	float shearspringrestlth = glm::sqrt(glm::pow(m_distance, 2) * 2);
	float bendingspringrestlth = m_distance * 2.0f;
	for (int y = 0; y < m_height - 1; ++y)
	{
		for (int x = 0; x < m_width - 1; ++x)
		{
			//upper tri
			m_indices.push_back(index2d(x, y, m_width));
			m_indices.push_back(index2d(x + 1, y + 1, m_width));
			m_indices.push_back(index2d(x, y + 1, m_width));
			//lower tri
			m_indices.push_back(index2d(x, y, m_width));
			m_indices.push_back(index2d(x + 1, y, m_width));
			m_indices.push_back(index2d(x + 1, y + 1, m_width));

			//springs
			//structural springs
			Spring s1;
			s1.ks = context.springKs;
			s1.kd = context.springKd;
			s1.restlength = m_distance;
			s1.p1 = &m_particles[index2d(x, y, m_width)];
			s1.p2 = &m_particles[index2d(x + 1, y, m_width)];
			m_springs.push_back(s1);
			m_particles[index2d(x, y, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
			m_particles[index2d(x + 1, y, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);

			Spring s2;
			s2.ks = context.springKs;
			s2.kd = context.springKd;
			s2.restlength = m_distance;
			s2.p1 = &m_particles[index2d(x, y, m_width)];
			s2.p2 = &m_particles[index2d(x, y + 1, m_width)];
			m_springs.push_back(s2);
			m_particles[index2d(x, y, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
			m_particles[index2d(x, y + 1, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);

			if (x == m_width - 2)
			{
				Spring s3;
				s3.ks = context.springKs;
				s3.kd = context.springKd;
				s3.restlength = m_distance;
				s3.p1 = &m_particles[index2d(x + 1, y, m_width)];
				s3.p2 = &m_particles[index2d(x + 1, y + 1, m_width)];
				m_springs.push_back(s3);
				m_particles[index2d(x + 1, y, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
				m_particles[index2d(x + 1, y + 1, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
			}
			if (y == m_height - 2)
			{
				Spring s4;
				s4.ks = context.springKs;
				s4.kd = context.springKd;
				s4.restlength = m_distance;
				s4.p1 = &m_particles[index2d(x, y + 1, m_width)];
				s4.p2 = &m_particles[index2d(x + 1, y + 1, m_width)];
				m_springs.push_back(s4);
				m_particles[index2d(x, y + 1, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
				m_particles[index2d(x + 1, y + 1, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
			}

			
			//shear springs
			Spring s5;
			s5.ks = context.springKs;
			s5.kd = context.springKd;
			s5.restlength = shearspringrestlth;
			s5.p1 = &m_particles[index2d(x, y, m_width)];
			s5.p2 = &m_particles[index2d(x + 1, y + 1, m_width)];
			m_springs.push_back(s5);
			m_particles[index2d(x, y, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
			m_particles[index2d(x + 1, y + 1, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);

			if (x >= 1)
			{
				Spring s6;
				s6.ks = context.springKs;
				s6.kd = context.springKd;
				s6.restlength = shearspringrestlth;
				s6.p1 = &m_particles[index2d(x, y, m_width)];
				s6.p2 = &m_particles[index2d(x - 1, y + 1, m_width)];
				m_springs.push_back(s6);
				m_particles[index2d(x, y, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
				m_particles[index2d(x - 1, y + 1, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
			}

			if (x + 2 < m_width)
			{
				//bending springs
				Spring s7;
				s7.ks = context.springKs;
				s7.kd = context.springKd;
				s7.restlength = bendingspringrestlth;
				s7.p1 = &m_particles[index2d(x, y, m_width)];
				s7.p2 = &m_particles[index2d(x + 2, y, m_width)];
				m_springs.push_back(s7);
				m_particles[index2d(x, y, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
				m_particles[index2d(x + 2, y, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
			}

			if (y + 2 < m_height)
			{
				Spring s8;
				s8.ks = context.springKs;
				s8.kd = context.springKd;
				s8.restlength = bendingspringrestlth;
				s8.p1 = &m_particles[index2d(x, y, m_width)];
				s8.p2 = &m_particles[index2d(x, y + 2, m_width)];
				m_springs.push_back(s8);
				m_particles[index2d(x, y, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
				m_particles[index2d(x, y + 2, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
			}
		}
	}

	//add missing bending springs at the rim
	//top rim
	for (int x = 0; x < m_width - 2; x++)
	{
		Spring s9;
		s9.ks = context.springKs;
		s9.kd = context.springKd;
		s9.restlength = bendingspringrestlth;
		s9.p1 = &m_particles[index2d(x, m_height - 1, m_width)];
		s9.p2 = &m_particles[index2d(x + 2, m_height - 1, m_width)];
		m_springs.push_back(s9);
		m_particles[index2d(x, m_height - 1, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
		m_particles[index2d(x + 2, m_height - 1, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
	}

	for (int y = 0; y < m_height - 2; y++)
	{
		Spring s10;
		s10.ks = context.springKs;
		s10.kd = context.springKd;
		s10.restlength = bendingspringrestlth;
		s10.p1 = &m_particles[index2d(m_width - 1, y, m_width)];
		s10.p2 = &m_particles[index2d(m_width - 1, y + 2, m_width)];
		m_springs.push_back(s10);
		m_particles[index2d(m_width - 1, y, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
		m_particles[index2d(m_width - 1, y + 2, m_width)].springs.push_back(&m_springs[m_springs.size() - 1]);
	}

	//calc normals
	updateNormals();
	prepareForRender();
	
}

void Cloth::updateNormals()
{
	try
	{
		for (size_t i = 0; i < m_vertices.size(); i++) //initialize all Vertex normals with nullvectors
		{
			m_vertices[i].normal = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		for (size_t i = 0; i < m_indices.size(); i += 3)
		{
			glm::vec3 v1 = m_vertices[m_indices[i]].position;
			glm::vec3 v2 = m_vertices[m_indices[i + 1]].position;
			glm::vec3 v3 = m_vertices[m_indices[i + 2]].position;

			//counter clockwise winding
			glm::vec3 edge1 = v2 - v1;
			glm::vec3 edge2 = v3 - v1;

			glm::vec3 normal = glm::cross(edge1, edge2);

			//for each Vertex all corresponing normals are added. The result is a non unit length vector wich is the average direction of all assigned normals.
			m_vertices[m_indices[i]].normal += normal;
			m_vertices[m_indices[i + 1]].normal += normal;
			m_vertices[m_indices[i + 2]].normal += normal;
		}

		for (size_t i = 0; i < m_vertices.size(); i++)	//normalize all normals calculated in the previous step
		{
			m_vertices[i].normal = glm::normalize(m_vertices[i].normal);
		}
		int dummy = 0;
	}
	catch (const std::exception& ex)
	{
		throw ex;
	}
}

void Cloth::prepareForRender()
{
	GLuint vao;
	glGenVertexArrays(1, &vao); GLERR
	if (vao == 0)
	{
		throw std::logic_error("VAO could not be created.");
	}
	GLuint vbo;
	glGenBuffers(1, &vbo);
	if (vbo == 0)
	{
		glDeleteVertexArrays(1, &vao);
		throw std::logic_error("VBO could not be created.");
	}
	GLuint ibo;
	glGenBuffers(1, &ibo);	
	if (ibo == 0)
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		throw std::logic_error("IBO could not be created.");
	}
	glBindVertexArray(vao);	GLERR
	glBindBuffer(GL_ARRAY_BUFFER, vbo); GLERR
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo); GLERR
	glBufferData(GL_ARRAY_BUFFER, sizeof(CVertex) * m_vertices.size(), reinterpret_cast<const GLvoid*>(m_vertices.data()), GL_DYNAMIC_DRAW); GLERR
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index) * m_indices.size(), reinterpret_cast<const GLvoid*>(m_indices.data()), GL_STATIC_DRAW); GLERR
	glVertexAttribPointer(0, 3,	GL_FLOAT, GL_FALSE,	sizeof(CVertex),	reinterpret_cast<void*>(offsetof(CVertex, position))); GLERR
	glEnableVertexAttribArray(0); GLERR
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CVertex), reinterpret_cast<void*>(offsetof(CVertex, normal))); GLERR
	glEnableVertexAttribArray(1); GLERR
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CVertex), reinterpret_cast<void*>(offsetof(CVertex, uv))); GLERR
	glEnableVertexAttribArray(2); GLERR
	glBindBuffer(GL_ARRAY_BUFFER, 0); GLERR
	glBindVertexArray(0); GLERR
	m_vao = std::make_shared<VAO>(vbo, ibo, vao, m_indices.size());
}

void Cloth::updateRenderData()
{
	updateNormals();
	glBindBuffer(GL_ARRAY_BUFFER, m_vao->vbo); GLERR
	glBufferData(GL_ARRAY_BUFFER, sizeof(CVertex) * m_vertices.size(), reinterpret_cast<const GLvoid*>(m_vertices.data()), GL_DYNAMIC_DRAW); GLERR
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

glm::vec3 Cloth::accumulateForces(const ClothParticle & p, const PhysicsContext & context)
{
	glm::vec3 forceaccum(0.0f);
	//springs
	for (Spring* s : p.springs)
	{
		ClothParticle* p1 = (s->p1 == &p ? s->p1 : s->p2);	//position of this particle
		ClothParticle* p2 = (s->p1 == &p ? s->p2 : s->p1);
		glm::vec3 pos1 = (s->p1 == &p ? s->p1 : s->p2)->vertex->position;	//position of this particle
		glm::vec3 pos2 = (s->p1 == &p ? s->p2 : s->p1)->vertex->position;	//position of other particle

		glm::vec3 er = glm::normalize(pos2 - pos1);
		float auslenkung = glm::length(pos2 - pos1) - s->restlength;
		glm::vec3 force = er * s->ks * auslenkung - er * s->kd * glm::dot(p.velocity, er);
		
		forceaccum += force;
	}
	forceaccum += p.mass * context.gravity;
	
	//luftreibung
	forceaccum += -p.velocity * context.airfric;

	return forceaccum;
}

bool SphereCollider::collide(std::vector<SphereCollider>& colliders)
{
	for (auto& col : colliders)
	{
		if (glm::length(col.center - this->center) < (this->radius + col.radius) && col.fixed)
		{
			glm::vec3 dir = glm::normalize(this->center - col.center);
			glm::vec3 pos = col.center + dir * (col.radius + this->radius);
			this->center = pos;
			return true;
		}
	}
	return false;
}
