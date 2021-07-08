#include "physics.h"

static PxDefaultErrorCallback gDefaultErrorCallback;
//As PhysX performs all memory allocations via PxAllocatorCallback interfaces, we need one to initialize PhysX.
//However we can use the simple implementation of one in the PhysX Extensions Library to make it easier for us to get started
static PxDefaultAllocator gDefaultAllocatorCallback;
PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
PxPhysics* physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale());



Physics::Physics(GLFWwindow* win)
{
	window = win;
}

void Physics::PhysMain()
{
	
		/*
			---------------- SET UP OF PHYSX ----------------
		*/

		//Every PhysX module needs a PxFoundation instance avaliable.
		//(Version ID, allocator callback, error callback) 
		//Using PX_PHYSICS_VERSION allows PhysX to check for version mismatches between header files and SDK .dll files

		//If we can't make a foundation for whatever reason, tell the user
		if (!foundation)
			std::cout << "Could not create foundation\n";

		//Top level PxPhysics object that we use to access functions such as creating scenes and actors
		//Tolerance values allow the correct results to be produced regardless of the scale, units of length and mass, typical speed etc.
		//We use the defaults via the empty constructor/default object for PxTolerancesScale.
		

		//If we can't make a physics object for whatever reason, tell the user
		if (!physics)
			std::cout << "Could not create physics\n";

		/*
			---------------- SCENE CREATION ----------------
		*/

		//This holds information that we want the scene to have
		//We need the PxTolerancesScale to be the same so we grab it via the PxPhysics object
		PxSceneDesc sceneDesc(physics->getTolerancesScale());

		//We need to tell the scene what value we want the gravity to be applied to objects in the scene,
		//give it a CPU dispatcher and a Filter Shader (which we will use PhysX defaults for to make it easy for us)
		sceneDesc.gravity = PxVec3(0.f, -9.8f, 0.f);
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1); //schedules execution of tasks passed to it by the SKD, 1 thread
		sceneDesc.filterShader = PxDefaultSimulationFilterShader; //provides logic for handling collision pairs (how should potentially colliding objects be processed by the simulation - process (and how), ignore, discard)

		//Our scene where actors will be held and updated - created with the descriptor above
		//It is a collection of bodies, particle systems and constraints which can interact
		scene = physics->createScene(sceneDesc);

		/*
			---------------- ACTOR CREATION ----------------
		*/

		//Materials represent surface properties
		//(static friction, dynamic friction, restitution)
		PxMaterial* baseMaterial = physics->createMaterial(0.5f, 0.5f, 0.5f);

		//Actors will need: 
		//	A Transform (position and orientation)
		//	A Type (RigidStatic / RigidDynmaic)
		//	A Shape (Type of Shape, Size of Shape, Material of Shape)

		//We create a Plane object that is a static rigidbody (so it will not move due to gravity, collisions etc.)

		//We have to rotate it so it acts like a floor
		PxTransform planePos = PxTransform(PxVec3(0.f, 0.f, 0.f), PxQuat(PxHalfPi, PxVec3(0.f, 0.f, 1.f)));
		//We create the Actor via a object type given a Transform
		PxRigidStatic* plane = physics->createRigidStatic(planePos);
		//We create a Shape based off some Geometry and a Material (Plane Geometry is infinite remember)
		PxShape* planeShape = physics->createShape(PxPlaneGeometry(), *baseMaterial);
		//We attach the Shape to the RigidBody so now the Plane has a Transform, Type and Shape
		plane->attachShape(*planeShape);
		//We add the Actor to the Scene so it is simulated
		scene->addActor(*plane);

		//We create a Cube with the same logic
		//Create a Transform -> Create A RigidBody Type (dynamic in this case)
		//Create a Shape based off:
		//			Geometry (note its a box so we have to pass in size information)
		//			A Material (I'm using the same material but you could use any you make)
		//Attach the Shape to the RigidBody
		//Add the Actor to the Scene to simulate it
		PxTransform cubePos = PxTransform(PxVec3(0.f, 0.5f, -3.f));
		cube = physics->createRigidDynamic(cubePos);
		
		PxBoxGeometry cubeGeo(PxVec3(1.f, 0.001f, 1.f));
		PxShape* cubeShape = physics->createShape(cubeGeo, *baseMaterial);
		cube->attachShape(*cubeShape);

		scene->addActor(*cube);
		
		/*PxTransform playerColPos(PxVec3(0.0f, 1.f, 3.f));
		playerCol = physics->createRigidDynamic(playerColPos);

		PxBoxGeometry playerGeo(PxVec3(0.5f, 2.f, 0.5f));
		PxShape* playerShape = physics->createShape(playerGeo, *baseMaterial);
		playerCol->attachShape(*playerShape);

		

		scene->addActor(*playerCol);*/

		//Finally, I create a Sphere object that will not be affected by gravity and has a torque applied to it
		PxTransform spherePos = PxTransform(PxVec3(5, 10, 0));
		sphere = physics->createRigidDynamic(spherePos);

		/*PxSphereGeometry sphereGeo(1.0f);
		PxShape* sphereShape = physics->createShape(sphereGeo, *baseMaterial);
		sphere->attachShape(*sphereShape);

		sphere->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

		//scene->addActor(*sphere);

		sphere->addTorque(PxVec3(0, 0, 100));

		std::cout << "Scene and Actors created!\n";*/

}

void Physics::Update()
{
	float deltaTime = 1.f / 60.f; 	//A hard coded step size which could use a changing deltaTime
	bool run = true;
	int counter = 0; //Just to see which frame we are on
	if(glfwGetKey)
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cube->addForce(PxVec3(0.0f, 0.0f, -walkSpeed));
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cube->addForce(PxVec3(0.0f, 0.0f, +walkSpeed));
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cube->addForce(PxVec3(-walkSpeed, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cube->addForce(PxVec3(+walkSpeed, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		cube->addForce(PxVec3(0.f, 30.f, 0.0f));
	
		//Advances the simulation by delta time - does some processing until physics is computed
		scene->simulate(deltaTime);
		//Allows the simulation to finish and returns the results
		//true means the simulation will block until its finished (so results of the simulation step are guaranteed to be available)
		scene->fetchResults(true);

		//Some print outs to show things are moving (or not) due to gravity and then collision with the Plane
		//You can comment out the line 117 = [scene->addActor(*plane);] so that the cube falls forever
		cubePos = cube->getGlobalPose().p;
		//std::cout << "Cube Pos: " << cubePos.x << ", " << cubePos.y << ", " << cubePos.z << "\n";

		//PxVec3 planePos = plane->getGlobalPose().p;
		//std::cout << "Plane Pos: " << planePos.x << ", " << planePos.y << ", " << planePos.z << "\n";

		float angle;
		PxVec3 axis;
		sphere->getGlobalPose().q.toRadiansAndUnitAxis(angle, axis);
		//std::cout << "Sphere Angle: " << angle << "\t Sphere Axis: [" << axis.x << ", " << axis.y << ", " << axis.z << "]\n";
	

	/*
		---------------- CLEAN UP / SHUTDOWN ----------------
	*/

	//Dispose of all PhysX objects via the release() function, destroying the object and all contained objects
}

void Physics::CleanUp()
{
	physics->release();
	foundation->release();
}
