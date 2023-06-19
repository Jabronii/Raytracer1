#pragma once
#include "Walnut/Image.h"
#include <glm/glm.hpp>
#include "Walnut/Image.h"
#include "Scene.h"
#include <memory>
#include "Ray.h"
#include "Camera.h"

class Renderer
{
public:
	struct Settings
	{
		bool Accumulate = true;
	};
	Renderer()= default;
	void Render(const Scene& scene, const Camera& camera);
	glm::vec4 PerPixel(uint32_t x, uint32_t y);
	Ray rayFromScreen(uint32_t x, uint32_t y);
	void OnResize(uint32_t width, uint32_t height);
	glm::vec3 Renderer::sample(uint32_t x, uint32_t y);
	glm::vec3 sampleLi(Ray ray, unsigned int depth, unsigned int maxDepth);
	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }

	std::shared_ptr<Walnut::Image> m_FinalImage;
	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;
	Settings m_Settings;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

	uint32_t m_FrameIndex = 1;
};

