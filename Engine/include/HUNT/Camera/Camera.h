//style and ordering taken from shaders project

#include <HUNT/HUNTMath/float3.h>
#include <HUNT/HUNTMath/float4.h>
#include <HUNT/HUNTMath/mat4.h>

class Camera
{
public:
	Camera();	///< Initialised default camera object
	~Camera();

	void update();

	void setPosition(float xIn, float yIn, float zIn);		///< Set camera position directly
	void setRotation(float xIn, float yIn, float zIn);		///< Set camera rotation directly

	HUNT::float3 getPosition();		///< Get camera's current position
	HUNT::float3 getRotation();		///< Get camera's current rotation

	

private:
	HUNT::float3 position;		///< float3 for position
	HUNT::float3 rotation;		///< float3 for rotation (angles)
	HUNT::mat4 viewMatrix;	///< matrix for current view
	HUNT::mat4 orthoMatrix;	///< current orthographic matrix
	float speed, frameTime;	///< movement speed and time variables
	float lookSpeed;		///< rotation speed
};