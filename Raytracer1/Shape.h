#pragma once
#include "Transform.h"
#include "Ray.h"
#include "SurfaceInteraction.h"
class Shape
{
public:
	Shape(Transform objectToWorld);
	virtual bool Intersect(Ray ray, float* tHit, SurfaceInteraction* interaction) const;
	virtual bool IntersectP(Ray ray);
	Transform objectToWorld;
};

