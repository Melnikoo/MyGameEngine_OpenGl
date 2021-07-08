/*
*	This is a very stripped, super basic example of PhysX running. I've removed anything file wise not required (aka nearly all of it).
*	NOTE: You will have to download the PhysX API and build it yourself to access anything more complex than this!
*
* A quick rundown of PhysX and how it works:
*
*	Actors are the physics entities in the game world
*		Meaning they have physical state (position, orientation, velocity, energy etc.)
*	Actors can be made of 1+ shapes
*		Via a PxGeometry class, which need a Material (static friction, dynamic friction and restitution)
*	Actors can be static (unmoving), dynamic (updated by PhysX) or moved by the user
*
*	A PhysX world contains Scenes, which contain Actors
*	Each Scene has its own state (reference frame, gravity etc.)
*	We update the scene which simulates it - movement, rotation, collision detection, collision response
*
*	Math libraries are built in such as PxMat44, PxVec3, PxQuat etc.
*
*	In this code, there is no debugging - If you use PhysX, I highly suggest you look into this to help you out when you run your game engine!
*
*	I've used conio.h (which is a non standard header file) for checking for keypresses in the while loop - if it breaks on your PC, comment out the entire first if statement to remove the _kbhit() and _getch()
*	If it does work, press space bar to apply a force to the cube, knocking it upwards.
*/
#include <GLFW/glfw3.h>
#include <iostream>
#include <conio.h>

//Include for PhysX
#include <PhysX/PxConfig.h>
#include <PhysX/PxPhysicsAPI.h>


//Additional lib files we need to tell Visual Studio about (note: these are placed where the .cpp/.h files are)
//We could also do this via Properties -> Linker -> Input -> Additional Dependencies (they both do the same thing)
#pragma comment(lib,"PhysX_64.lib")
#pragma comment(lib,"PhysXFoundation_64.lib")
#pragma comment(lib,"PhysXCommon_64.lib")
#pragma comment(lib,"PhysXCooking_64.lib")
#pragma comment(lib,"PhysXExtensions_static_64.lib")

//Saves us having to type it everywhere for anything PhysX related
using namespace physx;



class Physics
{
public:
	float walkSpeed = 10.f;
	PxScene* scene;
	PxRigidDynamic* cube;
	PxRigidDynamic* sphere;
	PxRigidDynamic* playerCol;
	PxVec3 cubePos;
	PxVec3 playerColPos;
	GLFWwindow* window;
	Physics(GLFWwindow* win);
	void PhysMain();
	void Update();
	void CleanUp();
};