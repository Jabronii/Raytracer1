#pragma once
#include <glm/glm.hpp>
class Light
{
public:
	Light(glm::vec3 position, float power);
	glm::vec3 position;
	float power;
};

