#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Transform
{
public:
	Transform(glm::vec3 position, float theta = 0, float phi = 0);
	glm::mat4 mat, inv;
};

