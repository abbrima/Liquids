#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class FPSCamera
{
private:
	float yaw, pitch, sensitivity,CameraSpeed;
	glm::vec3 CameraDirection;
	glm::vec3 CameraPosition;
public:
	void Update(glm::mat4& view);
	void OnImGuiRender();

	inline glm::vec3 GetCameraPosition() const { return CameraPosition; }
	inline glm::vec3 GetCameraDirection() const { return CameraDirection; }
	inline void SetYaw(float val) { yaw = val; }
	inline void SetPitch(float val) { pitch = val; }
	inline void SetSensitivity(float val) { sensitivity = val; }
	inline void SetCameraSpeed(float val) { CameraSpeed = val; }
	inline void SetCameraPosition(float x, float y, float z) { CameraPosition = glm::vec3(x, y, z); }
	inline void SetCameraDirection(float x, float y, float z) { CameraDirection = glm::vec3(x, y, z); }
	inline float GetSensitivity() const { return sensitivity; }
	inline float GetCameraSpeed() const { return CameraSpeed; }
};