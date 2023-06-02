#include "Scene.h"

void Scene::addPrimitive(Shape* shape, Material material)
{
	Primitive primitive(shape, material);
	primitives.push_back(primitive);
}

void Scene::addLight(glm::vec3 position, float power)
{

	Light light(position, power);
	lights.push_back(light);
}
