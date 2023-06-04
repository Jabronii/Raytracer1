#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include <memory>
#include <iostream>

#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Transform.h"
#include "Sphere.h"
#include "Light.h"
#include "Plane.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f), spherePos(0.5, 0., -3.)
	{
		Material redMaterial(glm::vec3(0.3,0.0,0.0), 1., 0.0);
		Material blueMaterial(glm::vec3(0., 0., 1.), 0.2, 0.4);
		Material greenMaterial(glm::vec3(0., 1., 0.), 0.2, 0.4);
		Material magentaMaterial(glm::vec3(1., 0., 1.), 0.2, 0.1);
		Material yellowMaterial(glm::vec3(1., 0.8, 0.1), 0.2, 0.2);

		//m_Camera.m_ForwardDirection = glm::normalize(glm::vec3(0., -1., -1.));

		//Add sphere
		Shape* sphere = new Sphere(spherePos, 0.5);
		m_Scene.addPrimitive(sphere, magentaMaterial);

		//Add sphere 2
		Shape* sphere2 = new Sphere(spherePos + glm::vec3(-1.5,0.3,0.5), 0.7);
		m_Scene.addPrimitive(sphere2, greenMaterial);

		//Add sphere 3
		Shape* sphere3 = new Sphere(spherePos + glm::vec3(0., 0.5, 1.), 0.3);
		m_Scene.addPrimitive(sphere3, yellowMaterial);

		//top plane
		Shape* plane = new Plane(glm::vec3(0., 2., 0.), glm::vec3(0., -1., 0.));
		m_Scene.addPrimitive(plane, blueMaterial);
		//bottom plane
		Shape* plane1 = new Plane(glm::vec3(0., -0.5, 0.), glm::vec3(0., 1., 0.));
		m_Scene.addPrimitive(plane1, redMaterial);
		//left plane
		Shape* plane2 = new Plane(glm::vec3(-2., 0., 0.), glm::vec3(1., 0., 0.));
		m_Scene.addPrimitive(plane2, magentaMaterial);
		//right plane
		Shape* plane3 = new Plane(glm::vec3(1.5, 0., 0.), glm::vec3(-1., 0., 0.));
		m_Scene.addPrimitive(plane3, blueMaterial);
		//front plane
		Shape* plane4 = new Plane(glm::vec3(0., 0., 3.5), glm::vec3(0., 0., -1.));
		m_Scene.addPrimitive(plane4, yellowMaterial);
		//back plane
		Shape* plane5 = new Plane(glm::vec3(0., 0., -3.4), glm::vec3(0., 0., 1.));
		m_Scene.addPrimitive(plane5, yellowMaterial);

		//add light
		m_Scene.addLight(glm::vec3(1, 1., 1.5), 250.);
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");

		ImGui::PushID(1);

		Primitive& primitive = m_Scene.primitives[0];
		Light& light = m_Scene.lights[0];

		ImGui::DragFloat3("Sphere position", glm::value_ptr(spherePos), 0.1f);
		ImGui::DragFloat3("Light position", glm::value_ptr(light.position), 0.1f);
		ImGui::DragFloat("Power", &light.power, 0.1f);

		ImGui::Separator();

		ImGui::PopID();
		primitive.shape->objectToWorld = Transform(spherePos);

		/*
		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

		if (ImGui::Button("Reset"))
			m_Renderer.ResetFrameIndex();

		ImGui::End();
		*/
		ImGui::Text("Frame Index %.i", m_Renderer.m_FrameIndex);
		std::cout << m_Renderer.m_FrameIndex << std::endl;

		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
		}
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}
	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Scene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts))
			m_Renderer.ResetFrameIndex();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	Scene m_Scene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0; 
	float m_LastRenderTime = 0.0f;

	glm::vec3 spherePos;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Raytracer1";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}