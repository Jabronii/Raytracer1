#include "Ray.h"

Ray::Ray()
{
	tMax = 1000000;
}

Ray::Ray(glm::vec3 origin, glm::vec3 direction, float tMax):
	o(origin), d(direction), tMax(tMax)
{
}
glm::vec3 Ray::operator()(float t) const { return o + d * t; }