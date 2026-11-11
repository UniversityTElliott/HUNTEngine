#include "PhysicsWorld.h"
#include "PhysicsHandler.h"
#include "HUNT/Objects/Components/HUNTRigidBodyComponent.h"
#include "HUNT/Math/mathFunctions.h"



void PhysicsWorld::Init()
{
    physicsSystem = std::make_unique<JPH::PhysicsSystem>();

    physicsSystem->Init(
       1024,//Bodies
       0,//Mutex
       1024,//Paiur
       1024,//constraint
       broadPhaseLayerInterface,
       objectVsBroadPhaseLayerFilter,
       objectLayerPairFilter
   );
    //Init Contact Listener with the objects
    contactListener.Init(sceneObjects);
    //set it with jolt
    physicsSystem->SetContactListener(&contactListener);

}

void PhysicsWorld::Reset() {

    //Clear allbodies
    ClearBodies();
    //Reset JOLT stuff
    if (physicsSystem)
    {
        physicsSystem->SetContactListener(nullptr);
        physicsSystem.reset();
    }
    //empty objects
    sceneObjects = nullptr;


}
void PhysicsWorld::FixedUpdate(JPH::TempAllocator& tempIn, JPH::JobSystemThreadPool& jobIn) {

    if (!sceneObjects) {
        return;
    }
    
    InterpolateBodies(tempIn,jobIn);
    contactListener.ProcessEvents();
    SyncTransforms();

}

void PhysicsWorld::ClearBodies()
{
    auto& bodyInterface = physicsSystem->GetBodyInterface();
    
       
        for (const auto& entity : *sceneObjects)
        {
            auto rb = entity.get()->GetComponent<HUNT::HUNTRigidBodyComponent>();

            if (rb) {

            bodyInterface.RemoveBody(rb->GetBodyID());
            bodyInterface.DestroyBody(rb->GetBodyID());
            }
    
        }
        physicsSystem->OptimizeBroadPhase();
}

void PhysicsWorld::SyncTransforms()
{
    for (const auto& entity : *sceneObjects)
    {
        auto rb = entity.get()->GetComponent<HUNT::HUNTRigidBodyComponent>();
        if (rb) {
            auto trans = entity.get()->GetComponent<HUNT::HUNTTransformComponent>();
            if (trans) {
                HUNT::float3 position = HUNT::vectorLerp(rb->previousPosition, rb->currentPosition, lerpValue);
                HUNT::quat rotation = HUNT::quaternionSlerp(rb->previousRotation, rb->currentRotation, lerpValue);

                trans->position = position;
                trans->rotation = rotation;
            }
         }

    }
}

void PhysicsWorld::InterpolateBodies(JPH::TempAllocator& tempIn, JPH::JobSystemThreadPool& jobIn)
{

            auto& lockingInterface = physicsSystem->GetBodyLockInterface();

            constexpr int collisionSteps = 1;
            //GET PREVIOUSPOSITION

            for (const auto& entity : *sceneObjects)
            {
                auto rb = entity.get()->GetComponent<HUNT::HUNTRigidBodyComponent>();
                if (rb) {

                    rb->previousPosition = rb->currentPosition;
                    rb->previousRotation = rb->currentRotation;

                }
            }
            //RUN THE PHYSICS UPDATE
            physicsSystem->Update(
                fixedStep,
                collisionSteps,
                &tempIn,
                &jobIn
            );
    
            //GET NEW POSITION
            for (const auto& entity : *sceneObjects)
            {
                auto rb = entity.get()->GetComponent<HUNT::HUNTRigidBodyComponent>();
                if (rb) {
                    JPH::BodyLockRead lock(lockingInterface, rb->GetBodyID());
                    if (!lock.Succeeded()) continue;

                    const JPH::Body& body = lock.GetBody();
                    HUNT::float3 newPos = HUNT::float3(body.GetPosition().GetX(), body.GetPosition().GetY(), body.GetPosition().GetZ());
                    HUNT::quat newRot = HUNT::quat(body.GetRotation().GetW(),body.GetRotation().GetX(), body.GetRotation().GetY(), body.GetRotation().GetZ());
                    rb->currentPosition = newPos;
                    rb->currentRotation = newRot;

                }
            }
}

void PhysicsWorld::UpdateSceneObjects(std::vector<std::unique_ptr<HUNT::HUNTObject>>* objects)
{
    sceneObjects = objects;
}

JPH::BodyID PhysicsWorld::CreateBox(HUNT::HUNTRigidBodyComponent* rb, HUNT::float3 position, HUNT::float3 halfExtent, JPH::EMotionType motionType)
{
    if (!physicsSystem || !rb)
        return JPH::BodyID();

    auto& bodyInterface = physicsSystem->GetBodyInterface();

    //add collision shape
    JPH::ShapeRefC shape =new JPH::BoxShape(JPH::Vec3(halfExtent.x,halfExtent.y,halfExtent.z));

    //and body settings
    JPH::ObjectLayer objectLayer;

    if (motionType == JPH::EMotionType::Static)
    {
        objectLayer = Layers::NON_MOVING;
    }
    else
    {
        objectLayer = Layers::MOVING;
    }

    JPH::BodyCreationSettings settings(shape,JPH::RVec3(position.x, position.y, position.z),JPH::Quat::sIdentity(),motionType,objectLayer);
    //create and add to interface
    JPH::BodyID bodyID = bodyInterface.CreateAndAddBody(settings, JPH::EActivation::Activate);

    //set the rb id
    rb->SetBodyID(bodyID);

    //set positions for smoothness later
    rb->previousPosition = position;
    rb->currentPosition = position;

    return bodyID;
}

