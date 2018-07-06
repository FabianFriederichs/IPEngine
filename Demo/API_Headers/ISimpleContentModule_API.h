#ifndef _ISIMPLECONTENTMODULE_API_H_
#define _ISIMPLECONTENTMODULE_API_H_
#include <IModule_API.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <type_traits>


namespace SCM
{
	class TextureData;
	using index = uint32_t;
	using TextureMap = std::unordered_map<std::string, TextureData>;
	
	class TransformData
	{
		friend class Transform;
	private:
		TransformData() :
			m_local_position(0.0f, 0.0f, 0.0f),
			m_local_rotation(),
			m_local_scale(1.0f, 1.0f, 1.0f),
			m_ltp_dirty(true)
		{}

		TransformData(const glm::vec3 & position, const glm::quat & rotation, const glm::vec3 & scale) :
			m_local_position(position),
			m_local_rotation(rotation),
			m_local_scale(scale),
			m_ltp_dirty(true)
		{
		}

		TransformData(const TransformData& other) = default;
		//{
		//	//TODO: fix this
		//}
		TransformData& operator=(const TransformData& other) = default;
		//{
		//	//TODO: and that
		//}
		

		// private data ------------------------------------------------------------------
		// transformation data with respect to parent coordinate system
		glm::vec3 m_local_position;
		glm::quat m_local_rotation;
		glm::vec3 m_local_scale;

		glm::vec3 m_local_xaxis;
		glm::vec3 m_local_yaxis;
		glm::vec3 m_local_zaxis;

		// transformation data with respect to world coordinate system
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
	};

	class Transform
	{
	private:
		Transform* m_parent;
		TransformData* m_back;
		TransformData* m_front;

		//check if i can make that thing a const function
		TransformData& rtd()// const
		{
			//for now, only work with the backbuffer as swapping is not realy possible at the moment and not necessary for the demo, apparently.
			//assert(m_front && "Transform front buffer is nullptr.");
			//return *m_front;
			assert(m_back && "Transform back buffer is nullptr.");
			return *m_back;
		}

		TransformData& wtd()
		{
			assert(m_back && "Transform back buffer is nullptr.");
			return *m_back;
		}

	public:
		void swap()
		{
			//do nothing for now
		}

		// public interface --------------------------------------------------------------
		Transform() :
			m_back(new TransformData()),
			m_front(new TransformData()),
			m_parent(nullptr)
		{}

		Transform(const glm::mat4 & transformMatrix) :
			m_parent(nullptr)
		{
			setTransformMatrix(transformMatrix);
		}

		Transform(const glm::vec3 & position, const glm::quat & rotation, const glm::vec3 & scale) :
			m_parent(nullptr),
			m_back(new TransformData(position, rotation, scale)),
			m_front(new TransformData(position, rotation, scale))
		{
		}

		Transform(const Transform& other) :
			m_back(new TransformData(*other.m_back)),
			m_front(new TransformData(*other.m_front)),
			m_parent(other.m_parent)
		{			
		}

		Transform(Transform&& other) :
			m_back(other.m_back),
			m_front(other.m_front),
			m_parent(other.m_parent)

		{
			other.m_back = nullptr;
			other.m_front = nullptr;
			other.m_parent = nullptr;
		}


		Transform& operator=(const Transform& other)
		{
			if (this == &other)
				return *this;

			if (m_back)
				*m_back = *other.m_back;
			else
				m_back = new TransformData(*other.m_back);

			if (m_front)
				*m_front = *other.m_front;
			else
				m_front = new TransformData(*other.m_front);

			m_parent = other.m_parent;
			
			return *this;
		}

		Transform& operator=(Transform&& other)
		{
			if (this == &other)
				return *this;

			if (m_back)
				delete m_back;

			if (m_front)
				delete m_front;

			m_back = other.m_back;
			m_front = other.m_front;
			other.m_back = nullptr;
			other.m_front = nullptr;

			m_parent = other.m_parent;
			other.m_parent = nullptr;

			return *this;
		}

		~Transform()
		{
			if (m_back)
				delete m_back;
			if (m_front)
				delete m_front;
		}

		// convenience constructors

		// getters/setters
		//get position with respect to parent coordinate system
		const glm::vec3& getLocalPosition()
		{
			return rtd().m_local_position;
		}

		//get position with respect to world coordinate system
		const glm::vec3& getWorldPosition()
		{
			updateLTW();
			return rtd().m_world_position;
		}

		//get rotation in repsect to parent coordinate system
		const glm::quat& getLocalRotation()
		{
			return rtd().m_local_rotation;
		}

		//get rotation with respect to world coordinate system
		const glm::quat& getWorldRotation()
		{
			updateLTW();
			return rtd().m_world_rotation;
		}

		//get scale with respect to parent coordinate system
		const glm::vec3& getLocalScale()
		{
			return rtd().m_local_scale;
		}

		//get scale with respect to world coordinate system
		const glm::vec3& getWorldScale()
		{
			updateLTW();
			return rtd().m_world_scale;
		}

		//get matrix wich transforms from local to parent coordinate system
		const glm::mat4& getLocalToParentMatrix()
		{
			updateLTP();
			return rtd().m_local_to_parent;
		}

		//get matrix wich transforms from local to world coordinate system
		const glm::mat4& getLocalToWorldMatrix()
		{
			updateLTW();
			return rtd().m_local_to_world;
		}

		//get matrix wich transforms from parent to local coordinate system
		const glm::mat4& getParentToLocalMatrix()
		{
			updatePTL();
			return rtd().m_parent_to_local;
		}

		//get matrix wich transforms from world to local coordinate system
		const glm::mat4& getWorldToLocalMatrix()
		{
			updateWTL();
			return rtd().m_world_to_local;
		}

		//get x axis with respect to parent coordinate system
		const glm::vec3& getLocalXAxis()
		{
			updateLTP();
			return rtd().m_local_xaxis;
		}

		//get y axis with respect to parent coordinate system
		const glm::vec3& getLocalYAxis()
		{
			updateLTP();
			return rtd().m_local_yaxis;
		}

		//get z axis with respect to parent coordinate system
		const glm::vec3& getLocalZAxis()
		{
			updateLTP();
			return rtd().m_local_zaxis;
		}

		//get x axis with respect to world coordinate system
		const glm::vec3& getWorldXAxis()
		{
			updateLTW();
			return rtd().m_world_xaxis;
		}

		//get y axis with respect to world coordinate system
		const glm::vec3& getWorldYAxis()
		{
			updateLTW();
			return rtd().m_world_yaxis;
		}

		//get z axis with respect to world coordinate system
		const glm::vec3& getWorldZAxis()
		{
			updateLTW();
			return rtd().m_world_zaxis;
		}

		//setters 
		//set position with respect to parent coordinate system
		void setLocalPosition(const glm::vec3& position)
		{
			wtd().m_local_position = position;
			wtd().m_ltp_dirty = true;
		}

		//set position with respect to world coordinate system
		void setWorldPosition(const glm::vec3& position)
		{
			translateWorld(position - getWorldPosition());
			wtd().m_world_position = position;
		}

		//set rotation with respect to parent coordinate system
		void setLocalRotation(const glm::quat& rotation)
		{
			wtd().m_local_rotation = rotation;
			wtd().m_ltp_dirty = true;
		}

		//set rotation with respect to parent coordinate systen, given euler angles
		void setLocalRotationXYZ(const glm::vec3& rotation)
		{
			wtd().m_local_rotation = glm::quat(rotation);
			wtd().m_ltp_dirty = true;
		}

		//set rotation with respect to world coordinate system
		void setWorldRotation(const glm::quat& rotation)
		{
			if (m_parent)
			{
				wtd().m_local_rotation = glm::inverse(m_parent->getWorldRotation()) * rotation;
			}
			else
			{
				wtd().m_local_rotation = rotation;
			}
			wtd().m_ltp_dirty = true;
		}

		//set rotation with respect to world coordinate system, given euler angles
		void setWorldRotationXYZ(const glm::vec3& rotation)
		{
			if (m_parent)
			{
				wtd().m_local_rotation = glm::inverse(m_parent->getWorldRotation()) * glm::quat(rotation);
			}
			else
			{
				wtd().m_local_rotation = glm::quat(rotation);
			}
			wtd().m_ltp_dirty = true;
		}

		//set scale with respect to parent coordinate system
		void setLocalScale(const glm::vec3& scale)
		{
			wtd().m_local_scale = scale;
			wtd().m_ltp_dirty = true;
		}

		//set scale with respect to world coordinate system
		//void setWorldScale(const glm::vec3& scale)
		//{
		//	glm::vec3 wsc = getWorldScale();
		//	wsc = 1.0f / wsc;
		//	m_local_scale = wsc * scale;
		//	m_ltp_dirty = true;
		//}

		//setters rvalues
		//set position with respect to parent coordinate system
		void setLocalPosition(glm::vec3&& position)
		{
			wtd().m_local_position = position;
			wtd().m_ltp_dirty = true;
		}

		//set position with respect to world coordinate system
		void setWorldPosition(glm::vec3&& position)
		{
			translateWorld(position - getWorldPosition());
			wtd().m_ltp_dirty = true;
		}

		//set rotation with respect to parent coordinate system
		void setLocalRotation(glm::quat&& rotation)
		{
			wtd().m_local_rotation = rotation;
			wtd().m_ltp_dirty = true;
		}

		//set rotation with respect to parent coordinate system, given euler angles
		void setLocalRotationXYZ(glm::vec3&& rotation)
		{
			wtd().m_local_rotation = glm::quat(rotation);
			wtd().m_ltp_dirty = true;
		}

		//set rotation with respect to world coordinate system
		void setWorldRotation(glm::quat&& rotation)
		{
			if (m_parent)
			{
				wtd().m_local_rotation = glm::inverse(m_parent->getWorldRotation()) * rotation;
			}
			else
			{
				wtd().m_local_rotation = rotation;
			}
			wtd().m_ltp_dirty = true;
		}

		//set rotation with respect to world coordinate system, given euler angles
		void setWorldRotationXYZ(glm::vec3&& rotation)
		{
			if (m_parent)
			{
				wtd().m_local_rotation = glm::inverse(m_parent->getWorldRotation()) * glm::quat(rotation);
			}
			else
			{
				wtd().m_local_rotation = glm::quat(rotation);
			}
			wtd().m_ltp_dirty = true;
		}

		//set scale with respect to parent coordinate system
		void setLocalScale(glm::vec3&& scale)
		{
			wtd().m_local_scale = scale;
			wtd().m_ltp_dirty = true;
		}

		//set scale with respect to world coordinate system
		//void setWorldScale(glm::vec3&& scale)
		//{
		//	glm::vec3 wsc = getWorldScale();
		//	wsc = 1.0f / wsc;
		//	m_local_scale = wsc * scale;
		//	m_ltp_dirty = true;
		//}

		// manipulators
		//translate with respect to local coordinate system
		void translateLocal(const glm::vec3& offset)
		{
			wtd().m_local_position += getLocalXAxis() * offset.x + getLocalYAxis() * offset.y + getLocalZAxis() * offset.z;
			wtd().m_ltp_dirty = true;
		}

		//translate with respect to parent coordinate system
		void translate(const glm::vec3& offset)
		{
			wtd().m_local_position += offset;
			wtd().m_ltp_dirty = true;
		}

		//translate with respect to world coordinate system
		void translateWorld(const glm::vec3& offset)
		{
			if (m_parent)
			{
				auto& s = m_parent->getWorldScale();
				auto& rx = m_parent->getWorldXAxis();
				auto& ry = m_parent->getWorldYAxis();
				auto& rz = m_parent->getWorldZAxis();

				wtd().m_local_position += glm::vec3(
					1.0f / s.x * (glm::dot(rx, offset)),
					1.0f / s.y * (glm::dot(ry, offset)),
					1.0f / s.z * (glm::dot(rz, offset))
				);
			}
			else
			{
				wtd().m_local_position += offset;
			}

			wtd().m_ltp_dirty = true;
		}

		//rotate with respect to local coordinate system
		void rotateLocal(const glm::quat& rotation)
		{
			wtd().m_local_rotation = glm::normalize(getLocalRotation() * rotation);
			wtd().m_ltp_dirty = true;
		}

		//rotate with respect to parent coordinate system
		void rotate(const glm::quat& rotation)
		{
			wtd().m_local_rotation = glm::normalize(rotation * getLocalRotation());
			wtd().m_ltp_dirty = true;
		}

		//rotate with respect to world coordinate system
		void rotateWorld(const glm::quat& rotation)
		{
			if (m_parent)
			{
				wtd().m_local_rotation = glm::normalize(glm::inverse(m_parent->getWorldRotation()) * rotation * m_parent->getWorldRotation()) * getLocalRotation();
			}
			else
			{
				wtd().m_local_rotation = glm::normalize(rotation * getLocalRotation());
			}
			wtd().m_ltp_dirty = true;
		}

		//rotate with respect to local coordinate system
		void rotateLocalAroundPoint(const glm::quat& rotation, const glm::vec3& point)
		{
			wtd().m_local_rotation = glm::normalize(getLocalRotation() * rotation);
			glm::vec3 wpo = point - getWorldPosition();
			glm::mat3 invrotmat = glm::transpose(glm::toMat3(rotation));
			translateWorld(glm::vec3(
				glm::dot(invrotmat[0], -wpo) + wpo.x,
				glm::dot(invrotmat[1], -wpo) + wpo.y,
				glm::dot(invrotmat[2], -wpo) + wpo.z
			));
			wtd().m_ltp_dirty = true;
		}

		//rotate with respect to parent coordinate system
		void rotateAroundPoint(const glm::quat& rotation, const glm::vec3& point)
		{
			wtd().m_local_rotation = glm::normalize(rotation * getLocalRotation());
			glm::vec3 wpo = point - getWorldPosition();
			glm::mat3 invrotmat = glm::transpose(glm::toMat3(rotation));
			translateWorld(glm::vec3(
				glm::dot(invrotmat[0], -wpo) + wpo.x,
				glm::dot(invrotmat[1], -wpo) + wpo.y,
				glm::dot(invrotmat[2], -wpo) + wpo.z
			));
			wtd().m_ltp_dirty = true;
		}

		//rotate with respect to world coordinate system
		void rotateWorldAroundPoint(const glm::quat& rotation, const glm::vec3& point)
		{
			if (m_parent)
			{
				wtd().m_local_rotation = glm::normalize(glm::inverse(m_parent->getWorldRotation()) * rotation * m_parent->getWorldRotation()) * getLocalRotation();
			}
			else
			{
				wtd().m_local_rotation = glm::normalize(rotation * getLocalRotation());
			}
			glm::vec3 wpo = point - getWorldPosition();
			glm::mat3 invrotmat = glm::transpose(glm::toMat3(rotation));
			translateWorld(glm::vec3(
				glm::dot(invrotmat[0], -wpo) + wpo.x,
				glm::dot(invrotmat[1], -wpo) + wpo.y,
				glm::dot(invrotmat[2], -wpo) + wpo.z
			));
			wtd().m_ltp_dirty = true;
		}

		//scale with respect to local coordinate system
		void scaleLocal(const glm::vec3& scale)
		{
			wtd().m_local_scale *= scale;
			wtd().m_ltp_dirty = true;
		}

		//scale with respect to parent coordinate system
		//this doesn't work at the moment. Need to figure out whats wrong with non uniform scale.
		//void scale(const glm::vec3& scale)
		//{
		//	glm::mat4 mat = glm::translate(getLocalPosition()) * glm::scale(scale) * glm::toMat4(getLocalRotation()) * glm::scale(getLocalScale());
		//	setTransformMatrix(mat);
		//}
		//
		////scale with respect to world coordinate system
		//void scaleWorld(const glm::vec3& scale)
		//{
		//	glm::mat4 mat = glm::inverse(glm::scale(getWorldScale())) * glm::toMat4(glm::inverse(getWorldRotation())) * glm::scale(scale) * glm::toMat4(getWorldRotation()) * glm::scale(getWorldScale());
		//	rightApplyTransformMatrix(mat);
		//}
		//
		////scale with respect to local coordinate system
		//void scaleLocalAroundPoint(const glm::vec3& scale, const glm::vec3& point)
		//{}
		//
		////scale with respect to parent coordinate system
		//void scaleAroundPoint(const glm::vec3& scale, const glm::vec3& point)
		//{}
		//
		////scale with respect to world coordinate system
		//void scaleWorldAroundPoint(const glm::vec3& scale, const glm::vec3& point)
		//{}

		//manipulations via matrices (ltp only)

		void setTransformMatrix(const glm::mat4& matrix)
		{
			wtd().m_local_to_parent = matrix;
			updateLocalComponents(matrix);
			wtd().m_ltp_dirty = false;
			wtd().m_ltw_dirty = true;
			wtd().m_ptl_dirty = true;
			wtd().m_wtl_dirty = true;
		}

		void rightApplyTransformMatrix(const glm::mat4& matrix)
		{
			wtd().m_local_to_parent = getLocalToParentMatrix() * matrix;
			updateLocalComponents(wtd().m_local_to_parent);
			wtd().m_ltp_dirty = false;
			wtd().m_ltw_dirty = true;
			wtd().m_ptl_dirty = true;
			wtd().m_wtl_dirty = true;
		}

		void leftApplyTransformMatrix(const glm::mat4& matrix)
		{
			wtd().m_local_to_parent = matrix * getLocalToParentMatrix();
			updateLocalComponents(wtd().m_local_to_parent);
			wtd().m_ltp_dirty = false;
			wtd().m_ltw_dirty = true;
			wtd().m_ptl_dirty = true;
			wtd().m_wtl_dirty = true;
		}

		void reset()
		{
			wtd().m_local_to_parent = glm::mat4(1.0f);
			updateLocalComponents(wtd().m_local_to_parent);
			wtd().m_ltp_dirty = false;
			wtd().m_ltw_dirty = true;
			wtd().m_ptl_dirty = true;
			wtd().m_wtl_dirty = true;
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

			wtd().m_local_xaxis = ax / xl;
			wtd().m_local_yaxis = ay / yl;
			wtd().m_local_zaxis = az / zl;

			wtd().m_local_position = pos;
			wtd().m_local_rotation = glm::toQuat(glm::mat3(
				wtd().m_local_xaxis,
				wtd().m_local_yaxis,
				wtd().m_local_zaxis
			));
			wtd().m_local_scale = glm::vec3(xl, yl, zl);
		}

		bool isLTPDirty()
		{
			return rtd().m_ltp_dirty;
		}

		bool isLTWDirty()
		{
			return rtd().m_ltw_dirty || isLTPDirty() || m_parent;//(m_parent ? m_parent->isLTWDirty() : false);
		}

		bool isPTLDirty()
		{
			return rtd().m_ptl_dirty || isLTPDirty();
		}

		bool isWTLDirty()
		{
			return rtd().m_wtl_dirty || isLTWDirty();
		}

		void updateLTP()
		{
			if (isLTPDirty())
			{
				wtd().m_local_to_parent = glm::translate(rtd().m_local_position) * glm::toMat4(rtd().m_local_rotation) * glm::scale(rtd().m_local_scale);
				wtd().m_local_xaxis = glm::normalize(glm::vec3(rtd().m_local_to_parent[0]));
				wtd().m_local_yaxis = glm::normalize(glm::vec3(rtd().m_local_to_parent[1]));
				wtd().m_local_zaxis = glm::normalize(glm::vec3(rtd().m_local_to_parent[2]));
				wtd().m_ltp_dirty = false;
				wtd().m_ltw_dirty = true;
				wtd().m_ptl_dirty = true;
				wtd().m_wtl_dirty = true;
			}
		}

		void updateLTW()
		{
			if (isLTWDirty())
			{
				if (m_parent)
				{
					wtd().m_local_to_world = m_parent->getLocalToWorldMatrix() * getLocalToParentMatrix();
				}
				else
				{
					wtd().m_local_to_world = getLocalToParentMatrix();
				}

				auto ax = glm::vec3(wtd().m_local_to_world * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
				auto ay = glm::vec3(wtd().m_local_to_world * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
				auto az = glm::vec3(wtd().m_local_to_world * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
				auto pos = glm::vec3(wtd().m_local_to_world * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

				float xl = glm::length(ax);
				float yl = glm::length(ay);
				float zl = glm::length(az);

				wtd().m_world_xaxis = ax / xl;
				wtd().m_world_yaxis = ay / yl;
				wtd().m_world_zaxis = az / zl;

				wtd().m_world_position = pos;
				wtd().m_world_rotation = glm::toQuat(glm::mat3(
					wtd().m_world_xaxis,
					wtd().m_world_yaxis,
					wtd().m_world_zaxis
				));
				wtd().m_world_scale = glm::vec3(xl, yl, zl);
				wtd().m_ltw_dirty = false;
				wtd().m_wtl_dirty = true;
			}
		}

		void updateWTL()
		{
			if (isWTLDirty())
			{
				wtd().m_world_to_local = glm::inverse(getLocalToWorldMatrix());
				wtd().m_wtl_dirty = false;
			}
		}

		void updatePTL()
		{
			if (isPTLDirty())
			{
				wtd().m_parent_to_local = glm::inverse(getLocalToParentMatrix());
				wtd().m_ptl_dirty = false;
			}
		}

	};

	class BoundingBox
	{
	public:
		glm::quat m_rotation = glm::quat();
		glm::vec3 m_center = glm::vec3(0.0f);
		glm::vec3 m_size = { 0.f,0.f,0.f };
		glm::mat4 bdtoworld = glm::mat4(1.0f);
		glm::vec3 m_velocity = glm::vec3(0.0f);
	};

	class BoundingSphere
	{
	public:
		glm::vec3 m_center = glm::vec3(0.0f);
		glm::float32 m_radius = 0.0f;
		glm::mat4 bdtoworld = glm::mat4(1.0f);
		glm::vec3 m_velocity = glm::vec3(0.0f);
	};

	union BoundingData
	{
		BoundingData()
		{
			box = BoundingBox();
		}
		BoundingData(BoundingBox b) :box(b) {}
		BoundingData(BoundingSphere s) : sphere(s) {}
		BoundingBox box;
		BoundingSphere sphere;		
	};

	class ShaderData
	{
	public:
		ShaderData() = default;
		ShaderData(ipengine::ipid id, std::string v, std::string f):m_shaderId(id), m_shaderFiles({v,f})
		{

		}
		std::vector<std::string> m_shaderFiles;
		ipengine::ipid m_shaderId;
		//Uniform descriptors maybe?
	};

	class MaterialData
	{
	public:
		
		MaterialData() = default;
		MaterialData(ipengine::ipid id, ipengine::ipid shaderid, const TextureMap &textures) : m_materialId(id), m_shaderId(shaderid), m_textures(textures) {};
		SCM::TextureMap m_textures; //textures plus name
		ipengine::ipid m_shaderId;
		ipengine::ipid m_materialId;
	};

	class TextureFile
	{
	public:
		TextureFile() = default;
		TextureFile(ipengine::ipid id, std::string path, bool isCube = false) : m_textureId(id), m_path(path), m_isCube(isCube){};
		ipengine::ipid m_textureId;
		std::string m_path;
		bool m_isCube;
	};

	class TextureData
	{
	public:
		TextureData() :
			m_texturefileId(IPID_INVALID),
			m_offset(0.0f),
			m_size(1.0f)
		{}
		TextureData(ipengine::ipid fileid,
					glm::vec2 offset = glm::vec2( 0,0 ),
					glm::vec2 size= glm::vec2(1,1)) :
			m_texturefileId(fileid),
			m_offset(offset),
			m_size(size) {}
		ipengine::ipid m_texturefileId;
		glm::vec2 m_offset;
		glm::vec2 m_size;
		std::vector<uint8_t> data;
	};

	class VertexData
	{
	public:
		VertexData(glm::vec3 pos, glm::vec2 uv, glm::vec3 norm, glm::vec3 tan) :m_position(pos), m_uv(uv), m_normal(norm), m_tangent(tan)
		{}
		glm::vec3 m_position;
		glm::vec2 m_uv;
		glm::vec3 m_normal;
		glm::vec3 m_tangent;
	};

	class VertexVector
	{
	public:
		VertexVector()
		{
		}
		VertexVector(const VertexVector& other) : m_front(other.m_front), m_back(other.m_back)
		{
		}
		const std::vector<VertexData>& getData() const {
			return m_front;
		}
		std::vector<VertexData>& setData() {
			return m_front;
		}

		void swap() { m_front.swap(m_back); }
	private:
		std::vector<VertexData> m_front;
		std::vector<VertexData> m_back;
	};

	class MeshData
	{
	public:
		VertexVector m_vertices;
		std::vector<index> m_indices;
		ipengine::ipid m_meshId;
		bool m_dynamic;
		bool m_dirty;
		bool m_isdoublesided;
		glm::vec2 m_texCoordOffset = glm::vec2(0.0f);
		glm::vec2 m_texCoordScale = glm::vec2(1.0f);
		//TODO: buffer intermediate data and write to actual mesh data at the end
		void updateNormals()
		{
			for (size_t i = 0; i < m_vertices.getData().size(); ++i)
			{
				m_vertices.setData()[i].m_normal = glm::vec3(0.0f);
			}
			//sum up normals
			for (size_t i = 0; i < m_indices.size(); i += 3)
			{
				glm::vec3 v1 = m_vertices.getData()[m_indices[i]].m_position;
				glm::vec3 v2 = m_vertices.getData()[m_indices[i + 1]].m_position;
				glm::vec3 v3 = m_vertices.getData()[m_indices[i + 2]].m_position;

				//counter clockwise winding
				glm::vec3 edge1 = v2 - v1;
				glm::vec3 edge2 = v3 - v1;

				glm::vec3 normal = glm::cross(edge1, edge2);

				//for each Vertex all corresponing normals are added. The result is a non unit length vector wich is the weighted average direction of all assigned normals.
				m_vertices.setData()[m_indices[i]].m_normal += normal;
				m_vertices.setData()[m_indices[i + 1]].m_normal += normal;
				m_vertices.setData()[m_indices[i + 2]].m_normal += normal;
			}
			//normalize them in shader
		}
		//TODO: buffer intermediate data and write to actual mesh data at the end
		void updateTangents()
		{
			static thread_local std::vector<glm::vec3> bitangents;
			//initialize tangents and bitangents with nullvecs
			for (size_t i = 0; i < m_vertices.getData().size(); ++i)
			{
				m_vertices.setData()[i].m_tangent = glm::vec3(0.0f, 0.0f, 0.0f);
				bitangents.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
			}

			float det;
			glm::vec3 tangent;
			glm::vec3 bitangent;
			glm::vec3 normal;

			

			//calculate and average tangents and bitangents just as we did when calculating the normals
			for (size_t i = 0; i < m_indices.size(); i += 3)
			{
				//3 vertices of a triangle
				glm::vec3 v1 = m_vertices.getData()[m_indices[i]].m_position;
				glm::vec3 v2 = m_vertices.getData()[m_indices[i + 1]].m_position;
				glm::vec3 v3 = m_vertices.getData()[m_indices[i + 2]].m_position;

				//uvs
				glm::vec2 uv1 = m_vertices.getData()[m_indices[i]].m_uv;
				glm::vec2 uv2 = m_vertices.getData()[m_indices[i + 1]].m_uv;
				glm::vec2 uv3 = m_vertices.getData()[m_indices[i + 2]].m_uv;

				//calculate edges in counter clockwise winding order
				glm::vec3 edge1 = v2 - v1;
				glm::vec3 edge2 = v3 - v1;

				//deltaus and deltavs
				glm::vec2 duv1 = uv2 - uv1;
				glm::vec2 duv2 = uv3 - uv1;

				det = duv1.x * duv2.y - duv2.x * duv1.y;

				if (fabs(det) < 1e-6f)		//if delta stuff is close to nothing ignore it
				{
					tangent = glm::vec3(1.0f, 0.0f, 0.0f);
					bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
				}
				else
				{
					det = 1.0f / det;

					tangent.x = det * (duv2.y * edge1.x - duv1.y * edge2.x);
					tangent.y = det * (duv2.y * edge1.y - duv1.y * edge2.y);
					tangent.z = det * (duv2.y * edge1.z - duv1.y * edge2.z);

					bitangent.x = det * (-duv2.x * edge1.x + duv1.x * edge2.x);
					bitangent.y = det * (-duv2.x * edge1.y + duv1.x * edge2.y);
					bitangent.z = det * (-duv2.x * edge1.z + duv1.x * edge2.z);
				}

				m_vertices.setData()[m_indices[i]].m_tangent += tangent;
				m_vertices.setData()[m_indices[i + 1]].m_tangent += tangent;
				m_vertices.setData()[m_indices[i + 2]].m_tangent += tangent;

				bitangents[m_indices[i]] += bitangent;
				bitangents[m_indices[i + 1]] += bitangent;
				bitangents[m_indices[i + 2]] += bitangent;
			}

			//orthogonalize and normalize tangents
			for (size_t i = 0; i < m_vertices.getData().size(); i++)
			{
				//normalize the stuff from before
				//m_vertices.setData()[i].m_normal = glm::normalize(m_vertices.getData()[i].m_normal);
				//m_vertices.setData()[i].m_tangent = m_vertices.getData()[i].m_tangent;

				//gram schmidt reorthogonalize normal-tangent system
				m_vertices.setData()[i].m_tangent = glm::normalize(m_vertices.getData()[i].m_tangent - (glm::dot(m_vertices.getData()[i].m_normal, m_vertices.getData()[i].m_tangent) * m_vertices.getData()[i].m_normal));

				//correct handedness where necessary
				if (glm::dot(glm::cross(m_vertices.getData()[i].m_normal, m_vertices.setData()[i].m_tangent), glm::normalize(bitangents[i])) < 0.0f)
					m_vertices.setData()[i].m_tangent *= -1.0f;
			}
			bitangents.clear();
		}

	};
	
	class MeshedObject
	{
	public:
		MeshedObject(std::vector<MeshData*>& mdata, ipengine::ipid id, std::string path = "") :m_meshes(mdata), m_meshObjectId(id), filepath(path) {}
		ipengine::ipid m_meshObjectId;
		std::vector<MeshData*> m_meshes;
		std::unordered_map<ipengine::ipid, ipengine::ipid> meshtomaterial;
		std::string filepath;
		virtual void swap()
		{
			for (auto& mesh : m_meshes)
			{
				mesh->m_vertices.swap();
			}
		}
	};

	class Component
	{
	public:
		Component(const ipengine::ipid& iid, const ipengine::ipid& tid, const ipengine::ipid& eid) :
			internalID(iid),
			typeID(tid),
			entityID(eid)
		{}

		Component() :
			internalID(IPID_INVALID),
			typeID(IPID_INVALID),
			entityID(IPID_INVALID)
		{}

		Component(const Component& other) = default;
		Component(Component&& other) = default;
		Component& operator=(const Component& other) = default;
		Component& operator=(Component&& other) = default;

		virtual ~Component() {}

		const ipengine::ipid& getInternalID() { return internalID; }
		const ipengine::ipid& getTypeID() { return typeID; }
		const ipengine::ipid& getEntity() { return entityID; }
		
	private:
		ipengine::ipid internalID;
		ipengine::ipid typeID; //quite redundant... //but more efficient later on
		ipengine::ipid entityID;
	};

	class Entity
	{
	public:
		Entity() :
			m_entityId(-1),
			m_parent(nullptr),
			isBoundingBox(true),
			isActive(false),
			m_components()
		{}
		Entity(const Entity& other) :
			m_transformData(other.m_transformData),
			m_parent(other.m_parent),
			m_name(other.m_name),
			m_boundingData(other.m_boundingData),
			isBoundingBox(other.isBoundingBox),
			isActive(other.isActive),
			m_components()
		{
			m_entityId = -1;

			//TODO: component vector copy
		}
		Entity(Entity&& other) :
			m_transformData(std::move(other.m_transformData)),
			m_parent(other.m_parent),
			m_name(std::move(other.m_name)),
			m_boundingData(other.m_boundingData),
			isBoundingBox(other.isBoundingBox),
			isActive(other.isActive),
			m_components(std::move(other.m_components))
		{
			m_entityId = -1;

			//TODO: move vector copy
		}
		Entity(
			ipengine::ipid id,
			Transform& transform,
			BoundingData& boundingdata,
			bool boundingbox,
			bool active):
				m_entityId(id),
				m_parent(nullptr),
				isBoundingBox(boundingbox),
				isActive(active),
				m_transformData(transform),
				m_boundingData(boundingdata),
			m_components()
		{
		}

		//virtual ~Entity() {};

		Transform m_transformData;
		Entity* m_parent;
		ipengine::ipid m_entityId;
		std::string m_name;
		BoundingData m_boundingData;
		bool boundingDataDirty;
		bool isBoundingBox; //True for Box, False for Sphere in Union BoundingData
		bool isActive;
		std::vector<std::unique_ptr<Component>> m_components;
		//std::map<std::string, boost::any> m_decorators;
		virtual void swap() { m_transformData.swap(); }

		//handle transform parent/child stuff
		void setParent(Entity* parent)
		{
			if (parent)
			{
				m_parent = parent;
				m_transformData.setParent(&parent->m_transformData);
			}
			else
			{
				orphane();
			}
		}

		void orphane()
		{
			m_parent = nullptr;
			m_transformData.orphane();
		}

		bool shouldCollide()
		{
			if (isBoundingBox)
			{
				if (glm::dot(m_boundingData.box.m_size, m_boundingData.box.m_size) > 1.e-5)
					return true;
			}
			else
			{
				if (m_boundingData.sphere.m_radius > 1.e-5)
					return true;
			}
			return false;
		}

		void updateBoundingData(float deltasecs)
		{
			if (shouldCollide())
			{
				if (isBoundingBox)
				{
					glm::vec3 oldpos{ glm::vec3(m_boundingData.box.bdtoworld[3]) };
					glm::mat4 bbtoentity = glm::translate(m_boundingData.box.m_center) * glm::mat4(m_boundingData.box.m_rotation) * glm::scale(m_boundingData.box.m_size * 0.5f);					
					m_boundingData.box.bdtoworld = m_transformData.getLocalToWorldMatrix() * bbtoentity;
					glm::vec3 newpos{ glm::vec3(m_boundingData.box.bdtoworld[3]) };
					m_boundingData.box.m_velocity = (newpos - oldpos) / deltasecs;
				}
				else
				{
					glm::vec3 oldpos{ glm::vec3(m_boundingData.sphere.bdtoworld[3]) };
					glm::mat4 bstoentity = glm::translate(m_boundingData.sphere.m_center) * glm::scale(glm::vec3(m_boundingData.sphere.m_radius));
					m_boundingData.sphere.bdtoworld = m_transformData.getLocalToWorldMatrix() * bstoentity;
					glm::vec3 newpos{ glm::vec3(m_boundingData.sphere.bdtoworld[3]) };
					m_boundingData.sphere.m_velocity = (newpos - oldpos) / deltasecs;
				}
				boundingDataDirty = false;
			}
		}

		bool addComponent(Component* component)
		{
			//TODO: prevent doubles
			m_components.push_back(std::unique_ptr<Component>(component));
			return true;
		}

		void removeComponent(ipengine::ipid comptype)
		{
			m_components.erase(std::remove_if(m_components.begin(), m_components.end(), [comptype](auto& p) { return p->getTypeID() == comptype; }), m_components.end());
		}

		template <typename ComponentType>
		void removeComponent()
		{		
			m_components.erase(std::remove_if(m_components.begin(), m_components.end(), [](auto& p) { return dynamic_cast<ComponentType*>(p.get()); }), m_components.end());
		}

		Component* getComponent(ipengine::ipid comptype)
		{
			for (auto& c : m_components)
			{
				if (c.get()->getTypeID() == comptype)
					return c.get();
			}
			return nullptr;
		}

		template <typename ComponentType>
		ComponentType* getComponent()
		{
			for (auto& c : m_components)
			{
				auto cp = dynamic_cast<ComponentType*>(c.get());
				if (cp)
					return cp;
			}
			return nullptr;
		}

		glm::vec3 getBVWorldPos()
		{
			if (isBoundingBox)
			{
				return glm::vec3(m_boundingData.box.bdtoworld[3]);
			}
			else
			{
				return glm::vec3(m_boundingData.sphere.bdtoworld[3]);
			}
		}

		glm::quat getBVWorldRot()
		{
			if (isBoundingBox)
			{
				return glm::normalize(glm::quat_cast(glm::mat3(
					glm::normalize(m_boundingData.box.bdtoworld[0]),
					glm::normalize(m_boundingData.box.bdtoworld[1]),
					glm::normalize(m_boundingData.box.bdtoworld[2])
				)));
			}
			else
			{
				return glm::quat();
			}
		}

		glm::vec3 getBVWorldScale()
		{
			if (isBoundingBox)
			{
				return glm::vec3(
					glm::length(m_boundingData.box.bdtoworld[0]),
					glm::length(m_boundingData.box.bdtoworld[1]),
					glm::length(m_boundingData.box.bdtoworld[2])
				);
			}
			else
			{
				return glm::vec3(0.0f);
			}
		}

		float getBVRadius()
		{
			if (isBoundingBox)
			{
				return 0.0f;
			}
			else
			{
				return/* m_boundingData.sphere.m_radius **/ glm::max(glm::length(m_boundingData.sphere.bdtoworld[0]), glm::max(glm::length(m_boundingData.sphere.bdtoworld[1]), glm::length(m_boundingData.sphere.bdtoworld[2])));
			}
		}
	};

	class ThreeDimEntity : public Entity
	{

	public:
		ThreeDimEntity(
			ipengine::ipid id,
			Transform& transform,
			BoundingData& boundingdata,
			bool boundingbox,
			bool active,
			MeshedObject* meshes):
				Entity(id, transform, boundingdata, boundingbox, active),
				m_mesheObjects(meshes)
		{
		}
		MeshedObject* m_mesheObjects;
		virtual void swap()
		{
			m_transformData.swap();
			m_mesheObjects->swap();
		}

		void generateBoundingBox()
		{
			glm::vec3 min(std::numeric_limits<float>::max());
			glm::vec3 max(std::numeric_limits<float>::lowest());

			for (auto& m : m_mesheObjects->m_meshes)
			{
				for (auto& v : m->m_vertices.getData())
				{
					min.x = glm::min(min.x, v.m_position.x);
					min.y = glm::min(min.y, v.m_position.y);
					min.z = glm::min(min.z, v.m_position.z);

					max.x = glm::max(max.x, v.m_position.x);
					max.y = glm::max(max.y, v.m_position.y);
					max.z = glm::max(max.z, v.m_position.z);
				}
			}

			SCM::BoundingBox bb;
			bb.m_center = min + (max - min) * 0.5f;
			bb.m_rotation = glm::quat();
			bb.m_size = glm::vec3(glm::max(max.x - min.x, 1.e-6f), glm::max(max.y - min.y, 1.e-6f), glm::max(max.z - min.z, 1.e-6f));
			this->isBoundingBox = true;
			this->m_boundingData.box = bb;
		}

		/// generates minimum volume bounding boxes
		void generateBoundingBoxOriented(unsigned int coarseSteps, unsigned int fineSteps)
		{
			//coarse and fine angle delta
			float cad = glm::pi<float>() / static_cast<float>(coarseSteps);
			float fad = (2.0f * cad) / static_cast<float>(fineSteps);

			//first find a standard aab
			glm::vec3 min;
			glm::vec3 max;
			glm::vec3 center;

			//initial bb axes
			glm::vec3 ax{ 1.0f, 0.0f, 0.0f };
			glm::vec3 ay{ 0.0f, 1.0f, 0.0f };
			glm::vec3 az{ 0.0f, 0.0f, 1.0f };

			//final bb axes
			glm::vec3 min_vol_ax{ 1.0f, 0.0f, 0.0f };
			glm::vec3 min_vol_ay{ 0.0f, 1.0f, 0.0f };
			glm::vec3 min_vol_az{ 0.0f, 0.0f, 1.0f };

			calcBoundingBox(ax, ay, az, min, max, center);

			//volume to minimize
			float vol = calcBBVolume(calcBBScale(min, max));

			//minimize volume via x-axis rotation ----------------------------------------------
			//we only have to search in range [0, PI]

			//coarse search
			for (unsigned int s = 0; s <= coarseSteps; ++s)
			{
				calcBoundingBox(ax, ay, az, min, max, center);
				float nvol = calcBBVolume(calcBBScale(min, max));
				if (calcBBVolume(calcBBScale(min, max)) < vol)
				{
					vol = nvol;
					min_vol_ax = ax;
					min_vol_ay = ay;
					min_vol_az = az;
				}
				rotateBBAxes(ax, ay, az, ax, cad);
			}

			ax = min_vol_ax;
			ay = min_vol_ay;
			az = min_vol_az;

			//search in -cad + cad interval for fine search : start at min rot - cad
			rotateBBAxes(ax, ay, az, ax, -cad);

			//fine search
			for (unsigned int s = 0; s <= fineSteps; ++s)
			{
				calcBoundingBox(ax, ay, az, min, max, center);
				float nvol = calcBBVolume(calcBBScale(min, max));
				if (calcBBVolume(calcBBScale(min, max)) < vol)
				{
					vol = nvol;
					min_vol_ax = ax;
					min_vol_ay = ay;
					min_vol_az = az;
				}
				rotateBBAxes(ax, ay, az, ax, fad);
			}

			//minimize volume via y-axis rotation -----------------------------------------------------
			//we only have to search in range [0, PI]

			ax = min_vol_ax;
			ay = min_vol_ay;
			az = min_vol_az;

			//coarse search
			for (unsigned int s = 0; s <= coarseSteps; ++s)
			{
				calcBoundingBox(ax, ay, az, min, max, center);
				float nvol = calcBBVolume(calcBBScale(min, max));
				if (calcBBVolume(calcBBScale(min, max)) < vol)
				{
					vol = nvol;
					min_vol_ax = ax;
					min_vol_ay = ay;
					min_vol_az = az;
				}
				rotateBBAxes(ax, ay, az, ay, cad);
			}

			ax = min_vol_ax;
			ay = min_vol_ay;
			az = min_vol_az;

			//search in -cad + cad interval for fine search : start at min rot - cad
			rotateBBAxes(ax, ay, az, ay, -cad);

			//fine search
			for (unsigned int s = 0; s <= fineSteps; ++s)
			{
				calcBoundingBox(ax, ay, az, min, max, center);
				float nvol = calcBBVolume(calcBBScale(min, max));
				if (calcBBVolume(calcBBScale(min, max)) < vol)
				{
					vol = nvol;
					min_vol_ax = ax;
					min_vol_ay = ay;
					min_vol_az = az;
				}
				rotateBBAxes(ax, ay, az, ay, fad);
			}

			//minimize volume via z-axis rotation -----------------------------------------------------
			//we only have to search in range [0, PI]

			ax = min_vol_ax;
			ay = min_vol_ay;
			az = min_vol_az;

			//coarse search
			for (unsigned int s = 0; s <= coarseSteps; ++s)
			{
				calcBoundingBox(ax, ay, az, min, max, center);
				float nvol = calcBBVolume(calcBBScale(min, max));
				if (calcBBVolume(calcBBScale(min, max)) < vol)
				{
					vol = nvol;
					min_vol_ax = ax;
					min_vol_ay = ay;
					min_vol_az = az;
				}
				rotateBBAxes(ax, ay, az, az, cad);
			}

			ax = min_vol_ax;
			ay = min_vol_ay;
			az = min_vol_az;

			//search in -cad + cad interval for fine search : start at min rot - cad
			rotateBBAxes(ax, ay, az, az, -cad);

			//fine search
			for (unsigned int s = 0; s <= fineSteps; ++s)
			{
				calcBoundingBox(ax, ay, az, min, max, center);
				float nvol = calcBBVolume(calcBBScale(min, max));
				if (calcBBVolume(calcBBScale(min, max)) < vol)
				{
					vol = nvol;
					min_vol_ax = ax;
					min_vol_ay = ay;
					min_vol_az = az;
				}
				rotateBBAxes(ax, ay, az, az, fad);
			}

			//now ew have (approximately) the orthogonal basis that minimizes the volume for our bounding box.
			//=> calculate the final bounding box with respect to min_vol_ax, min_vol_ay, min_vol_az
			//and translate that to our bounding box format

			calcBoundingBox(min_vol_ax, min_vol_ay, min_vol_az, min, max, center);

			BoundingBox bb;
			bb.m_rotation = glm::quat_cast(glm::mat3(glm::normalize(min_vol_ax), glm::normalize(min_vol_ay), glm::normalize(min_vol_az)));
			bb.m_center = bb.m_rotation * center; //center is currently defined in the coordinate system formed from the previously found volume-minimizing axes, so a transformation into world space is necessary.
			bb.m_size = calcBBScale(min, max) * 2.0f;
			bb.m_size.x = glm::max(bb.m_size.x, 1.e-6f);
			bb.m_size.y = glm::max(bb.m_size.y, 1.e-6f);
			bb.m_size.z = glm::max(bb.m_size.z, 1.e-6f);
			boundingDataDirty = true;

			m_boundingData = BoundingData(bb);
		}

		//TODO: second version of that thing above to better align the box to the mesh. Minimizing perimeter or edge lengths could be a good option.
		//Needs to be tested

		void generateBoundingSphere()
		{
			if (m_mesheObjects->m_meshes.size() == 0)
				return;
			//Ritter's algorithm for minial bounding sphere approximation
			SCM::BoundingSphere bs;
			bs.m_center = glm::vec3(0.0f);
			bs.m_radius = 0.0f;

			glm::vec3 x = m_mesheObjects->m_meshes[0]->m_vertices.getData()[0].m_position;
			glm::vec3 y = x;
			for (auto& m : m_mesheObjects->m_meshes)
			{
				for (auto& v : m->m_vertices.getData())
				{
					if (glm::length(v.m_position - x) > glm::length(y - x))
						y = v.m_position;
				}
			}
			glm::vec3 z = x;
			for (auto& m : m_mesheObjects->m_meshes)
			{
				for (auto& v : m->m_vertices.getData())
				{
					if (glm::length(v.m_position - y) > glm::length(z - y))
						z = v.m_position;
				}
			}

			bs.m_center = y + (z - y) * 0.5f;
			bs.m_radius = glm::length(z - y) * 0.5f;

			for (auto& m : m_mesheObjects->m_meshes)
			{
				for (auto& v : m->m_vertices.getData())
				{
					if (glm::length(v.m_position - bs.m_center) > bs.m_radius)
					{
						glm::vec3 ctp = v.m_position - bs.m_center;
						glm::vec3 nd1 = bs.m_center + (-glm::normalize(ctp) * bs.m_radius);
						bs.m_radius = glm::length(v.m_position - nd1) * 0.5f;
						bs.m_center = nd1 + (v.m_position - nd1) * 0.5f;
					}
				}
			}

			this->isBoundingBox = false;
			this->m_boundingData.sphere = bs;
		}

	private:
		void rotateBBAxes(glm::vec3& ax, glm::vec3& ay, glm::vec3& az, const glm::vec3& rotaxis, float angle)
		{
			glm::quat rot { glm::angleAxis(angle, rotaxis) };

			ax = rot * ax;
			ay = rot * ay;
			az = rot * az;
		}

		float calcBBVolume(const glm::vec3& bbscale)
		{
			return bbscale.x * bbscale.y * bbscale.z * 8.0f;
		}

		glm::vec3 calcBBScale(const glm::vec3& min, const glm::vec3& max)
		{
			return 0.5f * (max - min);
		}

		void calcBoundingBox(const glm::vec3& ax, const glm::vec3& ay, const glm::vec3& az, glm::vec3& outmin, glm::vec3& outmax, glm::vec3& outcenter)
		{
			outmin = glm::vec3(std::numeric_limits<float>::max());
			outmax = glm::vec3(std::numeric_limits<float>::lowest());
			for (auto& m : m_mesheObjects->m_meshes)
			{
				for (auto& v : m->m_vertices.getData())
				{
					glm::vec3 pv{
						glm::dot(v.m_position, ax),
						glm::dot(v.m_position, ay),
						glm::dot(v.m_position, az)
					};

					outmin.x = glm::min(outmin.x, pv.x);
					outmin.y = glm::min(outmin.y, pv.y);
					outmin.z = glm::min(outmin.z, pv.z);

					outmax.x = glm::max(outmax.x, pv.x);
					outmax.y = glm::max(outmax.y, pv.y);
					outmax.z = glm::max(outmax.z, pv.z);
				}
			}
			outcenter = outmin + (outmax - outmin) * 0.5f;
		}

		glm::vec3 projectOntoPlane(const glm::vec3& p, const glm::vec3& a, const glm::vec3& n)
		{
			return a + (p - a) - (glm::dot(p - a, n) * n);
		}
	};

	//lights

	class DirectionalLight : public Entity
	{
	public:
		DirectionalLight(
			//Entity params
			ipengine::ipid id,
			Transform& transform,
			BoundingData& boundingdata,
			bool boundingbox,
			bool active,

			//Light params
			const glm::vec3& color) :
				Entity(id, transform, boundingdata, boundingbox, active),
				m_color(color),
				castShadows(false)
		{
		}

		DirectionalLight(
			//Entity params
			ipengine::ipid id,
			Transform& transform,
			BoundingData& boundingdata,
			bool boundingbox,
			bool active,
			//Light params
			const glm::vec3& color,
			int _shadowResX,
			int _shadowResY,
			int _shadowBlurPasses,
			float _shadowVarianceBias,
			float _lightBleedReduction,
			float _shadowWarpFactor,
			const glm::vec3& _shadowMapVolumeMin,
			const glm::vec3& _shadowMapVolumeMax
			) :
			Entity(id, transform, boundingdata, boundingbox, active),
			m_color(color),
			castShadows(true), //TODO: finish constructor
			shadowResX(_shadowResX),
			shadowResY(_shadowResY),
			shadowBlurPasses(_shadowBlurPasses),
			shadowVarianceBias(_shadowVarianceBias),
			lightBleedReduction(_lightBleedReduction),
			shadowMapVolumeMin(_shadowMapVolumeMin),
			shadowMapVolumeMax(_shadowMapVolumeMax),
			shadowWarpFactor(_shadowWarpFactor)

		{}

		glm::vec3 getVSDirection(const glm::mat4& viewmat)
		{
			return glm::mat3(viewmat) * (m_transformData.getDirection());
		}

		glm::vec3 getDirection()
		{
			return m_transformData.getDirection();
		}

		glm::vec3 getVSPosition(const glm::mat4& viewmat)
		{
			return glm::vec3(viewmat * glm::vec4(m_transformData.getWorldPosition(), 1.0f));
		}

		glm::vec3 getPosition()
		{
			return m_transformData.getWorldPosition();
		}

		glm::vec3 m_color;
		bool castShadows = false;
		int shadowResX = 0;
		int shadowResY = 0;
		int shadowBlurPasses = 0;
		float shadowVarianceBias = 0;
		float lightBleedReduction = 0;
		glm::vec3 shadowMapVolumeMin = { 0,0,0 };
		glm::vec3 shadowMapVolumeMax = { 0,0,0 };
		float shadowWarpFactor = 1;
	};

	class PointLight : public Entity
	{
	public:
		PointLight(
			//Entity params
			ipengine::ipid id,
			Transform& transform,
			BoundingData& boundingdata,
			bool boundingbox,
			bool active,
			//Light params
			const glm::vec3& color,
			float maxRange) :
				Entity(id, transform, boundingdata, boundingbox, active),
				m_color(color),
				m_range(maxRange)
		{}

		glm::vec3 getVSPosition(const glm::mat4& viewmat)
		{
			return glm::vec3(viewmat * glm::vec4(m_transformData.getWorldPosition(), 1.0f));
		}

		glm::vec3 getPosition()
		{
			return m_transformData.getWorldPosition();
		}

		glm::vec3 m_color;
		float m_range;
	};

	class SpotLight : public Entity
	{
	public:
		SpotLight(
			//Entity params
			ipengine::ipid id,
			Transform& transform,
			BoundingData& boundingdata,
			bool boundingbox,
			bool active,
			//Light params
			const glm::vec3& color,
			float maxRange,
			float innerConeAngle,
			float outerConeAngle) :
				Entity(id, transform, boundingdata, boundingbox, active),
				m_color(color),
				m_range(maxRange),
				m_innerConeAngle(innerConeAngle),
				m_outerConeAngle(outerConeAngle)
		{}

		glm::vec3 getVSDirection(const glm::mat4& viewmat)
		{
			return glm::mat3(viewmat) * (m_transformData.getDirection());
		}

		glm::vec3 getDirection()
		{
			return m_transformData.getDirection();
		}

		glm::vec3 getVSPosition(const glm::mat4& viewmat)
		{
			return glm::vec3(viewmat * glm::vec4(m_transformData.getWorldPosition(), 1.0f));
		}

		glm::vec3 getPosition()
		{
			return m_transformData.getWorldPosition();
		}

		glm::vec3 m_color;
		float m_range;
		float m_innerConeAngle;
		float m_outerConeAngle;
	};

	class ISimpleContentModule_API : public IModule_API
	{
	public:
		//helpers
		//assume (0, 0, -1) as default front vector
		static glm::quat dirToQuat(const glm::vec3& direction)
		{
			glm::quat r1 = RotationBetweenVectors(glm::vec3(0, 0, -1), direction);

			glm::vec3 right = glm::cross(direction, glm::vec3(0, 1, 0));
			glm::vec3 desiredUp = glm::cross(right, direction);

			glm::vec3 newUp = r1 * glm::vec3(0.0f, 1.0f, 0.0f);
			glm::quat r2 = RotationBetweenVectors(newUp, desiredUp);

			return r2 * r1;
		}

		//from opengl-tutorial.org: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/#quaternions
		static glm::quat RotationBetweenVectors(const glm::vec3& _start, const glm::vec3& _dest)
		{
			glm::vec3 start = glm::normalize(_start);
			glm::vec3 dest = glm::normalize(_dest);

			float cosTheta = glm::dot(start, dest);
			glm::vec3 rotationAxis;

			if (cosTheta < -1 + 0.001f)
			{
				// special case when vectors in opposite directions:
				// there is no "ideal" rotation axis
				// So guess one; any will do as long as it's perpendicular to start
				rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
				if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
					rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

				rotationAxis = glm::normalize(rotationAxis);
				return glm::angleAxis(glm::radians(180.0f), rotationAxis);
			}

			rotationAxis = glm::cross(start, dest);

			float s = glm::sqrt((1 + cosTheta) * 2);
			float invs = 1 / s;

			return glm::quat(
				s * 0.5f,
				rotationAxis.x * invs,
				rotationAxis.y * invs,
				rotationAxis.z * invs
			);

		}

		//assume (0, 0, -1) as default front vector
		static glm::vec3 quatToDir(const glm::quat& quaternion)
		{
			return quaternion * glm::vec3(0, 0, -1);
		}

		virtual void Swap()
		{
			for (auto& ent : entities)
			{
				ent.second->swap();

			}
		}
		virtual std::unordered_map<ipengine::ipid, Entity*>& getEntities()
		{
			return entities;
		}

		virtual std::vector<Entity*> getEntitiesByName(std::string name)
		{
			std::vector<Entity*> ents;
			for (auto& e : entities)
			{
				if (e.second->m_name == name)
					ents.push_back(e.second);
			}
			return std::move(ents);
		}

		virtual std::unordered_map<ipengine::ipid, ThreeDimEntity*>& getThreeDimEntities()
		{
			return threedimentities;
		}

		virtual std::unordered_map<ipengine::ipid, DirectionalLight*>& getDirLights()
		{
			return dirLights;
		}

		virtual std::unordered_map<ipengine::ipid, PointLight*>& getPointLights()
		{
			return pointLights;
		}

		virtual std::unordered_map<ipengine::ipid, SpotLight*>& getSpotLights()
		{
			return spotLights;
		}

		virtual std::vector<ShaderData>& getShaders()
		{
			return shaders;
		}

		virtual std::vector<MaterialData>& getMaterials()
		{
			return materials;
		}

		virtual std::vector<TextureFile>& getTextures()
		{
			return texturefiles;
		}

		virtual std::vector<MeshData>& getMeshes()
		{
			return meshes;
		}

		virtual std::vector<MeshedObject>& getMeshedObjects()
		{
			return meshedobjects;
		}

		virtual TextureFile* getTextureById(ipengine::ipid id)
		{
			auto itF = std::find_if(texturefiles.begin(), texturefiles.end(), [id](TextureFile& a)->bool {return a.m_textureId == id; });
			if (itF != texturefiles.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		}
		virtual MaterialData* getMaterialById(ipengine::ipid id)
		{ 
			auto itF = std::find_if(materials.begin(), materials.end(), [id](MaterialData& a)->bool {return a.m_materialId == id; });
			if (itF != materials.end())
			{
				return &*itF;
			}
			else
				return nullptr; 
		};
		virtual ShaderData* getShaderById(ipengine::ipid id)
		{ 
			auto itF = std::find_if(shaders.begin(), shaders.end(), [id](ShaderData& a)->bool {return a.m_shaderId == id; });
			if (itF != shaders.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		};
		virtual Entity* getEntityById(ipengine::ipid id)
		{ 
			if (entities.count(id) > 0)
				return entities[id];
			else
				return nullptr;
		};
		virtual MeshData* getMeshById(ipengine::ipid id)
		{ 
			auto itF = std::find_if(meshes.begin(), meshes.end(), [id](MeshData& a)->bool {return a.m_meshId == id; });
			if (itF != meshes.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		}
		virtual MeshedObject* getMeshedObjectById(ipengine::ipid id)
		{ 
			auto itF = std::find_if(meshedobjects.begin(), meshedobjects.end(), [id](MeshedObject& a)->bool {return a.m_meshObjectId == id; });
			if (itF != meshedobjects.end())
			{
				return &*itF;
			}
			else
				return nullptr;
		}
		virtual ipengine::ipid addMeshFromFile(std::string path, std::string format, std::vector<ipengine::ipid> mats) = 0;
		virtual ipengine::ipid getDefaultShaderId() = 0;
		virtual ipengine::ipid getDefaultTextureId() = 0;

		virtual bool setEntityParent(ipengine::ipid child, ipengine::ipid parent)
		{
			auto ent = getEntityById(child);
			auto ent2 = getEntityById(parent);
			if (ent != nullptr && ent2 != nullptr)
			{
				ent->setParent(ent2);
				return true;
			}
			return false;
		}

		ipengine::ipid registerComponentType(const std::string& ctypename)
		{
			auto newid = m_core->getIDGen().createID();
			componentTypes.insert(std::make_pair(ctypename, newid));
			return newid;
		}

		bool unregisterComponentType(const std::string& ctypename)
		{
			return componentTypes.erase(ctypename);
		}

		ipengine::ipid getComponentTypeByName(const std::string& ctypename)
		{
			auto it = componentTypes.find(ctypename);
			if (it != componentTypes.end())
				return it->second;
			return IPID_INVALID;
		}

		std::string getComponentTypenameById(const ipengine::ipid ctypeid)
		{
			auto ctypeit = std::find_if(componentTypes.begin(), componentTypes.end(), [ctypeid](auto& c) {return c.second == ctypeid; });
			return ctypeit != componentTypes.end() ? ctypeit->first : "";
		}

		bool isComponent(ipengine::ipid comptype)
		{
			return std::find_if(componentTypes.begin(), componentTypes.end(), [comptype](auto& c) {return c.second == comptype; }) != componentTypes.end();
		}

		bool isComponent(const std::string& comptypename)
		{
			return componentTypes.find(comptypename) != componentTypes.end();
		}

		//Would prefer add/remove/const get functions over returning container refs. All virtual so SCMs can change their implementation more freely
	private:

		std::unordered_map<ipengine::ipid, Entity*> entities;
		std::unordered_map<ipengine::ipid, ThreeDimEntity*> threedimentities;
		std::unordered_map<ipengine::ipid, DirectionalLight*> dirLights;
		std::unordered_map<ipengine::ipid, PointLight*> pointLights;
		std::unordered_map<ipengine::ipid, SpotLight*> spotLights;
		std::vector<ShaderData> shaders;
		std::vector<MaterialData> materials;
		std::vector<TextureFile> texturefiles;
		std::vector<MeshData> meshes;
		std::vector<MeshedObject> meshedobjects;
	protected:
		std::unordered_map<std::string, ipengine::ipid> componentTypes;
	};

	static std::string glmvec2tostring(glm::vec2 v)
	{
		std::string out = "";
		out += std::to_string(v.x) + " / " + std::to_string(v.y);
		return out;
	}
	static std::string glmvec3tostring(glm::vec3 v)
	{
		std::string out = "";
		out += std::to_string(v.x) + " / " + std::to_string(v.y) + " / " + std::to_string(v.z);
		return out;
	}
	static std::string glmquattostring(glm::quat q)
	{
		std::string out = "";
		out += std::to_string(q.w) + " / " + std::to_string(q.x) + " / " + std::to_string(q.y) + " / " + std::to_string(q.z);
		return out;
	}

	static std::string allEntitiesAsString(ISimpleContentModule_API& content, bool withproperties = false)
	{
		std::string out="";
		for (auto ents : content.getEntities())
		{
			out += "Id: " + std::to_string(ents.second->m_entityId) + ": " + ents.second->m_name + " Active: " + (ents.second->isActive?"True":"False") + "\n";
			if (withproperties)
			{
				if (ents.second->m_parent != nullptr)
				{
					out += "\t Parent: " + std::to_string(ents.second->m_parent->m_entityId) + "\n";;
				}
				else
					out += "\t Parent: -\n";
				auto& vd = ents.second->m_transformData;
				out += "\t Transform: \n";
				out += "\t\t Location: " + glmvec3tostring(vd.getLocalPosition()) + "\n";
				out += "\t\t Scale: " + glmvec3tostring(vd.getLocalScale()) + "\n";
				out += "\t\t Rotation: " + glmquattostring(vd.getLocalRotation()) + "\n";
				out += "\t\t Local X: " + glmvec3tostring(vd.getLocalXAxis()) + "\n";
				out += "\t\t Local Y: " + glmvec3tostring(vd.getLocalYAxis()) + "\n";
				out += "\t\t Local Z: " + glmvec3tostring(vd.getLocalZAxis()) + "\n";

				auto& bd = ents.second->m_boundingData;
				if (ents.second->isBoundingBox)
				{
					out += "\t Bounding Box: \n";
					out += "\t\t Center: " + glmvec3tostring(bd.box.m_center) + "\n";
					out += "\t\t Size: " + glmvec3tostring(bd.box.m_size) + "\n";
					out += "\t\t Rotation: " + glmquattostring(bd.box.m_rotation) + "\n";
				}
				else
				{
					out += "\t Bounding Sphere: \n";
					out += "\t\t Center: " + glmvec3tostring(bd.sphere.m_center) + "\n";
					out += "\t\t Radius: " + std::to_string(bd.sphere.m_radius) + "\n";
				}
				out += "\n\n";
			}
		}
		return out;
	}

	static std::string allMeshObjectsAsString(ISimpleContentModule_API& content, bool withproperties = false, bool extended=false)
	{
		std::string out = "";

		for (auto mobs : content.getMeshedObjects())
		{
			out += "Id: " + std::to_string(mobs.m_meshObjectId) + ": " + std::to_string(mobs.m_meshes.size()) + " meshes\n";
			if (withproperties)
			{
				for (auto m : mobs.m_meshes)
				{
					out += "    MeshId: " + std::to_string(m->m_meshId) + "\n";
					out += "\t Indices: " + std::to_string(m->m_indices.size()) + "\n";
					if (extended)
					{
						for (auto i : m->m_indices)
						{
							out += "\t\t  " + std::to_string(i) + "\n";
						}
					}
					out += "\t Vertices: " + std::to_string(m->m_vertices.getData().size()) + "\n";
					if (extended)
					{
						for (auto v : m->m_vertices.getData())
						{
							out += "\t\t Position:" + glmvec3tostring(v.m_position);
							out += "\t\t UVs:" + glmvec2tostring(v.m_uv);
							out += "\t\t Normal:" + glmvec3tostring(v.m_normal);
							out += "\t\t Tangent:" + glmvec3tostring(v.m_tangent);
							out += "\t\t----\n";
						}
					}
				}
			}
		}
		return out;
	}
	
}

#endif