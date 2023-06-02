#include "Primitive.h"

Primitive::Primitive(Shape* shape, Material material):
	shape(shape), material(material)
{
}
bool Primitive::Intersect(Ray ray, float* tHit, SurfaceInteraction* interaction) const
{
	if (shape->Intersect(ray, tHit, interaction))
	{
		interaction->material = material;
		return true;
	}
	return false;
}
