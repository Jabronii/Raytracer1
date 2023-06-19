#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <algorithm>
enum bsdfType{COOK_TORRANCE_BSDF, PERFECT_REFLECTION_BSDF};

class Material
{
public:
	Material();
	//reflection BSDF constructor
	Material(glm::vec3 albedo);
	//cook torrance BSDF constructor
	Material(glm::vec3 albedo, float metalness, float roughness);

	glm::vec3 bsdf(glm::vec3 normal, glm::vec3 wi, glm::vec3 wo);
	glm::vec3 sampleBSDF(float& pdf, glm::vec3 normal, glm::vec3 wo);
	glm::vec3 cookTorrancebsdf(glm::vec3 normal, glm::vec3 wi, glm::vec3 wo);

	bool isDelta = 0;
	glm::vec3 albedo;
	float metalness, roughness;
	bsdfType type;
};

