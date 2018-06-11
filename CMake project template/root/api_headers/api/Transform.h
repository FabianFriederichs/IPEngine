#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <limits>
//#include <vector>
//#include <memory>
class Transform
{
public:
	// public interface --------------------------------------------------------------
	Transform() :
		m_local_position(0.0f, 0.0f, 0.0f),
		m_local_rotation(),
		m_local_scale(1.0f, 1.0f, 1.0f),
		m_ltp_dirty(true),
		m_parent(nullptr)
	{}

	Transform(const glm::mat4 & transformMatrix) :
		m_parent(nullptr)
	{
		setTransformMatrix(transformMatrix);
	}

	Transform(const glm::vec3 & position, const glm::quat & rotation, const glm::vec3 & scale) :
		m_local_position(position),
		m_local_rotation(rotation),
		m_local_scale(scale),
		m_ltp_dirty(true),
		m_parent(nullptr)
	{
	}

	Transform(const Transform& other) = default;
	Transform(Transform&& other) = default;
	Transform& operator=(const Transform& other) = default;
	Transform& operator=(Transform&& other) = default;

	~Transform()
	{}

	// convenience constructors

	// getters/setters
	//get position in respect to parent coordinate system
	const glm::vec3& getLocalPosition()
	{
		return m_local_position;
	}

	//get position in respect to world coordinate system
	const glm::vec3& getWorldPosition()
	{
		updateLTW();
		return m_world_position;
	}

	//get rotation in repsect to parent coordinate system
	const glm::quat& getLocalRotation()
	{
		return m_local_rotation;
	}

	//get rotation in respect to world coordinate system
	const glm::quat& getWorldRotation()
	{
		updateLTW();
		return m_world_rotation;
	}

	//get scale in respect to parent coordinate system
	const glm::vec3& getLocalScale()
	{
		return m_local_scale;
	}

	//get scale in respect to world coordinate system
	const glm::vec3& getWorldScale()
	{
		updateLTW();
		return m_world_scale;
	}

	//get matrix wich transforms from local to parent coordinate system
	const glm::mat4& getLocalToParentMatrix()
	{
		updateLTP();
		return m_local_to_parent;
	}

	//get matrix wich transforms from local to world coordinate system
	const glm::mat4& getLocalToWorldMatrix()
	{
		updateLTW();
		return m_local_to_world;
	}

	//get matrix wich transforms from parent to local coordinate system
	const glm::mat4& getParentToLocalMatrix()
	{
		updatePTL();
		return m_parent_to_local;
	}

	//get matrix wich transforms from world to local coordinate system
	const glm::mat4& getWorldToLocalMatrix()
	{
		updateWTL();
		return m_world_to_local;
	}

	//get x axis in respect to parent coordinate system
	const glm::vec3& getLocalXAxis()
	{
		updateLTP();
		return m_local_xaxis;
	}

	//get y axis in respect to parent coordinate system
	const glm::vec3& getLocalYAxis()
	{
		updateLTP();
		return m_local_yaxis;
	}

	//get z axis in respect to parent coordinate system
	const glm::vec3& getLocalZAxis()
	{
		updateLTP();
		return m_local_zaxis;
	}

	//get x axis in respect to world coordinate system
	const glm::vec3& getWorldXAxis()
	{
		updateLTW();
		return m_world_xaxis;
	}

	//get y axis in respect to world coordinate system
	const glm::vec3& getWorldYAxis()
	{
		updateLTW();
		return m_world_yaxis;
	}

	//get z axis in respect to world coordinate system
	const glm::vec3& getWorldZAxis()
	{
		updateLTW();
		return m_world_zaxis;
	}

	//setters 
	//set position in respect to parent coordinate system
	void setLocalPosition(const glm::vec3& position)
	{
		m_local_position = position;
		m_ltp_dirty = true;
	}

	//set position in respect to world coordinate system
	void setWorldPosition(const glm::vec3& position)
	{
		translateWorld(position - getWorldPosition());
		m_world_position = position;
	}

	//set rotation in respect to parent coordinate system
	void setLocalRotation(const glm::quat& rotation)
	{
		m_local_rotation = rotation;
		m_ltp_dirty = true;
	}

	//set rotation in respect to parent coordinate systen, given euler angles
	void setLocalRotationXYZ(const glm::vec3& rotation)
	{
		m_local_rotation = glm::quat(rotation);
		m_ltp_dirty = true;
	}

	//set rotation in respect to world coordinate system
	void setWorldRotation(const glm::quat& rotation)
	{
		if (m_parent)
		{
			m_local_rotation = glm::inverse(m_parent->getWorldRotation()) * rotation;
		}
		else
		{
			m_local_rotation = rotation;
		}
		m_ltp_dirty = true;
	}

	//set rotation in respect to world coordinate system, given euler angles
	void setWorldRotationXYZ(const glm::vec3& rotation)
	{
		if (m_parent)
		{
			m_local_rotation = glm::inverse(m_parent->getWorldRotation()) * glm::quat(rotation);
		}
		else
		{
			m_local_rotation = glm::quat(rotation);
		}
		m_ltp_dirty = true;
	}

	//set scale in respect to parent coordinate system
	void setLocalScale(const glm::vec3& scale)
	{
		m_local_scale = scale;
		m_ltp_dirty = true;
	}

	//set scale in respect to world coordinate system
	//void setWorldScale(const glm::vec3& scale)
	//{
	//	glm::vec3 wsc = getWorldScale();
	//	wsc = 1.0f / wsc;
	//	m_local_scale = wsc * scale;
	//	m_ltp_dirty = true;
	//}

	//setters rvalues
	//set position in respect to parent coordinate system
	void setLocalPosition(glm::vec3&& position)
	{
		m_local_position = position;
		m_ltp_dirty = true;
	}

	//set position in respect to world coordinate system
	void setWorldPosition(glm::vec3&& position)
	{
		translateWorld(position - getWorldPosition());
		m_ltp_dirty = true;
	}

	//set rotation in respect to parent coordinate system
	void setLocalRotation(glm::quat&& rotation)
	{
		m_local_rotation = rotation;
		m_ltp_dirty = true;
	}

	//set rotation in respect to parent coordinate system, given euler angles
	void setLocalRotationXYZ(glm::vec3&& rotation)
	{
		m_local_rotation = glm::quat(rotation);
		m_ltp_dirty = true;
	}

	//set rotation in respect to world coordinate system
	void setWorldRotation(glm::quat&& rotation)
	{
		if (m_parent)
		{
			m_local_rotation = glm::inverse(m_parent->getWorldRotation()) * rotation;
		}
		else
		{
			m_local_rotation = rotation;
		}
		m_ltp_dirty = true;
	}

	//set rotation in respect to world coordinate system, given euler angles
	void setWorldRotationXYZ(glm::vec3&& rotation)
	{
		if (m_parent)
		{
			m_local_rotation = glm::inverse(m_parent->getWorldRotation()) * glm::quat(rotation);
		}
		else
		{
			m_local_rotation = glm::quat(rotation);
		}
		m_ltp_dirty = true;
	}

	//set scale in respect to parent coordinate system
	void setLocalScale(glm::vec3&& scale)
	{
		m_local_scale = scale;
		m_ltp_dirty = true;
	}

	//set scale in respect to world coordinate system
	//void setWorldScale(glm::vec3&& scale)
	//{
	//	glm::vec3 wsc = getWorldScale();
	//	wsc = 1.0f / wsc;
	//	m_local_scale = wsc * scale;
	//	m_ltp_dirty = true;
	//}

	// manipulators
	//translate in respect to local coordinate system
	void translateLocal(const glm::vec3& offset)
	{
		m_local_position += getLocalXAxis() * offset.x + getLocalYAxis() * offset.y + getLocalZAxis() * offset.z;
		m_ltp_dirty = true;
	}

	//translate in respect to parent coordinate system
	void translate(const glm::vec3& offset)
	{
		m_local_position += offset;
		m_ltp_dirty = true;
	}

	//translate in respect to world coordinate system
	void translateWorld(const glm::vec3& offset)
	{
		if (m_parent)
		{
			auto& s = m_parent->getWorldScale();
			auto& rx = m_parent->getWorldXAxis();
			auto& ry = m_parent->getWorldYAxis();
			auto& rz = m_parent->getWorldZAxis();

			m_local_position += glm::vec3(
				1.0f / s.x * (glm::dot(rx, offset)),
				1.0f / s.y * (glm::dot(ry, offset)),
				1.0f / s.z * (glm::dot(rz, offset))
			);
		}
		else
		{
			m_local_position += offset;
		}

		m_ltp_dirty = true;
	}

	//rotate in respect to local coordinate system
	void rotateLocal(const glm::quat& rotation)
	{
		m_local_rotation = glm::normalize(getLocalRotation() * rotation);
		m_ltp_dirty = true;
	}

	//rotate in respect to parent coordinate system
	void rotate(const glm::quat& rotation)
	{
		m_local_rotation = glm::normalize(rotation * getLocalRotation());
		m_ltp_dirty = true;
	}

	//rotate in respect to world coordinate system
	void rotateWorld(const glm::quat& rotation)
	{
		if (m_parent)
		{
			m_local_rotation = glm::normalize(glm::inverse(m_parent->getWorldRotation()) * rotation * m_parent->getWorldRotation()) * getLocalRotation();
		}
		else
		{
			m_local_rotation = glm::normalize(rotation * getLocalRotation());
		}
		m_ltp_dirty = true;
	}

	//rotate in respect to local coordinate system
	void rotateLocalAroundPoint(const glm::quat& rotation, const glm::vec3& point)
	{
		m_local_rotation = glm::normalize(m_local_rotation * rotation);
		glm::vec3 wpo = point - getWorldPosition();
		glm::mat3 invrotmat = glm::transpose(glm::toMat3(rotation));
		translateWorld(glm::vec3(
			glm::dot(invrotmat[0], -wpo) + wpo.x,
			glm::dot(invrotmat[1], -wpo) + wpo.y,
			glm::dot(invrotmat[2], -wpo) + wpo.z
		));
		m_ltp_dirty = true;
	}

	//rotate in respect to parent coordinate system
	void rotateAroundPoint(const glm::quat& rotation, const glm::vec3& point)
	{
		m_local_rotation = glm::normalize(rotation * m_local_rotation);
		glm::vec3 wpo = point - getWorldPosition();
		glm::mat3 invrotmat = glm::transpose(glm::toMat3(rotation));
		translateWorld(glm::vec3(
			glm::dot(invrotmat[0], -wpo) + wpo.x,
			glm::dot(invrotmat[1], -wpo) + wpo.y,
			glm::dot(invrotmat[2], -wpo) + wpo.z
		));
		m_ltp_dirty = true;
	}

	//rotate in respect to world coordinate system
	void rotateWorldAroundPoint(const glm::quat& rotation, const glm::vec3& point)
	{
		if (m_parent)
		{
			m_local_rotation = glm::normalize(glm::inverse(m_parent->getWorldRotation()) * rotation * m_parent->getWorldRotation()) * getLocalRotation();
		}
		else
		{
			m_local_rotation = glm::normalize(rotation * getLocalRotation());
		}
		glm::vec3 wpo = point - getWorldPosition();
		glm::mat3 invrotmat = glm::transpose(glm::toMat3(rotation));
		translateWorld(glm::vec3(
			glm::dot(invrotmat[0], -wpo) + wpo.x,
			glm::dot(invrotmat[1], -wpo) + wpo.y,
			glm::dot(invrotmat[2], -wpo) + wpo.z
		));
		m_ltp_dirty = true;
	}

	//scale in respect to local coordinate system
	void scaleLocal(const glm::vec3& scale)
	{
		m_local_scale *= scale;
		m_ltp_dirty = true;
	}

	//scale in respect to parent coordinate system
	//this doesn't work at the moment. Need to figure out whats wrong with non uniform scale.
	//void scale(const glm::vec3& scale)
	//{
	//	glm::mat4 mat = glm::translate(getLocalPosition()) * glm::scale(scale) * glm::toMat4(getLocalRotation()) * glm::scale(getLocalScale());
	//	setTransformMatrix(mat);
	//}
	//
	////scale in respect to world coordinate system
	//void scaleWorld(const glm::vec3& scale)
	//{
	//	glm::mat4 mat = glm::inverse(glm::scale(getWorldScale())) * glm::toMat4(glm::inverse(getWorldRotation())) * glm::scale(scale) * glm::toMat4(getWorldRotation()) * glm::scale(getWorldScale());
	//	rightApplyTransformMatrix(mat);
	//}
	//
	////scale in respect to local coordinate system
	//void scaleLocalAroundPoint(const glm::vec3& scale, const glm::vec3& point)
	//{}
	//
	////scale in respect to parent coordinate system
	//void scaleAroundPoint(const glm::vec3& scale, const glm::vec3& point)
	//{}
	//
	////scale in respect to world coordinate system
	//void scaleWorldAroundPoint(const glm::vec3& scale, const glm::vec3& point)
	//{}

	//manipulations via matrices (ltp only)

	void setTransformMatrix(const glm::mat4& matrix)
	{
		m_local_to_parent = matrix;
		updateLocalComponents(matrix);
		m_ltp_dirty = false;
		m_ltw_dirty = true;
		m_ptl_dirty = true;
		m_wtl_dirty = true;
	}

	void rightApplyTransformMatrix(const glm::mat4& matrix)
	{
		m_local_to_parent = getLocalToParentMatrix() * matrix;
		updateLocalComponents(m_local_to_parent);
		m_ltp_dirty = false;
		m_ltw_dirty = true;
		m_ptl_dirty = true;
		m_wtl_dirty = true;
	}

	void leftApplyTransformMatrix(const glm::mat4& matrix)
	{
		m_local_to_parent = matrix * getLocalToParentMatrix();
		updateLocalComponents(m_local_to_parent);
		m_ltp_dirty = false;
		m_ltw_dirty = true;
		m_ptl_dirty = true;
		m_wtl_dirty = true;
	}

	void reset()
	{
		m_local_to_parent = glm::mat4(1.0f);
		updateLocalComponents(m_local_to_parent);
		m_ltp_dirty = false;
		m_ltw_dirty = true;
		m_ptl_dirty = true;
		m_wtl_dirty = true;
	}

	//parent child stuff
	Transform* getParent()
	{
		return m_parent;
	}

	void setParent(Transform* parent)
	{
		if (parent)
		{
			m_parent = parent;
			leftApplyTransformMatrix(parent->getWorldToLocalMatrix());
		}
		else
		{
			orphane();
		}
	}

	void orphane()
	{
		if (m_parent)
		{
			leftApplyTransformMatrix(m_parent->getLocalToWorldMatrix());
			m_parent = nullptr;
		}
	}

	//const std::vector<Transform*>& getChilds()
	//{
	//	return m_childs;
	//}

	//void addChild(Transform* child)
	//{
	//
	//}
	//
	//void removeChild(Transform* child)
	//{
	//
	//}
	//
	//void clearChilds()
	//{
	//
	//}

	glm::vec3 getDirection()
	{
		return -getWorldZAxis();
	}

	void lookinto(const glm::vec3 & direction)
	{
		glm::vec3 ndir(glm::normalize(-direction));
		glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), ndir));
		glm::vec3 up = glm::normalize(glm::cross(ndir, right));

		//construct a 3x3 rotation matrix from direction and global up vector
		glm::mat3 rot(right, up, ndir);

		setWorldRotation(glm::quat_cast(rot));
	}

	// operators
private:
	// private member functions -------------------------------------------------------
	//extracts local components from matrix
	void updateLocalComponents(const glm::mat4& matrix)
	{
		auto ax = glm::vec3(matrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		auto ay = glm::vec3(matrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
		auto az = glm::vec3(matrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
		auto pos = glm::vec3(matrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		float xl = glm::length(ax);
		float yl = glm::length(ay);
		float zl = glm::length(az);

		m_local_xaxis = ax / xl;
		m_local_yaxis = ay / yl;
		m_local_zaxis = az / zl;

		m_local_position = pos;
		m_local_rotation = glm::toQuat(glm::mat3(
			m_local_xaxis,
			m_local_yaxis,
			m_local_zaxis
		));
		m_local_scale = glm::vec3(xl, yl, zl);
	}

	bool isLTPDirty()
	{
		return m_ltp_dirty;
	}

	bool isLTWDirty()
	{
		return m_ltw_dirty || isLTPDirty() || m_parent;//(m_parent ? m_parent->isLTWDirty() : false);
	}

	bool isPTLDirty()
	{
		return m_ptl_dirty || isLTPDirty();
	}

	bool isWTLDirty()
	{
		return m_wtl_dirty || isLTWDirty();
	}

	void updateLTP()
	{
		if (isLTPDirty())
		{
			m_local_to_parent = glm::translate(m_local_position) * glm::toMat4(m_local_rotation) * glm::scale(m_local_scale);
			m_local_xaxis = glm::normalize(glm::vec3(m_local_to_parent[0]));
			m_local_yaxis = glm::normalize(glm::vec3(m_local_to_parent[1]));
			m_local_zaxis = glm::normalize(glm::vec3(m_local_to_parent[2]));
			m_ltp_dirty = false;
		}
	}

	void updateLTW()
	{
		if (isLTWDirty())
		{
			if (m_parent)
			{
				m_local_to_world = m_parent->getLocalToWorldMatrix() * getLocalToParentMatrix();
			}
			else
			{
				m_local_to_world = getLocalToParentMatrix();
			}

			auto ax = glm::vec3(m_local_to_world * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
			auto ay = glm::vec3(m_local_to_world * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			auto az = glm::vec3(m_local_to_world * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
			auto pos = glm::vec3(m_local_to_world * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

			float xl = glm::length(ax);
			float yl = glm::length(ay);
			float zl = glm::length(az);

			m_world_xaxis = ax / xl;
			m_world_yaxis = ay / yl;
			m_world_zaxis = az / zl;

			m_world_position = pos;
			m_world_rotation = glm::toQuat(glm::mat3(
				m_world_xaxis,
				m_world_yaxis,
				m_world_zaxis
			));
			m_world_scale = glm::vec3(xl, yl, zl);
			m_ltw_dirty = false;
		}
	}

	void updateWTL()
	{
		if (isWTLDirty())
		{
			m_world_to_local = glm::inverse(getLocalToWorldMatrix());
			m_wtl_dirty = false;
		}
	}

	void updatePTL()
	{
		if (isPTLDirty())
		{
			m_parent_to_local = glm::inverse(getLocalToParentMatrix());
			m_ptl_dirty = false;
		}
	}
private:
	// private data ------------------------------------------------------------------
	// transformation data in respect to parent coordinate system
	glm::vec3 m_local_position;
	glm::quat m_local_rotation;
	glm::vec3 m_local_scale;

	glm::vec3 m_local_xaxis;
	glm::vec3 m_local_yaxis;
	glm::vec3 m_local_zaxis;

	// transformation data in respect to world coordinate system
	glm::vec3 m_world_position;
	glm::quat m_world_rotation;
	glm::vec3 m_world_scale;

	glm::vec3 m_world_xaxis;
	glm::vec3 m_world_yaxis;
	glm::vec3 m_world_zaxis;

	// matrices
	glm::mat4 m_local_to_parent;
	glm::mat4 m_local_to_world;

	glm::mat4 m_world_to_local;
	glm::mat4 m_parent_to_local;

	bool m_ltp_dirty;
	bool m_ltw_dirty;
	bool m_ptl_dirty;
	bool m_wtl_dirty;

	// parent child
	Transform* m_parent;
	//std::vector<Transform*> m_childs;
};

#endif