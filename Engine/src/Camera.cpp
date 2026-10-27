
#include <HUNT/Camera/Camera.h>
#include <HUNT/HUNTMath/mathFunctions.h>
namespace HUNT
{
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
		orthoMatrix = HUNT::lookAtLH(position, lookAt, up);
	}

	Camera::~Camera() = default;

	void Camera::update() {

		//Generic camera implementaion based on CMP203 when we had to do it for a weekly task
		// Convert Euler angles from degrees to radians.
		float pitch = glm::radians(rotation.x);
		float yaw = glm::radians(rotation.y);

		// Calculate the forward direction for a left-handed camera.
		forward = HUNT::float3(cosf(pitch) * sinf(yaw),sinf(pitch),cosf(pitch) * cosf(yaw));
		//maybe let users change this but for now up is Y+
		up = HUNT::float3(0.f, 1.f, 0.f);
		right = HUNT::normalise(HUNT::cross(up,forward));

		HUNT::float3 centre = HUNT::float3(position.x + forward.x, position.y + forward.y, position.z + forward.z);
		
		viewMatrix = HUNT::lookAtLH(position, centre, up);
	}

	void Camera::rotate(float yawDelta, float pitchDelta) //X then Y
	{
		rotation.x += pitchDelta;
		rotation.y += yawDelta;

		// Prevent flipping when looking straight up or down.
		rotation.x = glm::clamp(rotation.x, -89.0f, 89.0f);

		update();
	}

	void Camera::setPosition(float xIn, float yIn, float zIn)
	{
		position = HUNT::float3(xIn, yIn, zIn);
		update();
	}
	void Camera::setPosition(HUNT::float3 positionIn)
	{
		position = positionIn;
		update();
	}
	//probs wont use this? player might
	void Camera::setRotation(float xIn, float yIn, float zIn)
	{
		rotation = HUNT::float3(xIn, yIn, zIn);
		update();
	}

	void Camera::moveForward(float amount)
	{
		
		position.x += forward.x * amount;
		position.z += forward.z * amount;
		update();
	}

	void Camera::moveBackward(float amount)
	{
		moveForward(-amount);
	}

	void Camera::moveLeft(float amount)
	{
		moveRight(-amount);
	}

	void Camera::moveRight(float amount)
	{
		position.x += right.x * amount;
		position.z += right.z * amount;
		update();
	}

	HUNT::float3 Camera::getPosition()
	{
		return position;
	}

	HUNT::float3 Camera::getRotation()
	{
		return rotation;
	}

	HUNT::float3 Camera::getForward()
	{
		return forward;
	}

	HUNT::float3 Camera::getRight()
	{
		return right;
	}

	HUNT::mat4 Camera::getViewMatrix()
	{
		return viewMatrix;
	}
}