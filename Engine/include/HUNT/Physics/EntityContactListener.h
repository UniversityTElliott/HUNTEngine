
#pragma once

#include "JoltBackend.h"
#include "HUNT/Objects/HUNTObject.h"

#include <vector>
#include <memory>
#include <functional>
//You HAVE to make your own contact listener. Before it was just thrown in JPHYS but with my new scenebased arcitect it need to be its own class :(
class EntityContactListener final : public JPH::ContactListener
{
public:
	EntityContactListener() = default;
	~EntityContactListener() = default;

	void Init(std::vector<std::unique_ptr<HUNT::HUNTObject>>* objects);
	void Unregister();

	void ProcessEvents();

	HUNT::HUNTObject* FindObjectByID(uint64_t objectID);

	virtual void OnContactAdded(const JPH::Body& inBody1,const JPH::Body& inBody2,const JPH::ContactManifold& inManifold,JPH::ContactSettings& ioSettings) override;

	virtual void OnContactPersisted(const JPH::Body& inBody1,const JPH::Body& inBody2,const JPH::ContactManifold& inManifold,JPH::ContactSettings& ioSettings) override;

	virtual void OnContactRemoved(const JPH::SubShapeIDPair& inSubShapePair) override;

private:
	std::vector<std::unique_ptr<HUNT::HUNTObject>>* sceneObjects = nullptr;
	std::vector<std::function<void()>> sceneContacts;
};
