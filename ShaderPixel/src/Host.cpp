#include <glad/glad.h>
#include <imgui_impl_glfw.h>

#include "Host.h"
#include "UserInterface.h"
#include "ShaderPixel.h"

namespace { namespace Callbacks {
	void Error(int error, const char* description)
	{
		std::cerr << "GLFW Sent error " << error << ": " << description << "\n";
	}

	void MousePosition(GLFWwindow* window, double x, double y)
	{
		Host* host = Host::FromWindow(window);
		host->onMouseMove(float(x), float(y));
	}
	void Scroll(GLFWwindow* window, double x, double y)
	{
		Host* host = Host::FromWindow(window);
		host->onScroll(float(x), float(y));
	}
	void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Host* host = Host::FromWindow(window);
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		host->onKey(key, scancode, action, mods);
	}
	void FramebufferSize(GLFWwindow* window, int width, int height)
	{
		Host* host = Host::FromWindow(window);
		host->updateWindowSize(width, height);
	}
} }

Host* Host::FromWindow(GLFWwindow* window)
{
	return (Host*)glfwGetWindowUserPointer(window);
}

void Host::onKey(int key, int scancode, int action, int mods)
{
	if (mApplication)
		mApplication->onKey(key, scancode, action, mods);
}

void Host::onMouseMove(float x, float y)
{
	float deltaX = x - mMouseX;
	float deltaY = y - mMouseY;
	mMouseX = x;
	mMouseY = y;
	mApplication->onMouseMove(x, y, deltaX, deltaY);
	//glfwSetCursorPos(mWindow, mWidth / 2, mHeight / 2);
}

void Host::onScroll(float x, float y)
{
	mApplication->onScroll(x, y);
	//glfwSetCursorPos(mWindow, mWidth / 2, mHeight / 2);
}


void Host::preframe()
{
	mApplication->preframe();
}

Host::Host(const std::string &dllPath)
	: mMemory(nullptr)
	, mLib(NULL)
	, mDllPath(dllPath)
	, mApplication(nullptr)
	, allocate(&malloc)
	, deallocate(&free)
	, mForceReload(false)
{

	glfwSetErrorCallback(Callbacks::Error);
	// TODO: ??? glfwInitHint(); 
	glfwInit();


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_DOUBLEBUFFER, 1);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, 1);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwWindowHint(GLFW_FLOATING, 1); // window always on top

	mWindow = glfwCreateWindow(1024, 1024, "ShaderPixel", nullptr, nullptr);
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetWindowOpacity(mWindow, 0.8f); // transparent window

	glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
	glfwSetKeyCallback(mWindow, Callbacks::Keyboard);
	glfwSetCursorPosCallback(mWindow, Callbacks::MousePosition);
	glfwSetScrollCallback(mWindow, Callbacks::Scroll);
	glfwSetFramebufferSizeCallback(mWindow, Callbacks::FramebufferSize);

	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(1);

	glfwGetWindowContentScale(mWindow, &mScale, &mScale);

	gladLoadGL();

	mApplication = new ShaderPixel();
	mApplication->init(this);
	mApplication->updateWindowSize(mWidth, mHeight);

	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
	glfwFocusWindow(mWindow);

	double dmousex, dmousey;
	glfwGetCursorPos(mWindow, &dmousex, &dmousey);
	mMouseX = float(dmousex);
	mMouseY = float(dmousey);
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
	mApplication->update();
#if HOTLOAD
	mForceReload = ImGui::Button("ForceReloadDLL");
#endif
}

void Host::swapBuffers()
{
	mApplication->renderUI();
	glfwMakeContextCurrent(mWindow);
	glfwSwapBuffers(mWindow);
}

Host::~Host()
{
	delete mApplication;
    ImGui_ImplGlfw_Shutdown();
	glfwDestroyWindow(mWindow);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwTerminate();
}

