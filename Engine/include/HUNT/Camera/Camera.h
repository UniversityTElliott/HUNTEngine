//style and ordering taken from shaders project

#include <HUNT/HUNTMath/float3.h>
#include <HUNT/HUNTMath/float4.h>
#include <HUNT/HUNTMath/mat4.h>
namespace HUNT {
	class Camera
	{
	public:
		Camera();	/// Initialised default camera object
		~Camera();

		void update();

		void setPosition(float xIn, float yIn, float zIn);		//Set camera position directly
		void setPosition(HUNT::float3 posIn);		//Set camera position directly
		void setRotation(float xIn, float yIn, float zIn);		//Set camera rotation directly

		HUNT::float3 getPosition();		//Get camera's current position
		HUNT::float3 getRotation();		// Get camera's current rotation
		void rotate(float pitchDelta, float yawDelta);
		HUNT::float3 getForward();
		HUNT::float3 getRight();

		//Movement
		void moveForward(float amount);
		void moveBackward(float amount);
		void moveLeft(float amount);
		void moveRight(float amount);

		HUNT::mat4 getViewMatrix();
		HUNT::mat4 getProjectionMatrix();

	private:
		HUNT::float3 position;		//float3 for position
		HUNT::float3 rotation;		// float3 for rotation (angles)
		HUNT::float3 forward = HUNT::float3(0.f, 0.f, 1.f);
		HUNT::float3 right = HUNT::float3(1.f, 0.f, 0.f);
		HUNT::float3 up = HUNT::float3(0.f, 1.f, 0.f);
		HUNT::mat4 viewMatrix;	// matrix for current view
		HUNT::mat4 orthoMatrix;	// current orthographic matrix
		float speed;	// movement speed and time variables
		float lookSpeed;		// rotation speed

	};
}