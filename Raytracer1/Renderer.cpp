#include "Renderer.h"
#include <iostream>
#include "Sphere.h"
#include <glm/gtx/norm.hpp>
#include <glm/gtc/color_space.hpp>
#include <glm/gtc/random.hpp>

#include "bsdf.h"

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

glm::vec3 randomDirectionOnHemisphere(glm::vec3 normal)
{
	glm::vec3 randomDir = glm::sphericalRand(1.);
	if (glm::dot(normal, randomDir)<0)
	{
		randomDir = -randomDir;
	}
	return randomDir;
}

glm::vec3 Renderer::sample(uint32_t x, uint32_t y)
{
	Ray ray = rayFromScreen(x, y);
	//std::cout << ray.o.x << " " << ray.o.y << " " << ray.o.z << std::endl;
	constexpr uint32_t depth = 5;
	glm::vec3 totalRadiance = glm::vec3(0.);
	SurfaceInteraction interaction;
	SurfaceInteraction dummyInteraction;
	float multiplier = 1.;

	for (uint32_t i = 0; i < depth; i++)
	{
		//the interaction normal starts at 0 so it is ok, this is meant to prevent self intersections
		ray.o += 0.001f * interaction.normal;
		if (findIntersection(ray, *m_ActiveScene, &interaction))
		{
			for (uint32_t j = 0; j < m_ActiveScene->lights.size(); j++)
			{
				Light currentLight = m_ActiveScene->lights[j];

				float lightRadiance = currentLight.power / length2(currentLight.position - interaction.intersectPos);
				glm::vec3 toCamera = glm::normalize(-ray.d);
				glm::vec3 toLight = glm::normalize(currentLight.position - interaction.intersectPos);
				//because there are no transparent materials yet we can automatically reject if the surface does not face the light
				if (glm::dot(interaction.normal, toLight)<=0.f)
				{
					continue;
				}

				Ray rayToLight(interaction.intersectPos, toLight);

				//add light contribution if it isn't in shadow
				//get the intersection and then compare the distance to the distance to the light
				findIntersection(rayToLight, *m_ActiveScene, &dummyInteraction);
				if (length2(dummyInteraction.intersectPos - rayToLight.o)>length2(currentLight.position-rayToLight.o))
				{
					totalRadiance += lightRadiance * bsdf(interaction, toLight, toCamera)*multiplier;
				}
			}
		}
		else
		{
			//no intersection, we have hit the skybox
			glm::vec3 skyColor = glm::vec3(1., 0., 0.);
			totalRadiance += skyColor * multiplier;
			break;
		}
		//no need to do these expensive calculations if it is the end
		if (i == depth-1)
		{
			break;
		}
		glm::vec3 randomDir = randomDirectionOnHemisphere(interaction.normal);
		ray.o = interaction.intersectPos;
		ray.d = randomDir;
		multiplier *= 0.2;
	}
	return totalRadiance;
}

glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y)
{
	return glm::vec4(sample(x,y),1.);
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