#include "Shape.h"

Shape::Shape(Transform objectToWorld):
	objectToWorld(objectToWorld)
{
}

bool Shape::Intersect(Ray ray, float* tHit, SurfaceInteraction* interaction) const
{
	return false;
}

bool Shape::IntersectP(Ray ray)
{
	float tHit = ray.tMax;
	SurfaceInteraction interaction;
	return Intersect(ray, &tHit, &interaction);
}
