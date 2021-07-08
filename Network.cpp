
#include <iostream>
#include <GLFW/glfw3.h>
#include <enet/enet.h>
using namespace std;

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

void main()
{
	/* We begin with our SFML preamble - here, we're just setting up a simple
	background texture with instructions, and to help us be clear which window
	is the server and which is the client in our example. */

	int clientCount = 0;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWwindow* window = glfwCreateWindow(800, 600, "Server", NULL, NULL);

	glfwMakeContextCurrent(window);


	/* Now we're declaring our Vector2 - this vector will be controlling the
	position of an 'enemy' entity in the Client instance. */



	/* We initialise the ENet library, and double-check this has worked. We then
	declare a few library specific variables. An address (which represents the
	server address), a Host (which represents the server, i.e. this software
	instance), and an arbitrary Event. ENet is an event-driven library, and functions
	similarly to SFML in that regard. Just like we poll keyboard events from SFML's
	Event type, we can do the same for ENet's. */

	if (enet_initialize() != 0)
	{
		cout << "Enet failed to initialise!" << "\n\n";
	}

	ENetAddress address;
	ENetHost* server;
	ENetEvent enetEvent;

	ENetPacket* dataPacket;

	/* We set the address values to essentially be a local host */

	address.host = ENET_HOST_ANY;
	address.port = 1234;

	/* Now we create the server. We pass in the pointer to the address variable,
	the maximum number of clients we intend to support, the maximum number of
	channels to be used (in this case 2 channels, 0 and 1), and caps for
	upstream and downstream bandwidth (0 is uncapped). */

	server = enet_host_create(&address, 32, 2, 0, 0);

	/* Now we check server initialisation, before assigning some starting values
	to our position variable.*/

	if (server == NULL)
	{
		cout << "Server failed to initialise!" << "\n\n";
	}

	PhysicsData* physicsData = new PhysicsData;

	physicsData->positions[0].x = 0.0f;
	physicsData->positions[0].y = 2.0f;
	physicsData->positions[0].z = 3.0f;
	physicsData->positions[1].x = 5.0f;
	physicsData->positions[1].y = -0.4f;
	physicsData->positions[1].z = 0.0f;

	ClientPacket* clientPacket = new ClientPacket;

	clientPacket->clientIndex = 0;
	clientPacket->position.x = 0.0f;
	clientPacket->position.y = 0.0f;
	clientPacket->position.z = 0.0f;

	ClientData* clientData = new ClientData;



	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		/*if (e.type == sf::Event::Closed) {
			window.close();

		}*/

		/* As we mentioned, ENet is event driven. This while loop checks to see if
		the server has any events to respond to. We can use the event type to
		determine how to respond to a given event. Note that there are multiple types
		of ENet event	 - in this example, our server is only reacting to a client
		connection being established (ENET_EVENT_TYPE_CONNECT) or a connection being
		dropped (ENET_EVENT_TYPE_DISCONNECT). Note that given our sample Client uses
		the enet_peer_disconnect_now function, there is no guarantee the server will
		detect a disconnect event - that is something you can consider improving in your
		own networking subsystem. */

		while (enet_host_service(server, &enetEvent, 0) > 0)
		{
			switch (enetEvent.type)
			{
			case ENET_EVENT_TYPE_CONNECT:

				cout << "A client connected from address " << enetEvent.peer->address.host << ":" << enetEvent.peer->address.port << ".\n";

				clientData->clientIndex = clientCount;

				dataPacket = enet_packet_create(clientData, sizeof(ClientData), ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(enetEvent.peer, 0, dataPacket);

				/* This event type has an associated peer: the client which has
				connected to the server. We can store some data about this peer
				for as long as it remains in scope using the "data" variable. */

				//enetEvent.peer->data = "This is a client";
				clientCount++;

				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				cout << "The client from address " << enetEvent.peer->address.host << ":" << enetEvent.peer->address.port << " disconnected \n";

				enetEvent.peer->data = NULL;

				break;
			case ENET_EVENT_TYPE_RECEIVE:
				memcpy(clientPacket, enetEvent.packet->data, enetEvent.packet->dataLength);
				int currentClient = clientPacket->clientIndex;
				physicsData->positions[clientPacket->clientIndex] = clientPacket->position;
			}
			cout << clientCount << "\n";
		}

		/* Now we handle out keyboard inputs, manipulating the values of the
		position variable. Notice that these values are lower position changes
		than the equivalents Client-side - that's because if you set them
		equivalent, the server 'game loop' iterates far quicker than the client,
		letting it 'move faster'. This illustrates the dangers of untimed game
		loops which we highlighted last week. */

		//if (e.type == sf::Event::KeyPressed)
		//{
		//	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		//	{
		//		position.x += -0.2f;
		//	}

		//	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		//	{
		//		position.x += 0.2f;
		//	}
		//	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		//	{
		//		position.y += -0.2f;
		//	}
		//	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		//	{
		//		position.y += 0.2f;
		//	}

		//	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		//	{
		//		window.close();
		//	}

		//	cout << "The position of the entity is (" << position.x << "," << position.y << ")\n";

		//	/* We populate the packet we made earlier using the enet_packet_create function, which accepts
		//	a reference to our Vector 2, the size of the data being sent (so, one Vector2 in this case), and
		//	flags whether or not the packet receipt needs verifying (in this example, we say that it does,
		//	but in reality it doesn't necessarily need to). The host then broadcasts the data packet to all
		//	connected clients across Channel 0. */

		//	dataPacket = enet_packet_create(&position, sizeof(Vector2), ENET_PACKET_FLAG_RELIABLE);
		//	enet_host_broadcast(server, 0, dataPacket);
		//}

		/* Basic draw functionality for SFML, making sure our texture appears. */
		dataPacket = enet_packet_create(physicsData, sizeof(PhysicsData), ENET_PACKET_FLAG_RELIABLE);
		enet_host_broadcast(server, 0, dataPacket);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

	/* We destroy the ENet Host instance, and deinitialize the library on shutdown. */

	delete physicsData;
	delete clientData;
	delete clientPacket;

	enet_host_destroy(server);
	atexit(enet_deinitialize);
}