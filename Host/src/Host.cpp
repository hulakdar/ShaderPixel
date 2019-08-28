#include <glad/glad.h>
#include "Host.h"
#include "UserInterface.h"
#include <iostream>
#include <fstream>
#include <imgui_impl_glfw.h>
#include <windows.h>
#include <assert.h>
//#include "Application.h"
#include "ShaderPixel.h"

namespace {
namespace Callbacks {
	void Error(int error, const char* description)
	{
		std::cerr << "GLFW Sent error " << error << ": " << description << "\n";
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
}
}

Host* Host::FromWindow(GLFWwindow* window)
{
	return (Host*)glfwGetWindowUserPointer(window);
}

void Host::onKey(int key, int scancode, int action, int mods)
{
	if (mApplication)
		;//mApplication->onKey(key, scancode, action, mods);
}

void Host::preframe()
{
	updateDLL();
	mApplication->preframe();
}

Host::Host(const std::string &dllPath)
	: mMemory(nullptr)
	, mLib(NULL)
	, mDllPath(dllPath)
	, mApplication(nullptr)
	, allocate(&malloc)
	, deallocate(&free)
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
	//glfwWindowHint(GLFW_FLOATING, 1); // window always on top

	mWindow = glfwCreateWindow(500, 500, "ShaderPixel", nullptr, nullptr);
	glfwSetWindowUserPointer(mWindow, this);
	//glfwSetWindowOpacity(mWindow, 0.8f); // transparent window

	glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
	glfwSetKeyCallback(mWindow, Callbacks::Keyboard);
	glfwSetFramebufferSizeCallback(mWindow, Callbacks::FramebufferSize);

	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(1);

	glfwGetWindowContentScale(mWindow, &mScale, &mScale);

	updateDLL();
	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);

}

void Host::updateWindowSize(int x, int y)
{
	mWidth = x;
	mHeight = y;
	mApplication->updateWindowSize(x, y);
}

void Host::updateDLL()
{
#if HOTLOAD
	Timestamp currentDllTimestamp;
	currentDllTimestamp.update(mDllPath);

	if (mDllTimestamp == currentDllTimestamp && !mForceReload)
		return;
	mForceReload = false;

	if (mApplication)
		mApplication->deinit();
	std::string dllName = mDllPath.substr(mDllPath.rfind('\\') + 1, std::string::npos);
	std::string tmpDLL = mDllPath;
	tmpDLL.insert(tmpDLL.find(dllName), "tmp");
	CopyFileA(mDllPath.c_str(), tmpDLL.c_str(), false);

	HMODULE tmpModule = LoadLibraryA(tmpDLL.c_str() + tmpDLL.find("tmp"));

	if (!tmpModule)
		return;
	if (mLib)
		FreeLibrary((HMODULE)mLib);

	ApplicationGetter *app = (ApplicationGetter *)GetProcAddress((HMODULE)tmpModule, "getApplicationPtr");

	if (!app)
	{
		FreeLibrary((HMODULE)tmpModule);
		return;
	}

	mApplication = app();
	mApplication->init(this);
	mDllTimestamp = currentDllTimestamp;
#else
	if (mApplication)
		return;

	static ShaderPixel app;
	mApplication = (Application *)&app;
	mApplication->init(this);
#endif
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
	GLFWwindow* backup_current_context = glfwGetCurrentContext();
	mApplication->renderUI();
	glfwMakeContextCurrent(backup_current_context);
	glfwSwapBuffers(mWindow);
}

Host::~Host()
{
    ImGui_ImplGlfw_Shutdown();
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

