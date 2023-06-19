#pragma once
#include <glm/glm.hpp>
#include "SurfaceInteraction.h"
#include <algorithm>
#include <cmath>
//blinn-phong BSDF
/*
glm::vec3 bsdf(SurfaceInteraction interaction, glm::vec3 wi, glm::vec3 wo)
{

	glm::vec3 halfWayVec = glm::normalize(wi + wo);
	glm::vec3 normal = interaction.normal;

	Material material = interaction.material;
	//Calculate phong model
	//diffuse
	float dotProduct = glm::dot(normal, wi);
	dotProduct = dotProduct > 0 ? dotProduct : 0;
	glm::vec3 diffuse = dotProduct * material.diffuse;

	//specular
	dotProduct = glm::dot(halfWayVec, normal);
	float alpha = 8;
	glm::vec3 specular = std::pow(dotProduct, alpha) * material.specular;
	return diffuse + specular;
}
*/