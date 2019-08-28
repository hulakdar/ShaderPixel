#include "Host.h"
#include "UserInterface.h"

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

	while (!app.shouldClose())
	{
		app.preframe();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		glfwPollEvents();
		app.update();
		ImGui::Render();
		ImGui::EndFrame();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		app.swapBuffers();
	}
}

