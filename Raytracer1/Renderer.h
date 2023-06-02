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
	Renderer()= default;
	void Render(const Scene& scene, const Camera& camera);
	glm::vec4 PerPixel(uint32_t x, uint32_t y);
	Ray rayFromScreen(uint32_t x, uint32_t y);
	void OnResize(uint32_t width, uint32_t height);
	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	std::shared_ptr<Walnut::Image> m_FinalImage;
	const Scene* m_ActiveScene = nullptr;
	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;

};

