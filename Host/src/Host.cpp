#include "Host.h"
#include "UserInterface.h"
#include "Application.h"
#include <iostream>
#include <fstream>
#include <imgui_impl_glfw.h>

namespace {
namespace Callbacks {
	void Error(int error, const char* description)
	{
		std::cerr << "GLFW Sent error " << error << ": " << description << "\n";
	}
	void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Host* AppState = Host::FromWindow(window);
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	void FramebufferSize(GLFWwindow* window, int width, int height)
	{
		Host* AppState = Host::FromWindow(window);
		AppState->updateWindowSize(width, height);
	}
}
}

Host* Host::FromWindow(GLFWwindow* window)
{
	return (Host*)glfwGetWindowUserPointer(window);
}

Host::Host()
{
	glfwSetErrorCallback(Callbacks::Error);
	// TODO: glfwInitHint(); 
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, 1);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_FLOATING, 1);

	mWindow = glfwCreateWindow(500, 500, "ShaderPixel", nullptr, nullptr);
	glfwSetWindowUserPointer(mWindow, this);
	//glfwSetWindowOpacity(mWindow, 0.8f);

	glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
	glfwSetKeyCallback(mWindow, Callbacks::Keyboard);
	glfwSetFramebufferSizeCallback(mWindow, Callbacks::FramebufferSize);

	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(1);


	glfwGetWindowContentScale(mWindow, &mScale, &mScale);
	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);

	mApplication->init(this);
}

void Host::updateWindowSize(int x, int y)
{
	mWidth = x;
	mHeight = y;
	mApplication->updateWindowSize(x, y);
}

bool Host::shouldClose()
{
	return glfwWindowShouldClose(mWindow);
}

void Host::update()
{
	mApplication->update(this);
}

void Host::swapBuffers()
{
	GLFWwindow* backup_current_context = glfwGetCurrentContext();
	mApplication->renderUI(this);
	glfwMakeContextCurrent(backup_current_context);
	glfwSwapBuffers(mWindow);
}

Host::~Host()
{
    ImGui_ImplGlfw_Shutdown();
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

