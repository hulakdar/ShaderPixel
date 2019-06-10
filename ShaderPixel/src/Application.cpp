#include "Application.h"
#include "UserInterface.h"
#include <iostream>

namespace {
namespace Callbacks {
	void Error(int error, const char* description)
	{
		std::cerr << "GLFW Sent error " << error << ": " << description << "\n";
	}
	void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	void FramebufferSize(GLFWwindow* window, int width, int height)
	{
		Application* AppState = Application::FromWindow(window);
		AppState->updateWindowSize(width, height);
	}
}
}

Application* Application::FromWindow(GLFWwindow* window)
{
	return (Application*)glfwGetWindowUserPointer(window);
}

Application::Application()
{
	glfwSetErrorCallback(Callbacks::Error);
	// TODO: glfwInitHint(); 
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mWindow = glfwCreateWindow(1280, 720, "ShaderPixel", nullptr, nullptr);
	glfwSetWindowUserPointer(mWindow, this);

	glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
	glfwSetKeyCallback(mWindow, Callbacks::Keyboard);
	glfwSetFramebufferSizeCallback(mWindow, Callbacks::FramebufferSize);

	glfwMakeContextCurrent(mWindow);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 410 core");
}

void Application::updateWindowSize(int x, int y)
{
	mWidth = x;
	mHeight = y;
	glViewport(0, 0, mWidth, mHeight);
}

bool Application::shouldClose()
{
	return glfwWindowShouldClose(mWindow);
}

void Application::update()
{
	glfwPollEvents();
}

void Application::swapBuffers()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(mWindow);
}

Application::~Application()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}


