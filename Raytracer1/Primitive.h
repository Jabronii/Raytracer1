#pragma once
#include "Shape.h"
#include <memory>
class Primitive
{
public:
	Primitive(Shape* shape, Material material);
	bool Intersect(Ray ray, float* tHit, SurfaceInteraction* interaction) const;
	std::shared_ptr<Shape> shape;
	Material material;
};

