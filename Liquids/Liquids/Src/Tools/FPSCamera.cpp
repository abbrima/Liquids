#include "FPSCamera.h"
#include "imgui/imgui.h"
#include "MyExternals.h"


void FPSCamera::Update(glm::mat4& view)
{
	xOffset *= sensitivity;
	yOffset *= sensitivity;
	yaw += deltaTime  * xOffset;
	pitch += deltaTime  * yOffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	CameraDirection.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	CameraDirection.y = sin(glm::radians(pitch));
	CameraDirection.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	CameraDirection = glm::normalize(CameraDirection);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = (glm::cross(up, CameraDirection));
	glm::vec3 cameraUp = glm::cross(CameraDirection, cameraRight);

	if (keys[GLFW_KEY_A])
		CameraPosition -= glm::normalize(glm::cross(CameraDirection, cameraUp)) * deltaTime*CameraSpeed;
	if (keys[GLFW_KEY_D])
		CameraPosition += glm::normalize(glm::cross(CameraDirection, cameraUp)) * deltaTime*CameraSpeed;
	if (keys[GLFW_KEY_W])
		CameraPosition += deltaTime * CameraSpeed * CameraDirection;
	if (keys[GLFW_KEY_S])
		CameraPosition -= deltaTime * CameraSpeed * CameraDirection;
	if (keys[GLFW_KEY_SPACE])
		CameraPosition.y += deltaTime * CameraSpeed;
	if (keys[GLFW_KEY_LEFT_CONTROL])
		CameraPosition.y -= deltaTime * CameraSpeed;
	view = glm::lookAt(CameraPosition, CameraPosition + CameraDirection, cameraUp);
}
void FPSCamera::OnImGuiRender()
{
	ImGui::Text("YAW: %f\nPITCH:%f\nX:%f\tY:%f\nZ:%f", yaw, pitch, CameraPosition.x, CameraPosition.y, CameraPosition.z);
}