#pragma once
#include <vector>
#include "Primitive.h"
#include "Light.h"
#include <memory>

class Scene
{
public:
	void addPrimitive(Shape* shape, Material material);
	void addLight(glm::vec3 position, float power);
	std::vector<Primitive> primitives;
	std::vector<Light> lights;
};

