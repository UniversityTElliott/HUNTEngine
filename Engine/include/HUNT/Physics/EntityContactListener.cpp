#include "EntityContactListener.h"
#include "HUNT/Objects/Components/HUNTPhysicsComponents.h"
/*class EntityContactListener : public JPH::ContactListener
{

void EntityContactListener::prosesEvents()
{
    for (const auto& callback : m_contacts)
        callback(); // Call All pending contact Events
    m_contacts.clear();
}
void EntityContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
    if (m_registry)
    {
        const auto entityID_1 = static_cast<entt::entity>(inBody1.GetUserData());
        const auto entityID_2 = static_cast<entt::entity>(inBody2.GetUserData());

        // Differ Contact Events until next physic Step

        if(const auto callback = m_registry->try_get<EnterResponseComponent>(entityID_1))
        {
            m_contacts.emplace_back([=]{
            Skateboard::Entity entity{entityID_2,m_registry};
            callback->m_callback(entity); });
        }

        if(const auto callback = m_registry->try_get<EnterResponseComponent>(entityID_2))
        {
            m_contacts.emplace_back([=](){

                Skateboard::Entity entity(entityID_1,m_registry);
                callback->m_callback(entity);
                });
        }
    }
}

void EntityContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2,
    const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
    if (m_registry)
    {
        const auto entityID_1 = static_cast<entt::entity>(inBody1.GetUserData());
        const auto entityID_2 = static_cast<entt::entity>(inBody2.GetUserData());

        // Differ Contact Events until next physic Step

        if(const auto callback = m_registry->try_get<OverlapResponseComponent>(entityID_1))
        {
            m_contacts.emplace_back([=] {
                Skateboard::Entity entity{ entityID_2,m_registry };
                callback->m_callback(entity); });
        }

        if(const auto callback = m_registry->try_get<OverlapResponseComponent>(entityID_2))
        {
            m_contacts.emplace_back([=] {
                Skateboard::Entity entity{ entityID_1,m_registry };
                callback->m_callback(entity); });
        }
    }
}

void EntityContactListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
{
	const JPH::BodyLockInterfaceLocking& lock_interface = JPhys::GetPhysicsSystem()->GetBodyLockInterface();
	if (m_registry)
	{
		const JPH::Body* body1 = lock_interface.TryGetBody(inSubShapePair.GetBody1ID());
		const JPH::Body* body2 = lock_interface.TryGetBody(inSubShapePair.GetBody2ID());
		if (!body1 || !body2) return;

		const auto entityID_1 = static_cast<entt::entity>(body1->GetUserData());
		const auto entityID_2 = static_cast<entt::entity>(body2->GetUserData());

		// Differ Contact Events until next physic Step

		if (const auto callback = m_registry->try_get<ExitResponseComponent>(entityID_1))
		{
			m_contacts.emplace_back([=] {
				Skateboard::Entity entity{ entityID_2,m_registry };
				callback->m_callback(entity); });
		}

		if (const auto callback = m_registry->try_get<ExitResponseComponent>(entityID_2))
		{
			m_contacts.emplace_back([=] {
				Skateboard::Entity entity{ entityID_1,m_registry };
				callback->m_callback(entity); });
		}
	}
}*/




//Same as Handler
//https://github.com/jrouwe/JoltPhysics/blob/master/HelloWorld/HelloWorld.cpp had to relook at this so will quote it

void EntityContactListener::Init(std::vector<std::unique_ptr<HUNT::HUNTObject>>* objects)
{
    sceneObjects = objects;
}

void EntityContactListener::Unregister()
{
    sceneObjects->empty();
    sceneContacts.empty();
}

void EntityContactListener::ProcessEvents()
{
    for (const auto& callback : sceneContacts)
    callback(); // Call All pending contact Events
    sceneContacts.clear();
}

HUNT::HUNTObject* EntityContactListener::FindObjectByID(uint64_t objectID)
{
    for (const std::unique_ptr<HUNT::HUNTObject>& object : *sceneObjects)
    {
        //Check object exists and return it based on ID
        if (object && object->GetID() == objectID) {
            return object.get();
        }
    }

    return nullptr;
}

void EntityContactListener::OnContactAdded(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
    if (sceneObjects)
    {
        HUNT::HUNTObject* object1 = FindObjectByID(inBody1.GetUserData());
        HUNT::HUNTObject* object2 = FindObjectByID(inBody2.GetUserData());
        //Ensure both arent null
        if (!object1 || !object2) {
            return;
        }

        const auto objectID_1 = inBody1.GetUserData();
        const auto objectID_2 = inBody2.GetUserData();

        //Defer Contact Events until next physic Step
        if (const auto* callback = object1->GetComponent<HUNT::EnterResponseComponent>())
        {
            auto callbackFunction = callback->m_callback;

            sceneContacts.emplace_back([this, callbackFunction, objectID_2]()
                {
                    if (HUNT::HUNTObject* other = FindObjectByID(objectID_2))
                    {
                        callbackFunction(*other);
                    }
                });
        }

        if (const auto* callback = object2->GetComponent<HUNT::EnterResponseComponent>())
        {
            auto callbackFunction = callback->m_callback;

            sceneContacts.emplace_back([this, callbackFunction, objectID_1]()
                {
                    if (HUNT::HUNTObject* other = FindObjectByID(objectID_1))
                    {
                        callbackFunction(*other);
                    }
                });
        }
    }
}

void EntityContactListener::OnContactPersisted(const JPH::Body& inBody1, const JPH::Body& inBody2, const JPH::ContactManifold& inManifold, JPH::ContactSettings& ioSettings)
{
    if (sceneObjects)
    {
 
        HUNT::HUNTObject* object1 = FindObjectByID(inBody1.GetUserData());
        HUNT::HUNTObject* object2 = FindObjectByID(inBody2.GetUserData());

        //Ensure both arent null
        if (!object1 || !object2) {
            return;
        }

        const auto objectID_1 = inBody1.GetUserData();
        const auto objectID_2 = inBody2.GetUserData();
        //Defer Contact Events until next physic Step
        if (const auto* callback = object1->GetComponent<HUNT::OverlapResponseComponent>())
        {
            auto callbackFunction = callback->m_callback;

            sceneContacts.emplace_back([this, callbackFunction, objectID_2]()
                {
                    if (HUNT::HUNTObject* other = FindObjectByID(objectID_2))
                    {
                        callbackFunction(*other);
                    }
                });
        }

        if (const auto* callback = object2->GetComponent<HUNT::OverlapResponseComponent>())
        {
            auto callbackFunction = callback->m_callback;

            sceneContacts.emplace_back([this, callbackFunction, objectID_1]()
                {
                    if (HUNT::HUNTObject* other = FindObjectByID(objectID_1))
                    {
                        callbackFunction(*other);
                    }
                });
        }
    }
}

void EntityContactListener::OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair)
{
  
    if (sceneObjects)
    {
        const JPH::BodyID bodyID_1 = inSubShapePair.GetBody1ID();
        const JPH::BodyID bodyID_2 = inSubShapePair.GetBody2ID();
      
        const uint64_t objectID_1 = bodyID_1.GetIndexAndSequenceNumber();
        const uint64_t objectID_2 = bodyID_2.GetIndexAndSequenceNumber();

        HUNT::HUNTObject* object1 = FindObjectByID(objectID_1);
        HUNT::HUNTObject* object2 = FindObjectByID(objectID_2);

        //Ensure both arent null
        if (!object1 || !object2) {
            return;
        }
        // Differ Contact Events until next physic Step

        if (const auto* callback = object1->GetComponent<HUNT::ExitResponseComponent>())
        {
            auto callbackFunction = callback->m_callback;

            sceneContacts.emplace_back([this, callbackFunction, objectID_2]()
                {
                    if (HUNT::HUNTObject* other = FindObjectByID(objectID_2))
                    {
                        callbackFunction(*other);
                    }
                });
        }

        if (const auto* callback = object2->GetComponent<HUNT::ExitResponseComponent>())
        {
            auto callbackFunction = callback->m_callback;

            sceneContacts.emplace_back([this, callbackFunction, objectID_1]()
                {
                    if (HUNT::HUNTObject* other = FindObjectByID(objectID_1))
                    {
                        callbackFunction(*other);
                    }
                });
        }
    }
    

}
