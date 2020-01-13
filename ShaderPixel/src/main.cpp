#include "Host.h"
#include "UserInterface.h"
#include "Collision.h"
#include <iostream>

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
		app.swapBuffers();
	}
}

