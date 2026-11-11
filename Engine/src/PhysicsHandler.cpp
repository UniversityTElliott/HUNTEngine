#include "HUNT/Physics/PhysicsHandler.h"
#include "HUNT/Physics/PhysicsWorld.h"

PhysicsHandler::PhysicsHandler() = default;

PhysicsHandler::~PhysicsHandler()
{
    Reset();
}


void PhysicsHandler::Init()
{

    JPH::RegisterDefaultAllocator();
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(25 * 1024 * 1024);

    jobSystem = std::make_unique<JPH::JobSystemThreadPool>(
        JPH::cMaxPhysicsJobs,
        JPH::cMaxPhysicsBarriers,
        std::thread::hardware_concurrency() - 1
    );
}


void PhysicsHandler::Reset()
{
    activeWorld = nullptr;
    tempAllocator.reset();
    jobSystem.reset();

    JPH::UnregisterTypes();

    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
}

void PhysicsHandler::FixedUpdate()
{
    if (activeWorld)
    {
        activeWorld->FixedUpdate(*tempAllocator,*jobSystem);
    }
}
void PhysicsHandler::SetActiveWorld(PhysicsWorld* world)
{
    if (activeWorld != world) {
        activeWorld = world;
    }
}

PhysicsWorld* PhysicsHandler::GetActiveWorld() const
{
    return activeWorld;
}

JPH::TempAllocator& PhysicsHandler::GetTempAllocator() const
{
    return *tempAllocator;
}

JPH::JobSystemThreadPool& PhysicsHandler::GetJobSystem() const
{
    return *jobSystem;
}








