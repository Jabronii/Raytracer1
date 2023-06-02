#pragma once

#include <glm/glm.hpp>
#include "Shape.h"
class Sphere : public Shape
{
public:
	Sphere(glm::vec3 center, float radius);
	bool Intersect(Ray ray, float* tHit, SurfaceInteraction* interaction) const;
	float radius;
};

