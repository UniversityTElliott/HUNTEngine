#pragma once
#include "JoltBackend.h"
#include "HUNT/Objects/HUNTObject.h"
#include "HUNT/Objects/Components/HUNTRigidBodyComponent.h"
#include "EntityContactListener.h"
#include <memory>
#include <vector>


class PhysicsWorld
{
public:
    PhysicsWorld() = default;
    void Init();
    void Reset();
    void FixedUpdate(JPH::TempAllocator& tempIn, JPH::JobSystemThreadPool& jobIn);
    void ClearBodies();
    void SyncTransforms();
    void InterpolateBodies(JPH::TempAllocator& tempIn, JPH::JobSystemThreadPool& jobIn);
    void UpdateSceneObjects(std::vector<std::unique_ptr<HUNT::HUNTObject>>* objects);
    JPH::BodyID CreateBox(HUNT::HUNTRigidBodyComponent* rb,HUNT::float3 position,HUNT::float3 halfExtent,JPH::EMotionType motionType);

    JPH::PhysicsSystem* GetPhysicsSystem()
    {
        return physicsSystem.get();
    }

    JPH::BodyInterface& GetBodyInterface()
    {
        return physicsSystem->GetBodyInterface();
    }

private:
    std::vector<std::unique_ptr<HUNT::HUNTObject>>* sceneObjects = nullptr;

    std::unique_ptr<JPH::PhysicsSystem> physicsSystem;

    BPLayerInterfaceImpl broadPhaseLayerInterface;
    ObjectVsBroadPhaseLayerFilterImpl objectVsBroadPhaseLayerFilter;
    ObjectLayerPairFilterImpl objectLayerPairFilter;

    EntityContactListener contactListener;

 
    float fixedStep = 1.f / 120.f; //CHANGE MAYBE EVEN LET ENGINE DECIDE
    float lerpValue = 0;
};

