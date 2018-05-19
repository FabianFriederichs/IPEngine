#ifndef CAMERA_H
#define CAMERA_H
#include <vector>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
			updateProj();
		}
		return *this;
	}

	void getViewProjMatrix(glm::mat4 &V, glm::mat4 &P) const;
	glm::mat4 getView() { return viewMat; }
	glm::mat4 getProj() { return projMat; }
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

	void setAspectRatio(float aspect) { aspectRatio = aspect; updateProj(); }
	void setFov(float fov) { fovy = fov; updateProj();}
	void setZnear(float near, float far) { zNear = near; zFar = far; updateProj(); updateSegments(); }
	void setPosition(glm::vec3 pos) { position = pos; }
	void setLookAt(glm::vec3 pos) { lookAtPos = pos; }
	void forceViewMat(glm::mat4 view) { viewMat = view; }
protected:
	const float thetaLimit;
	const float minDistance;

	float fovy;
	float aspectRatio;
	float zNear, zFar;

	glm::vec3 position;
	glm::vec3 lookAtPos;
	glm::vec2 phiTheta; // azimuth and zenith angles
	glm::mat4 viewMat;
	glm::mat4 projMat;
	uint32_t segmentCount;
	float farPlaneZs[CSM_MAX_SEG_COUNT]; // in camera view space
	float normFarPlaneZs[CSM_MAX_SEG_COUNT];

	void updateProj()
	{
		projMat = glm::perspective(fovy, aspectRatio, zNear, zFar);
		projMat[1][1] *= -1.f; // the y-axis of clip space in Vulkan is pointing down in contrary to OpenGL
	}

	void updateSegments()
	{
		const float lambda = 0.5f;
		glm::mat4& P = projMat;

		for (uint32_t i = 1; i <= segmentCount; ++i)
		{
			float frac = float(i) / segmentCount;
			float logSplit = zNear * std::pow(zFar / zNear, frac);
			float uniSplit = zNear + (zFar - zNear) * frac;
			float splitDepth = (1.f - lambda) * uniSplit + lambda * logSplit;
			splitDepth = fmax(zNear, fmin(zFar, splitDepth));

			farPlaneZs[i - 1] = -splitDepth;
			float projectedDepth = (P[2][2] * -splitDepth + P[3][2]) / (P[2][3] * -splitDepth);
			projectedDepth = fmax(0.f, fmin(1.f, projectedDepth));
			normFarPlaneZs[i - 1] = projectedDepth;
		}
	}
};

#endif