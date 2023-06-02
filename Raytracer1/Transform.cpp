#include "Transform.h"

Transform::Transform(glm::vec3 position, float theta, float phi)
{
	glm::mat4 translation = glm::translate(glm::mat4(1), position);
	mat = glm::mat4(1) * translation;
	inv = glm::inverse(mat);
}