#pragma once
#include <glm/glm.hpp>
class Material
{
public:
	Material();
	Material(glm::vec3 albedo, float metalness, float roughness);
	glm::vec3 albedo;
	float metalness, roughness;
};

