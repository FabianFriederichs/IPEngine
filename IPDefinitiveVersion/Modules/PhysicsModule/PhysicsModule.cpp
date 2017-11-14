// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "PhysicsModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
PhysicsModule::PhysicsModule()
{
	m_info.identifier = "PhysicsModule";
	m_info.version = "1.0";
	m_info.iam = "IPhysicsModule_API";
	return;
}

//Main update task -------------------------------------------------------------------------------------------------------------

void PhysicsModule::update(ipengine::TaskContext & context)
{
	//context.getPool()->t_end.store(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
	//std::cout << context.getPool()->t_end - context.getPool()->t_start << "us\n";
	double dt = context;
	/*for (Cloth& cloth : clothInstances)
	{


	updateCloth(&cloth, dt, context);
	updateMesh(&cloth);
	}*/

	ipengine::TaskHandle updateMainTask = context.getPool()->createEmpty();
	for (Cloth& cloth : clothInstances)
	{
		ClothUpdateInfo ub{
			&cloth,
			dt
		};
		ipengine::TaskHandle child = context.getPool()->createChild(ipengine::TaskFunction::make_func<PhysicsModule, &PhysicsModule::updateCloth>(this),
																	ipengine::TaskContext(ub),
																	updateMainTask);
		child.spawn(&context);
	}
	updateMainTask.spawn(&context);
	updateMainTask.wait(&context);
}

//Cloth update -----------------------------------------------------------------------------------------------------------------

void PhysicsModule::updateCloth(Cloth * cloth, double dt, ipengine::TaskContext & parentContext)
{
	//single threaded update batch
	UpdateBatch ub{
		cloth,
		0,
		cloth->particleCount() - 1,
		dt
	};

	//do simulation
	//apply constraints
	if (cloth->m_pctx.use_constraints)
	{
		for (int i = 0; i < cloth->m_pctx.constraint_iterations; i++)
		{
			ipengine::TaskHandle constraintMainTask = parentContext.getPool()->createEmpty();
			for (size_t p = 0; p < cloth->particleCount(); p += PARTICLES_PER_TASK)
			{
				UpdateBatch ub{
					cloth,
					p,
					(p + PARTICLES_PER_TASK >= cloth->particleCount() ? cloth->particleCount() - 1 : p + PARTICLES_PER_TASK),
					dt
				};
				ipengine::TaskHandle child = parentContext.getPool()->createChild(ipengine::TaskFunction::make_func<PhysicsModule, &PhysicsModule::satisfyConstraintBatch>(this),
																				  ipengine::TaskContext(ub),
																				  constraintMainTask);
				child.spawn(&parentContext);
			}
			constraintMainTask.spawn(&parentContext);
			constraintMainTask.wait(&parentContext);

			//satisfyConstraintBatch(ub);
			cloth->swapBuffers();
		}
		//cloth->swapBuffers();
	}

	//collisions
	ipengine::TaskHandle collisionContext = parentContext.getPool()->createEmpty();
	for (size_t p = 0; p < cloth->particleCount(); p += PARTICLES_PER_TASK)
	{
		UpdateBatch ub{
			cloth,
			p,
			(p + PARTICLES_PER_TASK >= cloth->particleCount() ? cloth->particleCount() - 1 : p + PARTICLES_PER_TASK),
			dt
		};
		ipengine::TaskHandle child = parentContext.getPool()->createChild(ipengine::TaskFunction::make_func<PhysicsModule, &PhysicsModule::handleCollisions>(this),
																		  ipengine::TaskContext(ub),
																		  collisionContext);
		child.spawn(&parentContext);
	}
	collisionContext.spawn(&parentContext);
	collisionContext.wait(&parentContext);
	cloth->swapBuffers();


	//pass1
	ipengine::TaskHandle updateMainTask = parentContext.getPool()->createEmpty();
	for (size_t p = 0; p < cloth->particleCount(); p += PARTICLES_PER_TASK)
	{
		UpdateBatch ub{
			cloth,
			p,
			(p + PARTICLES_PER_TASK >= cloth->particleCount() ? cloth->particleCount() - 1 : p + PARTICLES_PER_TASK),
			dt
		};
		ipengine::TaskHandle child = parentContext.getPool()->createChild(ipengine::TaskFunction::make_func<PhysicsModule, &PhysicsModule::updateParticleBatchPass1>(this),
																		  ipengine::TaskContext(ub),
																		  updateMainTask);
		child.spawn(&parentContext);
	}
	updateMainTask.spawn(&parentContext);
	updateMainTask.wait(&parentContext);

	cloth->swapBuffers();

	//pass 2

	ipengine::TaskHandle updateMainTask2 = parentContext.getPool()->createEmpty();
	for (size_t p = 0; p < cloth->particleCount(); p += PARTICLES_PER_TASK)
	{
		UpdateBatch ub{
			cloth,
			p,
			(p + PARTICLES_PER_TASK >= cloth->particleCount() ? cloth->particleCount() - 1 : p + PARTICLES_PER_TASK),
			dt
		};
		ipengine::TaskHandle child = parentContext.getPool()->createChild(ipengine::TaskFunction::make_func<PhysicsModule, &PhysicsModule::updateParticleBatchPass2>(this),
																		  ipengine::TaskContext(ub),
																		  updateMainTask2);
		child.spawn(&parentContext);
	}
	updateMainTask2.spawn(&parentContext);
	updateMainTask2.wait(&parentContext);
	//updateParticleBatchPass2(ipengine::TaskContext(ub));
	cloth->swapBuffers();




}

void PhysicsModule::updateCloth(ipengine::TaskContext & context)
{
	ClothUpdateInfo cui = context;
	updateCloth(cui.cloth, cui.dt, context);
	updateMesh(cui.cloth);
}

//Simulation functions ---------------------------------------------------------------------------------------------------------
//save nearest fixed particle and prefer that direction!
void PhysicsModule::satisfyConstraintBatch(ipengine::TaskContext& context) //fix for super elasticity
{
	UpdateBatch& ub = context;

	float dt = static_cast<float>(ub.dt);
	auto& preadbuf = ub.m_cloth->oldBuf();
	auto& pwritebuf = ub.m_cloth->newBuf();

	//postiton based constraint enforcement
	/*for (int ci = 0; ci < ub.m_cloth->m_pctx.constraint_iterations; ci++)
	{
	bool allConstraintsSolved = true;
	for (size_t i = ub.from; i < ub.to; i++)
	{
	Particle& p1 = preadbuf[ub.m_cloth->m_csidx[i]];

	for (size_t s = p1.m_springIndex; s < p1.m_springIndex + p1.m_springCount; s++)
	{
	const Spring& spring = ub.m_cloth->m_springs[s];
	Particle& p2 = preadbuf[spring.m_connectedParticleIndex];

	glm::vec3 distance = p2.m_position - p1.m_position;
	float error = glm::length(distance) - spring.m_restlength;
	if (glm::abs(error) < ub.m_cloth->m_pctx.max_stretch)
	continue;

	allConstraintsSolved = false;

	glm::vec3 cdir = glm::normalize(distance);

	p1.m_position = p1.m_fixed ? p1.m_position : (p2.m_fixed ? p1.m_position + (cdir * error) : p1.m_position + ((cdir * error) / 2.0f));
	p2.m_position = p2.m_fixed ? p2.m_position : (p1.m_fixed ? p2.m_position - (cdir * error) : p2.m_position - ((cdir * error) / 2.0f));
	}
	}
	if (allConstraintsSolved)
	break;
	}*/

	//impulse based constraint enforcement, single threaded, direct
	//for(int ci = 0; ci < ub.m_cloth->m_pctx.constraint_iterations; ci++)
	//{
	//	bool allConstraintsSatisfied = true;
	//	for (size_t i = ub.from; i < ub.to; i++)
	//	{
	//		Particle& p1 = preadbuf[i];//[ub.m_cloth->m_csidx[i]];
	//		Particle& wp1 = pwritebuf[i];//[ub.m_cloth->m_csidx[i]];

	//		for (size_t s = p1.m_springIndex; s < p1.m_springIndex + p1.m_springCount; s++)
	//		{
	//			const Spring& spring = ub.m_cloth->m_springs[s];
	//			Particle& p2 = preadbuf[spring.m_connectedParticleIndex];
	//			Particle& wp2 = pwritebuf[spring.m_connectedParticleIndex];

	//			if (p1.m_fixed && p2.m_fixed)
	//				continue;

	//			auto previewpos1 = p1.m_position + p1.m_velocity * static_cast<float>(ub.dt) + 0.5f * p1.m_acceleration * static_cast<float>(ub.dt * ub.dt);

	//			auto previewpos2 = p2.m_position + p2.m_velocity * static_cast<float>(ub.dt) + 0.5f * p2.m_acceleration * static_cast<float>(ub.dt * ub.dt);

	//			glm::vec3 dvec = previewpos2 - previewpos1;
	//			float distance = glm::length(dvec);
	//			float error = distance - spring.m_restlength;
	//			if (glm::abs(error) < ub.m_cloth->m_pctx.max_stretch)
	//				continue;

	//			//scale error down to match the given tolerance value
	//			error = error - (glm::sign(error) * spring.m_restlength * ub.m_cloth->m_pctx.max_stretch);

	//			allConstraintsSatisfied = false;

	//			glm::vec3 cdir = dvec / distance;

	//			float ka = p1.m_fixed ? 0.0f : (1.0f / p1.m_mass);
	//			float kb = p2.m_fixed ? 0.0f : (1.0f / p2.m_mass);

	//			float p = (error / ub.dt) / (ka + kb);

	//			glm::vec3 deltaV1 = cdir * (p / p1.m_mass);
	//			glm::vec3 deltaV2 = (-1.0f) * cdir * (p / p2.m_mass);

	//			if (!p1.m_fixed)
	//				p1.m_velocity += deltaV1;

	//			if (!p2.m_fixed)
	//				p2.m_velocity += deltaV2;

	//			/*wp2.m_position = p2.m_position;
	//			wp2.m_acceleration = p2.m_acceleration;*/
	//		}
	//	}
	//	if (allConstraintsSatisfied)
	//	{
	//		//std::cout << ci << " iterations to solve all constraints\n";
	//		break;
	//	}
	//		
	//	/*wp1.m_position = p1.m_position;
	//	wp1.m_acceleration = p1.m_acceleration;*/
	//}

	for (size_t i = ub.from; i < ub.to; i++)
	{
		Particle& p1 = preadbuf[i];//[ub.m_cloth->m_csidx[i]];
		Particle& wp1 = pwritebuf[i];//[ub.m_cloth->m_csidx[i]];
		glm::vec3 correctiveVelocity(0.0f, 0.0f, 0.0f);

		auto previewpos1 = p1.m_position + p1.m_velocity * static_cast<float>(ub.dt) + 0.5f * p1.m_acceleration * static_cast<float>(ub.dt * ub.dt);

		float totalVM = 0.0f;
		glm::vec4 cvels[MAX_SPRINGS_PER_PARTICLE]; //x, y, z velocity, w length

		for (size_t s = p1.m_springIndex; s < p1.m_springIndex + p1.m_springCount; s++)
		{
			const Spring& spring = ub.m_cloth->m_springs[s];
			Particle& p2 = preadbuf[spring.m_connectedParticleIndex];
			//Particle& wp2 = pwritebuf[spring.m_connectedParticleIndex];

			if (p1.m_fixed && p2.m_fixed)
				continue;

			//auto previewpos1 = p1.m_position + p1.m_velocity * static_cast<float>(ub.dt) + 0.5f * p1.m_acceleration * static_cast<float>(ub.dt * ub.dt);

			auto previewpos2 = p2.m_position + p2.m_velocity * static_cast<float>(ub.dt) + 0.5f * p2.m_acceleration * static_cast<float>(ub.dt * ub.dt);

			glm::vec3 dvec = previewpos2 - previewpos1;
			float distance = glm::length(dvec);
			float error = distance - spring.m_restlength;
			if (glm::abs(error) < ub.m_cloth->m_pctx.max_stretch)
				continue;

			//scale error down to match the given tolerance value
			error = error - (glm::sign(error) * spring.m_restlength * ub.m_cloth->m_pctx.max_stretch);

			glm::vec3 cdir = dvec / distance;

			float ka = p1.m_fixed ? 0.0f : (1.0f / p1.m_mass);
			float kb = p2.m_fixed ? 0.0f : (1.0f / p2.m_mass);

			float p = (error / ub.dt) / (ka + kb);

			glm::vec3 deltaV1 = cdir * (p / p1.m_mass);
			//glm::vec3 deltaV2 = (-1.0f) * cdir * (p / p2.m_mass);
			float dv1mag = glm::dot(deltaV1, deltaV1);

			if (!p1.m_fixed)
			{
				cvels[s - p1.m_springIndex] = glm::vec4(deltaV1, dv1mag);
				totalVM += dv1mag;
			}
			else
			{
				cvels[s - p1.m_springIndex] = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			}

			/*if (!p2.m_fixed)
			p2.m_velocity += deltaV2;*/

			/*wp2.m_position = p2.m_position;
			wp2.m_acceleration = p2.m_acceleration;*/
		}
		//some smal error is introduced through this crude approximation. fix this later
		if (totalVM > 1.0e-6f)
			for (size_t s = 0; s < p1.m_springCount; s++)
				correctiveVelocity += glm::vec3(cvels[s]) * (cvels[s].w / totalVM);

		wp1.m_velocity = p1.m_velocity + correctiveVelocity * (1.0f - ub.m_cloth->m_pctx.airfric);
		wp1.m_position = p1.m_position;
		wp1.m_acceleration = p1.m_acceleration;
	}



}

void PhysicsModule::updateParticleBatchPass1(ipengine::TaskContext & context) //the simulation step
{
	UpdateBatch ub = context;
	auto& preadbuf = ub.m_cloth->oldBuf();
	auto& pwritebuf = ub.m_cloth->newBuf();

	for (size_t i = ub.from; i <= ub.to; i++)
	{
		Particle& p = preadbuf[i];
		Particle& wp = pwritebuf[i];
		if (p.m_fixed)
			continue;
		wp.m_position = p.m_position + p.m_velocity * static_cast<float>(ub.dt) + 0.5f * p.m_acceleration * static_cast<float>(ub.dt * ub.dt);
		wp.m_velocity = p.m_velocity;
		wp.m_acceleration = p.m_acceleration;
	}
}

void PhysicsModule::updateParticleBatchPass2(ipengine::TaskContext & context)
{
	UpdateBatch ub = context;
	auto& preadbuf = ub.m_cloth->oldBuf();
	auto& pwritebuf = ub.m_cloth->newBuf();

	for (size_t i = ub.from; i <= ub.to; i++)
	{
		Particle& p = preadbuf[i];
		Particle& wp = pwritebuf[i];
		if (p.m_fixed)
			continue;
		glm::vec3 an1 = accumulateForces(ub.m_cloth, p) / p.m_mass;
		wp.m_velocity = p.m_velocity + ((an1 + p.m_acceleration) / 2.0f) * static_cast<float>(ub.dt);
		wp.m_acceleration = an1;
		wp.m_position = p.m_position;
	}
}

void PhysicsModule::handleCollisions(ipengine::TaskContext & context)
{
	UpdateBatch ub = context;
	auto& preadbuf = ub.m_cloth->oldBuf();
	auto& pwritebuf = ub.m_cloth->newBuf();

	//Get collider list
	//TODO: get possible colliders from scene module later. for testing we'll use a local list of thingies

	//for each collider call tryCollide and apply the correcting velocity to the particle

	for (size_t pid = ub.from; pid <= ub.to; pid++)
	{
		Particle& p = preadbuf[pid];
		Particle& wp = pwritebuf[pid];

		//TODO: get colliders from SCM
		//for (auto& bs : sphereColliders)
		//{
		//	glm::vec3 cvel = tryCollide(ub.m_cloth, p, bs, ub.dt);
		//	wp.m_velocity += cvel;
		//	wp.m_position = p.m_position;
		//	wp.m_acceleration = p.m_acceleration;
		//}

		//for (auto& bs : aabColliders)
		//{
		//	glm::vec3 cvel = tryCollide(ub.m_cloth, p, bs, ub.dt);
		//	/*if (glm::length(cvel) < 1.0e-6f)
		//	cvel = p.m_position;*/
		//	wp.m_velocity += cvel;
		//	wp.m_position = p.m_position;
		//	wp.m_acceleration = p.m_acceleration;
		//}
	}


}

glm::vec3 PhysicsModule::tryCollide(Cloth * cloth, Particle & particle, SCM::BoundingBox & collider, float dt)
{
	//do an optimistic test at the beginning. construct a sphere from the largest half-size of the box and
	//do a quick sphere-sphere intersection test. quit early if the test renders negative

	/*if (glm::length(particle.m_position - collider.m_center) > particle.m_radius + (glm::max(collider.m_size.x* 0.5f, glm::max(collider.m_size.y* 0.5f, collider.m_size.z* 0.5f)) ))
	return glm::vec3(0.0f, 0.0f, 0.0f);*/

	//Calculate planes from bounding box
	glm::mat3 rotmat = glm::mat3(collider.m_rotation);
	glm::vec3 localx(
		glm::normalize(rotmat[0])
	);

	glm::vec3 localy(
		glm::normalize(rotmat[1])
	);

	glm::vec3 localz(
		glm::normalize(rotmat[2])
	);

	//Calculate box points
	glm::vec3 boxpoints[8];

	glm::vec3 xoff = localx * (collider.m_size.x * 0.5f);
	glm::vec3 yoff = localy * (collider.m_size.y * 0.5f);
	glm::vec3 zoff = localz * (collider.m_size.z * 0.5f);

	boxpoints[0] = collider.m_center + zoff - xoff - yoff;
	boxpoints[1] = collider.m_center + zoff + xoff - yoff;
	boxpoints[2] = collider.m_center + zoff - xoff + yoff;
	boxpoints[3] = collider.m_center + zoff + xoff + yoff;
	boxpoints[4] = collider.m_center - zoff - xoff - yoff;
	boxpoints[5] = collider.m_center - zoff + xoff - yoff;
	boxpoints[6] = collider.m_center - zoff - xoff + yoff;
	boxpoints[7] = collider.m_center - zoff + xoff + yoff;


	//use sat to detect collision
	//calculate nearest point on box surface to sphere center, use spherecenter - closest point as seperating axis

	//project particle position on every axis and clamp them to box bounds
	glm::vec3 projectedPosition(
		glm::dot(particle.m_position - collider.m_center, localx),
		glm::dot(particle.m_position - collider.m_center, localy),
		glm::dot(particle.m_position - collider.m_center, localz)
	);

	//something goes wrong here
	projectedPosition.x = glm::clamp(projectedPosition.x, -collider.m_size.x * 0.5f, collider.m_size.x * 0.5f);//(collider.m_size.x * 0.5f) / glm::abs(projectedPosition.x);
	projectedPosition.y = glm::clamp(projectedPosition.y, -collider.m_size.y * 0.5f, collider.m_size.y * 0.5f);//(collider.m_size.y * 0.5f) / glm::abs(projectedPosition.y);
	projectedPosition.z = glm::clamp(projectedPosition.z, -collider.m_size.z * 0.5f, collider.m_size.z * 0.5f);//(collider.m_size.z * 0.5f) / glm::abs(projectedPosition.z);

	glm::vec3 nearestPointOnBox(
		localx * projectedPosition.x +
		localy * projectedPosition.y +
		localz * projectedPosition.z
	);

	nearestPointOnBox += collider.m_center;

	float mindistance = glm::length(nearestPointOnBox - particle.m_position);

	glm::vec3 sataxis = (nearestPointOnBox - particle.m_position) / glm::abs(mindistance);

	float minprojbox = std::numeric_limits<float>::max();
	float maxprojbox = std::numeric_limits<float>::lowest();

	float minprojparticle;
	float maxprojparticle;

	float maxproj;
	float minproj;

	for (size_t i = 0; i < 8; i++)
	{
		float proj = glm::dot(boxpoints[i], sataxis);
		minprojbox = glm::min(minprojbox, proj);
		maxprojbox = glm::max(maxprojbox, proj);
	}

	minprojparticle = glm::min(glm::dot(particle.m_position + particle.m_radius * sataxis, sataxis), glm::dot(particle.m_position - particle.m_radius * sataxis, sataxis));
	maxprojparticle = glm::max(glm::dot(particle.m_position + particle.m_radius * sataxis, sataxis), glm::dot(particle.m_position - particle.m_radius * sataxis, sataxis));

	minproj = glm::min(minprojbox, minprojparticle);
	maxproj = glm::max(maxprojbox, maxprojparticle);

	if ((maxprojbox - minprojbox) + (maxprojparticle - minprojparticle) > maxproj - minproj)
	{
		//collision!
		//calculate penetration depth
		//wrong if particle is completely inside box
		float penetrationDepth = ((maxprojbox - minprojbox) + (maxprojparticle - minprojparticle)) - (maxproj - minproj);//glm::length((particle.m_position + (sataxis * particle.m_radius)) - nearestPointOnBox);
																														 //std::cout << penetrationDepth << "\n";
		return (penetrationDepth * -sataxis) / dt;//planes[minpidx].n) / dt;
	}
	return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 PhysicsModule::tryCollide(Cloth * cloth, Particle & particle, SCM::BoundingSphere & collider, float dt)
{
	glm::vec3 psvec = particle.m_position - collider.m_center;
	float pslth = glm::length(psvec);
	if (pslth <= (particle.m_radius + collider.m_radius))
	{
		glm::vec3 collisionNormal = psvec / pslth;
		float penetrationDepth = (particle.m_radius + collider.m_radius) - pslth;
		//calculate a velocity that puishes the particle penetrationDepth in collisionNormal direction. apply some fiction thingy
		//std::cout << penetrationDepth << "\n";
		glm::vec3 cvel = (collisionNormal * penetrationDepth) / dt;
		return cvel;
	}
	return glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 PhysicsModule::accumulateForces(Cloth* cloth, Particle& particle)
{
	glm::vec3 forceaccum(0.0f);
	auto& preadbuf = cloth->oldBuf();
	for (size_t i = particle.m_springIndex; i < particle.m_springIndex + particle.m_springCount; i++)
	{
		const Spring& s = cloth->m_springs[i];
		glm::vec3 pos1 = particle.m_position;
		glm::vec3 pos2 = preadbuf[s.m_connectedParticleIndex].m_position;

		glm::vec3 er = glm::normalize(pos2 - pos1);
		float deflexion = glm::length(pos2 - pos1) - s.m_restlength;
		forceaccum += er * s.m_ks * deflexion - er * s.m_kd * glm::dot(particle.m_velocity, er);
	}
	forceaccum += externalForces(cloth, particle);

	return forceaccum;
}

glm::vec3 PhysicsModule::externalForces(Cloth * cloth, Particle & particle)
{
	glm::vec3 forceaccum = particle.m_mass * cloth->m_pctx.gravity;

	//air friction
	forceaccum += -particle.m_velocity * cloth->m_pctx.airfric;

	return forceaccum;
}
PhysicsModule::Particle & PhysicsModule::getParticle(const SCM::EntityId name, size_t  x, size_t y)
{
	// TODO: hier Rückgabeanweisung eingeben
	for (Cloth& c : clothInstances)
	{
		if (c.id == name)
		{
			return c.oldBuf()[index2D(x, y, c.m_width)];
		}
	}

}
//Update graphical representation ----------------------------------------------------------------------------------------------

void PhysicsModule::updateMesh(Cloth* cloth)
{
	auto& readbuf = cloth->oldBuf();
	auto& cent = contentmodule->getThreeDimEntities()[cloth->id];
	auto mobs = cent->m_mesheObjects;
	auto& mesh = mobs->m_meshes.front();
	//set new positions and reset normals
	for (size_t i = 0; i < cloth->particleCount(); i++)
	{
		mesh->m_vertices.setData()[i].m_position = readbuf[i].m_position;
		//cloth->m_vertices[i].m_color = glm::vec3(static_cast<float>(readbuf[i].m_fixdistance) / static_cast<float>(cloth->m_width));
		mesh->m_vertices.setData()[i].m_normal = glm::vec3(0.0f, 0.0f, 0.0f);
	}

	//sum up normals
	for (size_t i = 0; i < mesh->m_indices.size(); i += 3)
	{
		glm::vec3 v1 = mesh->m_vertices.getData()[mesh->m_indices[i]].m_position;
		glm::vec3 v2 = mesh->m_vertices.getData()[mesh->m_indices[i + 1]].m_position;
		glm::vec3 v3 = mesh->m_vertices.getData()[mesh->m_indices[i + 2]].m_position;

		//counter clockwise winding
		glm::vec3 edge1 = v2 - v1;
		glm::vec3 edge2 = v3 - v1;

		glm::vec3 normal = glm::cross(edge1, edge2);

		//for each Vertex all corresponing normals are added. The result is a non unit length vector wich is the weighted average direction of all assigned normals.
		mesh->m_vertices.setData()[mesh->m_indices[i]].m_normal += normal;
		mesh->m_vertices.setData()[mesh->m_indices[i + 1]].m_normal += normal;
		mesh->m_vertices.setData()[mesh->m_indices[i + 2]].m_normal += normal;
	}

	//normalize normals
	//Very costly on cpu. Let the vertex shader do this job!	
}

//Cloth creation/destruction ---------------------------------------------------------------------------------------------------

SCM::EntityId PhysicsModule::createCloth(const std::string &name,size_t width,
								size_t height,
								const SCM::TransformData& transform,
								const PhysicsContext & physicsContext, const SCM::IdType materialid)
{
	Cloth cloth;
	cloth.m_pctx = physicsContext;
	cloth.m_width = width;
	cloth.m_height = height;
	cloth.m_distance = physicsContext.particleDistance;
	cloth.id = contentmodule->generateNewEntityId();

	//setup buffers
	cloth.m_particles_buf1 = ipengine::alloc_aligned_array<Particle, TS_CACHE_LINE_SIZE>(width * height);
	cloth.m_particles_buf2 = ipengine::alloc_aligned_array<Particle, TS_CACHE_LINE_SIZE>(width * height);
	cloth.m_springs = ipengine::alloc_aligned_array<Spring, TS_CACHE_LINE_SIZE>(width * height * MAX_SPRINGS_PER_PARTICLE);

	//cloth.m_vertices.reserve(width * height);
	cloth.m_csidx.reserve(width * height);
	//cloth.m_indices.reserve(width * height * 2 * 3);
	std::vector<SCM::VertexData> m_vertices;
	std::vector<SCM::index> m_indices;
	glm::vec3 curpos(-(static_cast<float>(width) * cloth.m_distance) / 2.0f, -(static_cast<float>(height) * cloth.m_distance) / 2.0f, 0.0f);

	float st_s_rl = cloth.m_distance;						//structural spring rest length
	float sh_s_rl = glm::sqrt(2.0f * st_s_rl * st_s_rl);	//shear spring rest length
	float be_s_rl = 2.0f * cloth.m_distance;				//bending spring rest length
															//setup particles
	for (size_t y = 0; y < height; y++)
	{
		for (size_t x = 0; x < width; x++)
		{
			Particle p;
			//position			
			p.m_position = curpos + transform.m_location;
			curpos.x += cloth.m_distance;
			//std::cout << p.m_position.x << " " << p.m_position.y << " " << p.m_position.z << "\n";
			p.m_velocity = glm::vec3(0.0f);
			p.m_acceleration = glm::vec3(0.0f);
			p.m_vertexIndex = index2D(x, y, width);
			p.m_springIndex = index2D(x, y, width) * MAX_SPRINGS_PER_PARTICLE;
			p.m_radius = cloth.m_distance;
			p.m_fixed = false;
			p.m_mass = physicsContext.particleMass;
			p.m_mark = false;

			//springs. springs are defined in counter clockwise order
			p.m_springCount = 0;
			size_t d1, d2;
			//structural springs
			if (physicsContext.struct_springs)
			{
				if (x + 1 < width)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.struct_springKs,
						physicsContext.struct_springKd,
						st_s_rl,
						index2D(x, y, width),
						index2D(x + 1, y, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x + 1, y, width);
				}

				if (y + 1 < height)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.struct_springKs,
						physicsContext.struct_springKd,
						st_s_rl,
						index2D(x, y, width),
						index2D(x, y + 1, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x, y + 1, width);
				}

				if (static_cast<int64_t>(x) - 1 >= 0)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.struct_springKs,
						physicsContext.struct_springKd,
						st_s_rl,
						index2D(x, y, width),
						index2D(x - 1, y, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x - 1, y, width);
				}

				if (static_cast<int64_t>(y) - 1 >= 0)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.struct_springKs,
						physicsContext.struct_springKd,
						st_s_rl,
						index2D(x, y, width),
						index2D(x, y - 1, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x, y - 1, width);
				}
			}
			//shear springs
			if (physicsContext.shear_springs)
			{
				if (x + 1 < width && y + 1 < height)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.shear_springKs,
						physicsContext.shear_springKd,
						sh_s_rl,
						index2D(x, y, width),
						index2D(x + 1, y + 1, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x + 1, y + 1, width);
				}

				if (static_cast<int64_t>(x) - 1 >= 0 && y + 1 < height)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.shear_springKs,
						physicsContext.shear_springKd,
						sh_s_rl,
						index2D(x, y, width),
						index2D(x - 1, y + 1, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x - 1, y + 1, width);
				}

				if (static_cast<int64_t>(x) - 1 >= 0 && static_cast<int64_t>(y) - 1 >= 0)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.shear_springKs,
						physicsContext.shear_springKd,
						sh_s_rl,
						index2D(x, y, width),
						index2D(x - 1, y - 1, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x - 1, y - 1, width);
				}

				if (x + 1 < width && static_cast<int64_t>(y) - 1 >= 0)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.shear_springKs,
						physicsContext.shear_springKd,
						sh_s_rl,
						index2D(x, y, width),
						index2D(x + 1, y - 1, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x + 1, y - 1, width);
				}
			}
			//bending springs
			if (physicsContext.bend_springs)
			{
				if (x + 2 < width)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.bend_springKs,
						physicsContext.bend_springKd,
						be_s_rl,
						index2D(x, y, width),
						index2D(x + 2, y, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x + 2, y, width);
				}

				if (y + 2 < height)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.bend_springKs,
						physicsContext.bend_springKd,
						be_s_rl,
						index2D(x, y, width),
						index2D(x, y + 2, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x, y + 2, width);
				}

				if (static_cast<int64_t>(x) - 2 >= 0)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.bend_springKs,
						physicsContext.bend_springKd,
						be_s_rl,
						index2D(x, y, width),
						index2D(x - 2, y, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x - 2, y, width);
				}

				if (static_cast<int64_t>(y) - 2 >= 0)
				{
					cloth.m_springs[p.m_springIndex + p.m_springCount++] = Spring{
						physicsContext.bend_springKs,
						physicsContext.bend_springKd,
						be_s_rl,
						index2D(x, y, width),
						index2D(x, y - 2, width)
					};
					d1 = index2D(x, y, width);
					d2 = index2D(x, y - 2, width);
				}
			}

			//Particle is ready.
			cloth.m_particles_buf1[index2D(x, y, width)] = p;
			cloth.m_particles_buf2[index2D(x, y, width)] = p;

			//create vertex
			m_vertices.push_back(ClothVertex{
				p.m_position,
				//glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec2(static_cast<float>(x) / static_cast<float>(width - 1), static_cast<float>(y) / static_cast<float>(height - 1))
			, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3()});

			//initialize sorted particle index list
			cloth.m_csidx.push_back(index2D(x, y, width));

			if (x < width - 1 && y < width - 1)
			{
				//upper tri
				m_indices.push_back(static_cast<ClothIndex>(index2D(x, y, width)));
				size_t index = index2D(x, y, width);
				m_indices.push_back(static_cast<ClothIndex>(index2D(x + 1, y + 1, width)));
				index = index2D(x + 1, y + 1, width);
				m_indices.push_back(static_cast<ClothIndex>(index2D(x, y + 1, width)));
				index = index2D(x, y + 1, width);

				//lower tri
				m_indices.push_back(static_cast<ClothIndex>(index2D(x, y, width)));
				index = index2D(x, y, width);
				m_indices.push_back(static_cast<ClothIndex>(index2D(x + 1, y, width)));
				index = index2D(x + 1, y, width);
				m_indices.push_back(static_cast<ClothIndex>(index2D(x + 1, y + 1, width)));
				index = index2D(x + 1, y + 1, width);
			}
		}

		curpos.y += cloth.m_distance;
		curpos.x = -(static_cast<float>(width) * cloth.m_distance) / 2.0f;
	}
	cloth.m_current_old = PBUF1;
	//TODO: create new entity..
	//contentmodule.getEntities().insert(

	//setup mesh

	//push cloth
	clothInstances.push_back(std::move(cloth));

	//Crfeate SCM ENTity

	auto& entities = contentmodule->getEntities();

	auto& thrde = contentmodule->getThreeDimEntities();
	auto& meshedobjects = contentmodule->getMeshedObjects();
	auto& meshes = contentmodule->getMeshes();
	SCM::MeshData mdata;
	mdata.m_meshId = contentmodule->generateNewGeneralId();
	auto &tcloth = clothInstances.back();
	mdata.m_indices = m_indices;
	mdata.m_vertices.setData().swap(m_vertices);
	
	mdata.m_material = contentmodule->getMaterialById(materialid);

	meshes.push_back(mdata);
	meshedobjects.push_back(SCM::MeshedObject(std::vector<SCM::MeshData*>({ &meshes.back() }), contentmodule->generateNewGeneralId()));
	SCM::ThreeDimEntity* dimentity = new SCM::ThreeDimEntity(tcloth.id, SCM::Transform(transform), SCM::BoundingData(), true, false, &meshedobjects.back());
	thrde[tcloth.id] = dimentity;
	entities[name] = dimentity;
	return tcloth.id;
}

void PhysicsModule::destroyCloth(SCM::EntityId id)
{
	//search for cloth with entity id, remove the entity and then free the internal buffers
}

void PhysicsModule::fixParticle(const SCM::EntityId name, size_t x, size_t y, bool fixed)
{
	for (Cloth& c : clothInstances)
	{
		if (c.id == name)
		{
			c.oldBuf()[index2D(x, y, c.m_width)].m_fixed = fixed;
			c.newBuf()[index2D(x, y, c.m_width)].m_fixed = fixed;

			////sort particle indices by minimum distance to a fixed particle
			//std::vector<size_t> fixedParticles;

			//for (size_t i = 0; i < c.particleCount(); i++)
			//{
			//	if (c.oldBuf()[i].m_fixed)
			//		fixedParticles.push_back(i);
			//}

			//for (size_t i = 0; i < c.particleCount(); i++)
			//{
			//	size_t d = std::numeric_limits<size_t>::max();
			//	for (size_t f = 0; f < fixedParticles.size(); f++)
			//	{
			//		idx2d ip = index2D(i, c.m_width);
			//		idx2d ifx = index2D(fixedParticles[f], c.m_width);

			//		size_t td = std::max(std::abs(static_cast<int64_t>(ip.x) - static_cast<int64_t>(ifx.x)),
			//							  std::abs(static_cast<int64_t>(ip.y) - static_cast<int64_t>(ifx.y)));

			//		d = std::min(d, td);
			//	}
			//	c.oldBuf()[i].m_fixdistance = d;
			//	c.newBuf()[i].m_fixdistance = d;
			//}

			//std::sort(c.m_csidx.begin(), c.m_csidx.end(), [&](size_t a, size_t b)
			//{
			//	return c.oldBuf()[a].m_fixdistance < c.oldBuf()[b].m_fixdistance;
			//});
			//
			return;
		}
	}
}

bool PhysicsModule::isOutsidePlane(const glm::vec3 & p, const Plane & plane)
{
	return pointPlaneDistance(p, plane) > 0.0f;
}

bool PhysicsModule::intersectsPlane(const glm::vec3 & p, float radius, const Plane & plane)
{
	return glm::abs(pointPlaneDistance(p, plane)) <= radius;
}

float PhysicsModule::pointPlaneDistance(const glm::vec3& p, const Plane & plane)
{
	return glm::dot(plane.n, p - plane.p);
}


//Cloth class implementation ---------------------------------------------------------------------------------------------------

PhysicsModule::Cloth::Cloth() :
	m_width(0),
	m_height(0),
	m_distance(0.0f),
	m_particles_buf1(),
	m_particles_buf2(),
	m_springs(),
	m_pctx(),
	m_current_old(1),
	m_csidx()
{}

PhysicsModule::Cloth::Cloth(Cloth && other) :
	m_width(other.m_width),
	m_height(other.m_height),
	m_distance(other.m_distance),
	m_particles_buf1(std::move(other.m_particles_buf1)),
	m_particles_buf2(std::move(other.m_particles_buf2)),
	m_springs(std::move(other.m_springs)),
	m_pctx(std::move(other.m_pctx)),
	m_current_old(other.m_current_old),
	id(std::move(other.id)),
	m_csidx(std::move(other.m_csidx))
{
	other.m_width = 0;
	other.m_height = 0;
}

PhysicsModule::Cloth::~Cloth()
{
	ipengine::free_aligned_array(m_particles_buf1);
	ipengine::free_aligned_array(m_particles_buf2);
	ipengine::free_aligned_array(m_springs);
}

size_t PhysicsModule::Cloth::particleCount()
{
	return m_width * m_height;
}

void PhysicsModule::Cloth::swapBuffers()
{
	m_current_old = (m_current_old == PBUF1 ? PBUF2 : PBUF1);
}

ipengine::aligned_ptr<PhysicsModule::Particle>& PhysicsModule::Cloth::oldBuf()
{
	return (m_current_old == PBUF1 ? m_particles_buf1 : m_particles_buf2);
}

ipengine::aligned_ptr<PhysicsModule::Particle>& PhysicsModule::Cloth::newBuf()
{
	return (m_current_old == PBUF1 ? m_particles_buf2 : m_particles_buf1);
}

