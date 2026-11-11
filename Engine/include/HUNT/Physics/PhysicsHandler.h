
#pragma once
#include "JoltBackend.h"
#include <vector>
#include <memory>

class PhysicsWorld;

//New Handler, removed clunky structs filling old JPHYS now split into a proper handler and world
class PhysicsHandler
{
public:
	PhysicsHandler();
	~PhysicsHandler();
	void Init();
	void Reset();
	void FixedUpdate();

	void SetActiveWorld(PhysicsWorld* world);
	PhysicsWorld* GetActiveWorld() const;


	JPH::TempAllocator& GetTempAllocator() const;
	JPH::JobSystemThreadPool& GetJobSystem() const;



private:

	PhysicsWorld* activeWorld = nullptr;

	std::unique_ptr<JPH::TempAllocator> tempAllocator;
	std::unique_ptr<JPH::JobSystemThreadPool> jobSystem;
};
