#pragma once
#include <glm/glm.hpp>
class Ray
{
public:
	Ray();
	Ray(glm::vec3 origin, glm::vec3 direction, float tMax = 1000000000);
	glm::vec3 o, d;
	float tMax;
	glm::vec3 operator() (float t) const;
};

