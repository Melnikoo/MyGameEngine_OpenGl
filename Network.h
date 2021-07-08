#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#include <enet/enet.h>
using namespace std;
class Network
{
public:
	GLFWwindow* window;
	ENetAddress address;
	ENetHost* server;
	ENetEvent enetEvent;

	struct Vector3 {
		float x;
		float y;
		float z;
	};

	struct PhysicsData {
		int packetType = 1;
		Vector3 positions[2];
	};

	PhysicsData* physicsData;

	struct ClientData {
		int packetType = 0;
		int clientIndex;
	};
	
	struct ClientPacket {
		int clientIndex;
		Vector3 position;
	};
	
	ClientData* clientData;
	ENetPacket* dataPacket;
	ClientPacket* clientPacket;
	int clientCount = 0;
	Network();

	void Update();
};

