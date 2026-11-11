
#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include<HUNT/Math/float3.h>
#include<HUNT/Math/quat.h>
#include <HUNT/Objects/Components/HUNTBaseComponent.h>
#include <HUNT/Objects/Components/HUNTTransformComponent.h>

namespace HUNT {
	class HUNTRigidBodyComponent : public HUNTBaseComponent
	{
	public:
		HUNTRigidBodyComponent() = default;
		~HUNTRigidBodyComponent() = default;

		JPH::BodyID GetBodyID() const {
			return bodyID;
		}

		void SetBodyID(JPH::BodyID id) {
			bodyID = id;
			isInitialised = true;
		}

		bool IsInitialised() const {
			return isInitialised;
		}

		void SetTransform(HUNT::HUNTTransformComponent* transformIn) {
			transform = transformIn;
			currentPosition = transform->position;
		}

		HUNT::HUNTTransformComponent* GetTransform() const {
			return transform;
		}

		HUNT::float3 previousPosition{};
		HUNT::quat previousRotation{};
		HUNT::float3 currentPosition{};
		HUNT::quat currentRotation{};

	private:
		//Jolt ID
		JPH::BodyID bodyID;
		//"Front end" transform, will be manipulated by the physics system 
		HUNTTransformComponent* transform = nullptr;
		bool isInitialised = false;
	};
}