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
	Host app;

	while (!app.shouldClose())
	{
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		glfwPollEvents();
		app.update();
		app.swapBuffers();
	}
}

