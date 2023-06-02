#include "Renderer.h"
#include <iostream>
#include "Sphere.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtc/color_space.hpp>

namespace Utils {

	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}

	float length2(glm::vec3 vector)
	{
		return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
	}
}

void Renderer::Render(const Scene& scene, const Camera& camera)
{
	m_ActiveScene = &scene;
	m_ActiveCamera = &camera;
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 color = PerPixel(x, y);
			//Tone mapping
			color = color / (color + glm::vec4(1.));
			//gamma correction
			//color = glm::convertLinearToSRGB(color);
			color.w = 1.;

			//color = glm::clamp(color, glm::vec4(0.), glm::vec4(1.));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(color);
		}
	}
	m_FinalImage->SetData(m_ImageData);
}

//wi and wo must be normalized, point away from intersection point
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

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	Ray screenRay = rayFromScreen(x, y);
	float tHit;
	SurfaceInteraction interaction;
	SurfaceInteraction possibleInteraction;
	bool intersected = false;
	uint32_t objectCount = m_ActiveScene->primitives.size();
	std::vector<float> hits(objectCount);
	for (int i = 0; i < hits.size(); i++)
	{
		hits[i] = -1;
	}
	for (uint32_t i = 0; i < m_ActiveScene->primitives.size(); i++)
	{
		if (m_ActiveScene->primitives[i].Intersect(screenRay, &tHit, &possibleInteraction))
		{
			intersected = true;
			hits[i] = tHit;
			if (tHit< screenRay.tMax)
			{
				screenRay.tMax = tHit;
				interaction = possibleInteraction;
			}
		}
	}
	if (x == m_FinalImage->GetWidth()/2 && y == m_FinalImage->GetHeight() / 2)
	{
		for (int i = 0; i < hits.size(); i++)
		{
			std::cout << hits[i] << " ";
		}
		std::cout << std::endl;
	}
	if (intersected)
	{
		glm::vec4 radiance(interaction.material.ambient,1.);
		for (uint32_t j = 0; j < m_ActiveScene->lights.size(); j++)
		{
			Light currentLight = m_ActiveScene->lights[j];

			float lightRadiance = currentLight.power / length2(currentLight.position - interaction.intersectPos);
			glm::vec3 toCamera = glm::normalize(-screenRay.d);
			glm::vec3 toLight = glm::normalize(currentLight.position - interaction.intersectPos);
			
			radiance += lightRadiance * glm::vec4(bsdf(interaction, toLight, toCamera), 0.);
			//radiance += glm::vec4(1.);
		}
		return radiance;
	}
	return glm::vec4(1.);
}

Ray Renderer::rayFromScreen(uint32_t screenx, uint32_t screeny)
{
	Ray ray;
	ray.o = m_ActiveCamera->GetPosition();
	ray.d = m_ActiveCamera->GetRayDirections()[screenx + screeny * m_FinalImage->GetWidth()];
	return ray;
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];

	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];
}