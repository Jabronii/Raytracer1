#include "Material.h"
#include <iostream>
#include <algorithm>

const float PI = 3.14159265359;
float NDF(glm::vec3 N, glm::vec3 H, float roughness)
{
	float a = roughness;
	float a2 = a * a;
	float NdotH = std::max(glm::dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float k)
{
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
float GeometrySmith(glm::vec3 N, glm::vec3 V, glm::vec3 L, float k)
{
	float NdotV = std::max(glm::dot(N, V), 0.0f);
	float NdotL = std::max(glm::dot(N, L), 0.0f);
	float ggx1 = GeometrySchlickGGX(NdotV, k);
	float ggx2 = GeometrySchlickGGX(NdotL, k);

	return ggx1 * ggx2;
}
glm::vec3 fresnelShlick(float cosTheta, glm::vec3 F0)
{
	return F0 + (glm::vec3(1.) - F0) * (float)std::pow(std::clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
glm::vec3 Material::cookTorrancebsdf(glm::vec3 normal, glm::vec3 wi, glm::vec3 wo)
{
	float k = roughness * roughness / 2;
	glm::vec3 halfwayVec = glm::normalize(wi + wo);
	glm::vec3 F0 = glm::mix(glm::vec3(0.04), albedo, metalness);

	float D = NDF(normal, halfwayVec, roughness);
	float G = GeometrySmith(normal, wo, wi, k);
	glm::vec3 F = fresnelShlick(std::max(glm::dot(halfwayVec, wo), 0.0f), F0);

	glm::vec3 specular = (D * G * F) / (float)(4.0f * std::max(glm::dot(normal, wi), 0.0f) * std::max(glm::dot(normal, wo), 0.0f) + 0.0001);

	glm::vec3 kS = F;
	glm::vec3 kD = glm::vec3(1.f) - kS;
	kD *= 1.0f - metalness;

	float NdotL = glm::dot(normal, wi);
	glm::vec3 lambertian = kD * albedo / PI;

	//specular = glm::vec3(0.);
	/*
	if (glm::length(specular)>0.1&&metalness>0.6)
	{
		std::cout << specular.x << " " << specular.y << " " << specular.z << std::endl;
	}
	*/
	return lambertian + specular;
	//return glm::vec3(D)* NdotL;
}
//wi and wo must be normalized, point away from intersection point

glm::vec3 randomDirectionOnHemisphere(glm::vec3 normal)
{
	glm::vec3 randomDir = glm::sphericalRand(1.);
	if (glm::dot(normal, randomDir) < 0)
	{
		randomDir = -randomDir;
	}
	return randomDir;
}


Material::Material()
{
}

Material::Material(glm::vec3 albedo):
	albedo(albedo), isDelta(true), type(PERFECT_REFLECTION_BSDF)
{
}

Material::Material(glm::vec3 albedo, float metalness, float roughness):
	albedo(albedo), metalness(metalness), roughness(roughness), isDelta(false), type(COOK_TORRANCE_BSDF)
{

}

glm::vec3 Material::bsdf(glm::vec3 normal, glm::vec3 wi, glm::vec3 wo)
{
	switch (type)
	{
	case COOK_TORRANCE_BSDF:
		return cookTorrancebsdf(normal, wi, wo);
	case PERFECT_REFLECTION_BSDF:
		glm::vec3 reflection = glm::reflect(wo, normal);
		return glm::vec3(0.f);
		//return reflection == wi ? albedo : glm::vec3(0.f);
	}
}

glm::vec3 cosineWeightedHemisphereSample(glm::vec3 normal, float& pdf)
{
	glm::vec2 randomUniformOnDisk = glm::diskRand(1.);
	float y = std::sqrt(1. - randomUniformOnDisk.x * randomUniformOnDisk.x - randomUniformOnDisk.y * randomUniformOnDisk.y);
	//found z using pythagoras, it is also the cosine of theta
	constexpr float invPi = 0.31830988618;
	pdf = y * invPi;
	glm::vec3 v = glm::vec3(randomUniformOnDisk.x, y, randomUniformOnDisk.y);
	if (normal.y == 1.)
	{
		return v;
	}
	glm::vec3 newy = normal;
	glm::vec3 newz = glm::normalize(glm::cross(newy, glm::vec3(0, 1., 0)));
	glm::vec3 newx = glm::normalize(glm::cross(newy, newz));
	glm::mat3 transform = glm::mat3(newx, newy, newz);
	return transform * v;
}
//normal must be normalized
glm::vec3 Material::sampleBSDF(float& pdf, glm::vec3 normal, glm::vec3 wo)
{
		/*
	case COOK_TORRANCE_BSDF:
		return cosineWeightedHemisphereSample(normal, pdf);
		*/
	switch (type)
	{
	case COOK_TORRANCE_BSDF:
		pdf = 0.1591;
		return randomDirectionOnHemisphere(normal);
	}
	std::cout << "ERROR";
}


