#include <HUNT/Camera/Camera.h>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
	position = HUNT::float3(0.f, 0.f, 0.f);
	rotation = HUNT::float3(0.f, 0.f, 0.f);

	lookSpeed = 4.0f;//default

	// Generate ortho matrix
	HUNT::float3 up, position, lookAt;
	up = HUNT::float3(0.0f, 1.0, 0.0);
	position = HUNT::float3(0.0f, 0.0, -10.0);
	lookAt = HUNT::float3(0.0f, 0.0f, 0.0f);
	orthoMatrix = glm::lookAtLH(position, lookAt, up);
}

Camera::~Camera() = default;

void Camera::update() {

	//oh em gee camera
}