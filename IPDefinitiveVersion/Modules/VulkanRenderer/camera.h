#ifndef CAMERA_H
#define CAMERA_H
#include <vector>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#define CSM_MAX_SEG_COUNT 4

class Camera
{
public:
	Camera(
		const glm::vec3 position,
		const glm::vec3 lookAtPos,
		float fovy, float aspect,
		float zNear, float zFar,
		uint32_t segCount = 3);

	Camera& operator=(const Camera& other)
	{
		if (this != &other)
		{
			fovy = other.fovy;
			aspectRatio = other.aspectRatio;
			zNear = other.zNear;
			zFar = other.zFar;
			position = other.position;
			lookAtPos = other.lookAtPos;
			phiTheta = other.phiTheta;
			segmentCount = other.segmentCount;

		}
		return *this;
	}

	void getViewProjMatrix(glm::mat4 &V, glm::mat4 &P) const;
	
	float getFovy() const { return fovy; }
	float getZNear() const { return zNear; }
	float getZFar() const { return zFar; }
	glm::vec3 getLookAt() const { return lookAtPos; }
	float getAspectRatio() const { return aspectRatio; }
	const glm::vec3 &getPosition() const { return position; }
	uint32_t getSegmentCount() const { return segmentCount; }
	float getNormFarPlaneZ(uint32_t segIdx) const { return normFarPlaneZs[segIdx]; }
	void getSegmentDepths(std::vector<float> *segDepths) const;
	// On return, @corners contain (segmentCount * 4 + 4) points because
	// near plane corners are the far plane corners of the previous segment
	void getCornersWorldSpace(std::vector<glm::vec3> *corners) const;

	void addRotation(float phi, float theta);
	void addPan(float x, float y);
	void addZoom(float d);

	void setAspectRatio(float aspect) { aspectRatio = aspect; }

protected:
	const float thetaLimit;
	const float minDistance;

	float fovy;
	float aspectRatio;
	float zNear, zFar;

	glm::vec3 position;
	glm::vec3 lookAtPos;
	glm::vec2 phiTheta; // azimuth and zenith angles

	uint32_t segmentCount;
	float farPlaneZs[CSM_MAX_SEG_COUNT]; // in camera view space
	float normFarPlaneZs[CSM_MAX_SEG_COUNT];
};

#endif