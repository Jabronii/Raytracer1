#pragma once
#include <glm/glm.hpp>
#include "Material.h"

class SurfaceInteraction
{
public:
	SurfaceInteraction();
	glm::vec3 intersectPos;
	glm::vec3 normal;
	Material material;
};

