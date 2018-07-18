//Fizyka bryly sztywnej
/*#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <PxPhysicsAPI.h>

#define PVD_HOST "127.0.0.1"

#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")

using namespace std;
using namespace physx;

PxDefaultErrorCallback		defaultErrorCallback;
PxDefaultAllocator			defaultAllocator;
PxFoundation*				foundation = NULL;
PxPhysics*					physics = NULL;
PxScene*					scene = NULL;
PxReal						timestep = 1.0f / 60.0f;
PxVisualDebuggerConnection* connection = NULL;

void initPhysX()
{
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocator, defaultErrorCallback);
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());
	if (physics == NULL) {
		cerr << "Nie zainicjowano silnika gry!" << endl;
		exit(1);
	}
	if (physics->getPvdConnectionManager())
	{
		physics->getVisualDebugger()->setVisualizeConstraints(true);
		physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
		physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
		connection = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), PVD_HOST, 5425, 10);
	}
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDesc);
}

void shutdownPhysX()
{
	scene->release();
	if (connection != NULL) {
		connection->release();
	}
	physics->release();
	foundation->release();
}

void stepPhysx()
{
	scene->simulate(timestep);
	scene->fetchResults(true);
}

void addFloor()
{
	PxMaterial* material = physics->createMaterial(0.5, 0.5, 0.5);
	PxTransform position = PxTransform(
		PxVec3(0.0f, 0.0f, 0.0f),
		PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f))
		);
	PxRigidStatic* plane = physics->createRigidStatic(position);
	plane->createShape(PxPlaneGeometry(), *material);
	scene->addActor(*plane);
}

void addWalls()
{
	PxMaterial* material = physics->createMaterial(0.5, 0.5, 0.5);

	//Left wall
	PxTransform position = PxTransform(PxVec3(-0.635f, 0.0f, -1.27f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* wall = physics->createRigidStatic(position);
	wall->createShape(PxBoxGeometry(PxVec3(0.2f, 0.1f, 1.27f)), *material);
	scene->addActor(*wall);
	
	//Right wall
	position = PxTransform(PxVec3(0.635f, 0.0f, -1.27f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	wall = physics->createRigidStatic(position);
	wall->createShape(PxBoxGeometry(PxVec3(0.2f, 0.1f, 1.27f)), *material);
	scene->addActor(*wall);
	
	//Front wall
	position = PxTransform(PxVec3(0.0f, 0.0f, -2.54f)); //PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	wall = physics->createRigidStatic(position);
	wall->createShape(PxBoxGeometry(PxVec3(0.635f, 0.2f, 0.1f)), *material);
	scene->addActor(*wall);

	//Back wall
	position = PxTransform(PxVec3(0.0f, 0.0f, 0.07f)); //PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	wall = physics->createRigidStatic(position);
	wall->createShape(PxBoxGeometry(PxVec3(0.635f, 0.2f, 0.1f)), *material);
	scene->addActor(*wall);
}

void addBall(float _posZ, float _posX)
{
	PxMaterial* material = physics->createMaterial(0.5, 0.5, 0.5);
	PxTransform position = PxTransform(PxVec3(_posX, 0.0f, _posZ));
	PxRigidDynamic* ball = physics->createRigidDynamic(position);
	ball->createShape(PxSphereGeometry(0.057f), *material);
	ball->setMass(0.2f);
	scene->addActor(*ball);
	ball->addForce(PxVec3(0.0f, 0.0f, 0.0f), PxForceMode::eIMPULSE);
	ball->setLinearVelocity(PxVec3(0.0f, 0.0f, 0.0f));
}

void addMainBall()
{
	PxMaterial* material = physics->createMaterial(0.5, 0.5, 0.5);
	PxTransform position = PxTransform(PxVec3(0.0f, 0.0f, -0.15f));
	PxRigidDynamic* ball = physics->createRigidDynamic(position);
	ball->createShape(PxSphereGeometry(0.057f), *material);
	ball->setMass(0.2f);
	scene->addActor(*ball);
	ball->addForce(PxVec3(0.0f, 0.0f, -100.0f), PxForceMode::eIMPULSE);
	ball->setLinearVelocity(PxVec3(0.0f, 0.0f, 0.0f));
}

int main()
{
	initPhysX();
	addFloor();
	addWalls();
	float posZ = -2.0f;
	for (int z = 0; z < 2; z++)
	{
		float posX = -0.25f;
		for (int x = 0; x < 4; x++)
		{
			addBall(posZ, posX);
			posX += 0.12f;
		}
		posZ -= 0.12f;
	}
	addMainBall();
	for (int i = 0; i < 300; i++)
	{
		stepPhysx();
	}
	shutdownPhysX();
	system("pause");
	return 0;
}*/

//Kolizje
#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <PxPhysicsAPI.h>

#define PVD_HOST "127.0.0.1"

#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysXVisualDebuggerSDKDEBUG.lib")

using namespace std;
using namespace physx;

PxDefaultErrorCallback		defaultErrorCallback;
PxDefaultAllocator			defaultAllocator;
PxFoundation*				foundation = NULL;
PxPhysics*					physics = NULL;
PxScene*					scene = NULL;
PxReal						timestep = 1.0f / 60.0f;
PxVisualDebuggerConnection* connection = NULL;

void initPhysX()
{
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, defaultAllocator, defaultErrorCallback);
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());
	if (physics == NULL) {
		cerr << "Nie zainicjowano silnika gry!" << endl;
		exit(1);
	}
	if (physics->getPvdConnectionManager())
	{
		physics->getVisualDebugger()->setVisualizeConstraints(true);
		physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
		physics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
		connection = PxVisualDebuggerExt::createConnection(physics->getPvdConnectionManager(), PVD_HOST, 5425, 10);
	}
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDesc);
}

void shutdownPhysX()
{
	scene->release();
	if (connection != NULL) {
		connection->release();
	}
	physics->release();
	foundation->release();
}

void stepPhysx()
{
	scene->simulate(timestep);
	scene->fetchResults(true);
}

void addFloor()
{
	PxMaterial* material = physics->createMaterial(0.5, 0.5, 0.5);
	PxTransform position = PxTransform(
		PxVec3(0.0f, 0.0f, 0.0f),
		PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f))
		);
	PxRigidStatic* plane = physics->createRigidStatic(position);
	plane->createShape(PxPlaneGeometry(), *material);
	scene->addActor(*plane);
}

void addWalls()
{
	PxMaterial* material = physics->createMaterial(0.5, 0.5, 0.5);

	//Left wall
	PxTransform position = PxTransform(PxVec3(-0.635f, 0.0f, -1.27f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* wall = physics->createRigidStatic(position);
	wall->createShape(PxBoxGeometry(PxVec3(0.2f, 0.1f, 1.27f)), *material);
	scene->addActor(*wall);

	//Right wall
	position = PxTransform(PxVec3(0.635f, 0.0f, -1.27f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	wall = physics->createRigidStatic(position);
	wall->createShape(PxBoxGeometry(PxVec3(0.2f, 0.1f, 1.27f)), *material);
	scene->addActor(*wall);
}

void addBall()
{
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxTransform position = PxTransform(PxVec3(0.0f, 2.0f, -2.0f));
	PxRigidDynamic* ball = physics->createRigidDynamic(position);
	ball->createShape(PxSphereGeometry(0.057f), *material);
	ball->setMass(0.2f);
	scene->addActor(*ball);
	//ball->putToSleep();
}

void addPlayer()
{
	PxReal hx = 0.25f;
	PxReal hy = 0.25f;
	PxReal hz = 0.25f;
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxTransform position = PxTransform(PxVec3(0.0f, 0.0f, 0.0f));
	PxRigidDynamic* player = physics->createRigidDynamic(position);
	player->createShape(PxBoxGeometry(hx, hy, hz), *material);
	player->setMass(0.5f);
	scene->addActor(*player);
	player->addForce(PxVec3(0.0f, 0.0f, -10.0f), PxForceMode::eIMPULSE);
}

void addTrigger()
{
	PxReal hx = 0.5f;
	PxReal hy = 0.25f;
	PxReal hz = 0.5f;
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.5f);
	PxTransform position = PxTransform(PxVec3(0.0f, 0.0f, -2.0f));
	PxRigidStatic* boxTrigger = physics->createRigidStatic(position);
	boxTrigger->createShape(PxBoxGeometry(hx, hy, hz), *material);
	PxShape* triggerShape;
	boxTrigger->getShapes(&triggerShape, 1);
	triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	triggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	scene->addActor(*boxTrigger);
}

void PxSimulationEventCallback::onTrigger(PxTriggerPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxTriggerPair& curTriggerPair = pairs[i];
		PxRigidActor* otherActor = curTriggerPair.otherActor;
		
		addBall();
	}
}

int main()
{
	initPhysX();
	
	addFloor();
	addWalls();
	addPlayer();
	addBall();
	addTrigger();



	for (int i = 0; i < 300; i++)
	{
		stepPhysx();
	}
	shutdownPhysX();
	system("pause");
	return 0;
}