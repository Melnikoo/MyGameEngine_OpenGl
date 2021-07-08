#pragma once
#include <GLFW/glfw3.h>
#include <enet/enet.h>
#include <iostream>
#include <GLM/vec3.hpp>

using namespace std;

class NetworkClient
{
public:


	glm::vec3 avatar = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 enemy = glm::vec3(5.f, 0.f, 5.f);

	ENetAddress address;
	ENetHost* client;
	ENetPeer* peer;
	ENetEvent enetEvent;

	GLFWwindow* window;
	struct Vector3 {
		float x;
		float y;
		float z;
	};
	struct PhysicsData {
		int packetType = 1;
		Vector3 positions[2];
	};

	struct ClientData {
		int packetType = 0;
		int clientIndex;
	};

	struct ClientPacket {
		int clientIndex;
		Vector3 position;
	};

	ENetPacket* dataPacket;
	PhysicsData* serverData;
	ClientData* clientData;
	int* packetType;
	ClientPacket* clientPacket;
	int clientIndex;

	NetworkClient();

	void clientMain(GLFWwindow* win);

	void Update();

};

