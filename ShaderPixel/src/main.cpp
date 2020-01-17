#include "ShaderPixel.h"
#include "UserInterface.h"
#include "Collision.h"
#include <iostream>

int main(int argc, const char* argv[])
{
	ShaderPixel app;

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

