#pragma once



//ALL THE JOLT EXTRAS
//JPH_SUPPRESS_WARNINGS


#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterMask.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

//Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>

#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyLock.h>
#include <cstdint>
//Collision layer spaces.
namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer PLAYER = 2;
	static constexpr JPH::ObjectLayer WORLD = 3;
	static constexpr JPH::ObjectLayer TRIGGER = 4;
	static constexpr JPH::ObjectLayer RESPAWN = 5;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 6;
};

//Class that determines if two object layers can collide
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
	{
		switch (inObject1)
		{
		case Layers::PLAYER: //Player interacts with everything
			return true;

		case Layers::WORLD: //World interacts with anything thats set as moving above
			return inObject2 == Layers::MOVING ||
				inObject2 == Layers::PLAYER;

		case Layers::MOVING:
			return true; // Moving collides with everything

		case Layers::NON_MOVING: //Non moving interacts wiht moving and player
			return inObject2 == Layers::MOVING ||
				inObject2 == Layers::PLAYER;
		case Layers::TRIGGER:
			return inObject2 == Layers::PLAYER;; // Moving collides with everything
		default:
			return false;
		}
	}
};


namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr JPH::BroadPhaseLayer TRIGGER(2);
	static constexpr JPH::uint NUM_LAYERS(3);
};


class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
	BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::WORLD] = BroadPhaseLayers::NON_MOVING; // static
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::TRIGGER] = BroadPhaseLayers::NON_MOVING;

		mObjectToBroadPhase[Layers::PLAYER] = BroadPhaseLayers::MOVING;     // dynamic
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual JPH::uint GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

private:
	JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};



class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
	{

		//WIDE SWEEP esentially
		//Nonmoving and world only deal with moving
		//Moving and player interact with all
		switch (inLayer1)
		{
		case Layers::NON_MOVING:
		case Layers::WORLD:
		case Layers::RESPAWN:
			return inLayer2 == BroadPhaseLayers::MOVING;

		case Layers::MOVING:
		case Layers::PLAYER:
			return inLayer2 == BroadPhaseLayers::MOVING or inLayer2 == BroadPhaseLayers::NON_MOVING;

		default:
			return false;
		}
	}
};

class MyContactListener : public JPH::ContactListener
{
public:

	std::atomic<bool> scene_change_requested = false;

	virtual void OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
	{
		auto layer1 = inBody1.GetObjectLayer();
		auto layer2 = inBody2.GetObjectLayer();

		if ((layer1 == Layers::PLAYER && layer2 == Layers::WORLD) ||
			(layer1 == Layers::WORLD && layer2 == Layers::PLAYER))
		{
		}

	}

	virtual void OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings) override
	{

		auto layer1 = inBody1.GetObjectLayer();
		auto layer2 = inBody2.GetObjectLayer();
	}

	virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override
	{
		
	}
};


