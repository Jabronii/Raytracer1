#include "Renderer.h"
#include <iostream>
#include "Sphere.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtc/color_space.hpp>
#include <glm/gtc/random.hpp>

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

	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));
	for (uint32_t y = 0; y < m_FinalImage->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < m_FinalImage->GetWidth(); x++)
		{
			glm::vec4 color = PerPixel(x, y);
			m_AccumulationData[x + y * m_FinalImage->GetWidth()] += color;

			glm::vec4 accumulatedColor = m_AccumulationData[x + y * m_FinalImage->GetWidth()];
			accumulatedColor /= (float)m_FrameIndex;
			//Tone mapping
			accumulatedColor = accumulatedColor / (accumulatedColor + glm::vec4(1.));
			//gamma correction
			//color = glm::convertLinearToSRGB(color);
			accumulatedColor.w = 1.;

			//color = glm::clamp(color, glm::vec4(0.), glm::vec4(1.));
			m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(accumulatedColor);
		}
	}
	m_FinalImage->SetData(m_ImageData);

	if (m_Settings.Accumulate)
		m_FrameIndex++;
	else
		m_FrameIndex = 1;
}




bool findIntersection(Ray &ray, const Scene& scene, SurfaceInteraction* interaction)
{
	bool intersected = false;
	float tHit = -1;
	SurfaceInteraction possibleInteraction;
	SurfaceInteraction finalInteraction;
	for (uint32_t i = 0; i < scene.primitives.size(); i++)
	{
		if (scene.primitives[i].Intersect(ray, &tHit, &possibleInteraction))
		{
			intersected = true;
			if (tHit < ray.tMax)
			{
				ray.tMax = tHit;
				finalInteraction = possibleInteraction;
			}
		}
	}
	if (intersected)
	{
		*interaction = finalInteraction;
	}
	return intersected;
}

glm::vec3 Renderer::sampleLi(Ray ray, unsigned int depth, unsigned int maxDepth)
{
	glm::vec3 skyColor = 300.f* glm::vec3(0.5, 0.5, 1.);
	if (depth > maxDepth)
	{
		return glm::vec3(0.0);
	}
	glm::vec3 totalLightRadiance(0.);
	SurfaceInteraction interaction;
	SurfaceInteraction dummyInteraction;
	if (findIntersection(ray, *m_ActiveScene, &interaction))
	{
		//attenuation = 1 / length2(interaction.intersectPos - ray.o);
		for (uint32_t j = 0; j < m_ActiveScene->lights.size(); j++)
		{
			Light currentLight = m_ActiveScene->lights[j];
			//attenuation
			float lightRadiance = currentLight.power / length2(currentLight.position - interaction.intersectPos);
			glm::vec3 toOrigin = glm::normalize(-ray.d);
			glm::vec3 toLight = glm::normalize(currentLight.position - interaction.intersectPos);
			//because there are no transparent materials yet we can automatically reject if the surface does not face the light
			if (glm::dot(interaction.normal, toLight) <= 0.f)
			{
				continue;
			}

			Ray rayToLight(interaction.intersectPos, toLight);
			//rayToLight.o += 0.0001f * interaction.normal;

			//add light contribution if it isn't in shadow
			//get the intersection and then compare the distance to the distance to the light
			;
			if (!(findIntersection(rayToLight, *m_ActiveScene, &dummyInteraction) && (length2(dummyInteraction.intersectPos - rayToLight.o) > length2(currentLight.position - rayToLight.o))))
			{
				totalLightRadiance += lightRadiance * interaction.material.bsdf(interaction.normal, toLight, toOrigin) * glm::dot(interaction.normal, toLight);
			}
			/*
			if (glm::dot(interaction.normal,toLight)>0 && interaction.intersectPos.y>-0.4 && interaction.intersectPos.y>1.3&& glm::length(totalLightRadiance)==0)
			{
				std::cout << std::sqrt(length2(dummyInteraction.intersectPos - rayToLight.o)) << " " << std::sqrt(length2(currentLight.position - rayToLight.o)) << std::endl;
				std::cout << interaction.intersectPos.x << " " << interaction.intersectPos.y << " " << interaction.intersectPos.z << std::endl;
				std::cout << glm::length(totalLightRadiance) << std::endl;
				std::cout << dummyInteraction.material.roughness<<std::endl;
				std::cout << glm::length(dummyInteraction.material.albedo) << std::endl;
				std::cout << std::endl;
			}
			*/
		}
	}
	else
	{
		return skyColor;
	}
	float pdf = 0;
	glm::vec3 oldDir = -ray.d;
	glm::vec3 sampleDir = glm::normalize(interaction.material.sampleBSDF(pdf, interaction.normal, -ray.d));
	//glm::vec3 sampleDir = randomDirectionOnHemisphere(interaction.normal);
	ray.o = interaction.intersectPos;
	ray.d = sampleDir;
	ray.o += 0.0001f * interaction.normal;
	//multiplier is 1/2pi
	//accumulatedPDF *= 0.1591;
	//accumulatedPDF *= pdf;
	//std::cout << glm::length(interaction.material.bsdf(interaction.normal, sampleDir, oldDir))<<" "<< glm::dot(interaction.normal, sampleDir)<<std::endl;]
	glm::vec3 totalRadiance = totalLightRadiance + pdf * sampleLi(ray, depth + 1, maxDepth) * glm::dot(interaction.normal, sampleDir) * interaction.material.bsdf(interaction.normal, sampleDir, oldDir);
	/*
	if (interaction.material.metalness>0.7 )
	{
		std::cout << glm::length(totalRadiance) << std::endl;
		std::cout << glm::length(interaction.material.bsdf(interaction.normal, sampleDir, oldDir)) << std::endl;
		std::cout << sampleDir.x << " " << sampleDir.y << " " << sampleDir.z << std::endl;
		std::cout << oldDir.x << " " << oldDir.y << " " << oldDir.z << std::endl;
		std::cout << glm::dot(oldDir, interaction.normal) << std::endl;
		std::cout << std::endl;
	}
	*/
	return totalRadiance;
}

glm::vec3 Renderer::sample(uint32_t x, uint32_t y)
{
	Ray ray = rayFromScreen(x, y);
	//std::cout << ray.o.x << " " << ray.o.y << " " << ray.o.z << std::endl;
	constexpr uint32_t maxDepth = 5;
	SurfaceInteraction interaction;
	return sampleLi(ray, 1, maxDepth);
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	return glm::vec4(sample(x,y),1.);
}

Ray Renderer::rayFromScreen(uint32_t screenx, uint32_t screeny)
{
	float randx = (float)screenx + glm::linearRand(0., 1.);
	float randy = (float)screeny + glm::linearRand(0., 1.);
	Ray ray;
	ray.o = m_ActiveCamera->GetPosition();
	ray.d = m_ActiveCamera->getProjectedRay(randx, randy);
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