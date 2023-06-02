#include "Plane.h"
#include <iostream>

Plane::Plane(glm::vec3 origin, glm::vec3 normal):
	origin(origin), normal(glm::normalize(normal)), Shape(Transform(glm::vec3(0.)))
{
}

bool Plane::Intersect(Ray ray, float* tHit, SurfaceInteraction* interaction) const
{
	float t = glm::dot(origin - ray.o, normal) / glm::dot(ray.d, normal);
	//>0.001 so that nothing weird happens when it is very small
	if (t <0.001||t>ray.tMax)
	{
		return false;
	}
	*tHit = t;
	interaction->intersectPos = ray(t);
	//so it points towards the camera
	interaction->normal = normal;
	//interaction->normal = glm::dot(normal, ray.d) < 0 ? normal : -normal;
	return true;
}
