#include "Host.h"
#include "UserInterface.h"
#include "Collision.h"
#include <iostream>

static const struct
{
	float x, y;
	float r, g, b;
} vertices[] =
{
	{-1.f,-1.f, 1.f, 1.f, 0.f },
	{ 1.f,-1.f, 1.f, 1.f, 0.f },
	{-1.f, 1.f, 1.f, 0.f, 1.f },
	{ 1.f,-1.f, 1.f, 1.f, 0.f },
	{-1.f, 1.f, 1.f, 0.f, 1.f },
	{ 1.f, 1.f, 1.f, 1.f, 1.f }
};

int main(int argc, const char* argv[])
{
	std::string exePath = argv[0];
	exePath.resize(exePath.rfind('\\') + 1);
	Host app(exePath + "ShaderPixel.dll");

#if TESTING
	CollisionSphere sphere = {
		{15, 9, 8},
		3.f
	};
	CollisionAABB box = {
		{0, 0, 0},
	      {10, 10, 10}
	};
	
	assert(PointInsideBox(glm::vec3{1,1,1}, box));
	assert(PointInsideBox(glm::vec3{0,1,1}, box));
	assert(PointInsideBox(glm::vec3{10,1,1}, box));
	assert(!PointInsideBox(glm::vec3{-1,1,1}, box));
	assert(!PointInsideBox(glm::vec3{5,2, 11}, box));
	assert(!PointInsideBox(glm::vec3{10,11,1}, box));
#endif

	while (!app.shouldClose())
	{
		app.preframe();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		glfwPollEvents();
		app.update();
		ImGui::Render();
		ImGui::EndFrame();
		//ImGui::UpdatePlatformWindows();
		//ImGui::RenderPlatformWindowsDefault();
		app.swapBuffers();
	}
}

