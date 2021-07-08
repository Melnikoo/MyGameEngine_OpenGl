#pragma once
#include <glm/glm.hpp>
#include<map>

class GameObject
{
protected:
	glm::vec3 pos;
	glm::vec3 col;
public:
	static std::map<char, bool> keys;
	static std::map<int, bool> specialKeys;


	GameObject();
	GameObject(glm::vec3 position);
	GameObject(glm::vec3 position, glm::vec3 color);
	~GameObject();

	glm::vec3 GetPosition();


};