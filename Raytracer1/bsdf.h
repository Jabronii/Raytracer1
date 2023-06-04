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
const float PI = 3.14159265359;
float NDF(glm::vec3 normal, glm::vec3 halfwayVec, float roughness)
{
	float normalDotHalfway = glm::dot(normal, halfwayVec);
	//call roughness a for shorter formula
	float a = roughness;
	float b = normalDotHalfway * normalDotHalfway * (a * a - 1) + 1;
	return a * a / (PI * b * b);
}

float GeometrySchlickGGX(float NdotV, float k)
{
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
float GeometrySmith(glm::vec3 N, glm::vec3 V, glm::vec3 L, float roughness)
{
	float k = (roughness + 1) * (roughness + 1) / 8;
	float NdotV = std::max(dot(N, V), 0.0f);
	float NdotL = std::max(dot(N, L), 0.0f);
	float ggx1 = GeometrySchlickGGX(NdotV, k);
	float ggx2 = GeometrySchlickGGX(NdotL, k);

	return ggx1 * ggx2;
}
glm::vec3 fresnelShlick(float cosTheta, glm::vec3 F0)
{
	return F0 + (glm::vec3(1.) - F0) * (float)std::pow(std::clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


//wi and wo must be normalized, point away from intersection point
glm::vec3 bsdf(SurfaceInteraction interaction, glm::vec3 wi, glm::vec3 wo)
{
	glm::vec3 normal = interaction.normal;
	Material material = interaction.material;
	glm::vec3 halfwayVec = glm::normalize(wi + wo);
	glm::vec3 F0 = glm::mix(glm::vec3(0.04), material.albedo, material.metalness);

	float D = NDF(normal, halfwayVec, material.roughness);
	float G = GeometrySmith(normal, wo, wi, material.roughness);
	glm::vec3 F = fresnelShlick(std::clamp(glm::dot(halfwayVec, wo), 0.0f, 1.0f), F0);

	glm::vec3 specular = (D * G * F) / (float)(4.0f * std::max(glm::dot(normal, wi), 0.0f) * std::max(glm::dot(normal, wo), 0.0f) + 0.0001);

	glm::vec3 kS = F;
	glm::vec3 kD = glm::vec3(1.f) - kS;
	kD *= 1.0f - material.metalness;

	float NdotL = glm::dot(normal, wi);
	glm::vec3 lambertian = kD * material.albedo / PI;

	//specular = glm::vec3(0.);
	return (lambertian + specular) * NdotL;
	//return glm::vec3(D)* NdotL;
}