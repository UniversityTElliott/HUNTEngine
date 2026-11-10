#pragma once

#include "JoltBackend.h"
#include "HUNT/Objects/HUNTObject.h"
/// @brief JOLT contact listener for ENTT Contact Response Components 
class EntityContactListener final : public JPH::ContactListener
{
	std::vector<std::unique_ptr<HUNT::HUNTObject>>* m_objects;

public:
	EntityContactListener() : m_objects(nullptr) {}
	void Register(std::vector<std::unique_ptr<HUNT::HUNTObject>>* objects) { m_objects = objects; }
	void unregister() { m_objects = nullptr; }

	void prosesEvents();

	virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

	virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override;

	virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;
};

//STARTED PHYSICS

/// @brief Jolt physics Wrapper
class PhysicsHandler
{
	PhysicsHandler();

	entt::registry* m_registry;

	// Jolt physics systems
	std::unique_ptr<JPH::PhysicsSystem>	      m_physicsSystem;
	std::unique_ptr<JPH::TempAllocator>		  m_tempAllocator; 
	std::unique_ptr<JPH::JobSystemThreadPool> m_jobSystem;

	BPLayerInterfaceImpl m_broadPhaseLayerInterface;
	ObjectVsBroadPhaseLayerFilterImpl m_objectVsBroadPhaseLayerFilter;
	ObjectLayerPairFilterImpl m_objectLayerPairFilter;

	EntityContactListener m_contactListener;

	//So the system is frame independent
	float accumulator;
	float fixedStep = 1.f / 120.f;
	float lerpValue;

	/// @brief Clear all Physics Body's in Registry 
	void ClearBodies() const;

	/// @brief Sync Transforms to the Transform Component
	void SyncTransforms() const;

	/// @brief Interpolate Physics Body's positioning for rendering
	void InterpolateBodies(Skateboard::TimeManager* time);

	/// @brief Update Physics Implementation
	void UpdateImpl(Skateboard::TimeManager* time);

public:
	~PhysicsHandler() = delete;
	DISABLE_COPY_AND_MOVE(PhysicsHandler);

	void Init();
	void Reset();

	/// @brief Link registry
	void Register(entt::registry* registry);
	/// @brief Unlink Registry
	void Unregister();

	static JPH::PhysicsSystem* GetPhysicsSystem() { return Singleton().m_physicsSystem.get(); }
	static JPH::BodyInterface& GetBodyInterface() { return Singleton().m_physicsSystem->GetBodyInterface(); }

	/// @brief Update Physics
	static void Update(Skateboard::TimeManager* time) { Singleton().UpdateImpl(time); }

	static JPH::Vec3 vectorLerp(JPH::Vec3 a, JPH::Vec3 b, float t);
};

/// @brief  JOLT Physics body Component 
struct RigidBody3D
{
	RigidBody3D(Skateboard::Entity entity, JPH::BodyCreationSettings& inSettings, JPH::EActivation inActivationMode = JPH::EActivation::Activate) :
		previousPosition{}, previousRotation{}, currentPosition{}, currentRotation{}
	{
		inSettings.mUserData = static_cast<uint32_t>(entity);
		body = JPhys::GetBodyInterface().CreateAndAddBody(inSettings, inActivationMode);
	}

	RigidBody3D(const JPH::BodyID id) : body{ id }, previousPosition{}, previousRotation{}, currentPosition{}, currentRotation{}
	{
	}

	JPH::RVec3 GetPosition() const
	{
		return JPhys::GetBodyInterface().GetPosition(body);
	}
	JPH::Quat GetRotation() const
	{
		return JPhys::GetBodyInterface().GetRotation(body);
	}
	void SetPosition(const JPH::Vec3 inPosition, const JPH::EActivation inActivationMode = JPH::EActivation::Activate) const
	{
		JPhys::GetBodyInterface().SetPosition(body, inPosition, inActivationMode);
	}
	void SetRotation(const JPH::Quat inQuat, const JPH::EActivation inActivationMode = JPH::EActivation::Activate) const {
		JPhys::GetBodyInterface().SetRotation(body, inQuat, inActivationMode);
	}

	JPH::BodyID body; // Body ID in JOLT

	//THIS IS FOR SMOOTH PHYSICS STUFF
	bool isInitialised = false;

	JPH::Vec3 previousPosition;
	JPH::Quat previousRotation;

	JPH::Vec3 currentPosition;
	JPH::Quat currentRotation;
};

/// @brief Multi Body Component, Used for when an object has multiple colliders I.E. The level Colliders
struct MultiRigidBody3D
{
	MultiRigidBody3D() = default;

	MultiRigidBody3D(Skateboard::Entity entity, std::vector<JPH::BodyCreationSettings>& inSettings, std::vector<JPH::EActivation> inActivationMode = {})
	{
		if (inActivationMode.size() < inSettings.size())
		{
			inActivationMode.resize(inSettings.size(), JPH::EActivation::Activate);
		}
		for (uint32_t i = 0; i < inSettings.size(); i++)
		{
			bodies.push_back(RigidBody3D{ entity, inSettings[i], inActivationMode[i] });
		}
	}

	MultiRigidBody3D(const std::vector<RigidBody3D>& ids) : bodies{ ids }
	{
	}
	std::vector<RigidBody3D> bodies;

};

/// @brief On Overlapping Collision Response Component
/// @note This is run each tick that 2 bodies are overlapping
struct OverlapResponseComponent
{
	explicit OverlapResponseComponent(const std::function<void(Skateboard::Entity& other)>& callback)
		: m_callback{ callback }
	{
	}
	std::function<void(Skateboard::Entity& other)> m_callback;
};

/// @brief On Enter Collision Response Component
/// @note This is only run once when an object enters another. This also includes object that are just touching.
struct EnterResponseComponent
{
	explicit EnterResponseComponent(const std::function<void(Skateboard::Entity& other)>& callback)
		: m_callback{ callback }
	{
	}
	std::function<void(Skateboard::Entity& other)> m_callback;
};

/// @brief On Exit Collision Response Component
/// @note This is only run once when 2 objects stop overlapping / touching.
struct ExitResponseComponent
{
	explicit ExitResponseComponent(const std::function<void(Skateboard::Entity& other)>& callback)
		: m_callback{ callback }
	{
	}
	std::function<void(Skateboard::Entity& other)> m_callback;
};