#include "Application.h"
#include "UserInterface.h"
#include <iostream>
#include <fstream>

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
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

#if 0
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
	ImVec2 scale;
	glfwGetWindowContentScale(mWindow, &scale.x, &scale.y);
	assert(scale.x == scale.y);
	ImFontConfig cfg;
	cfg.SizePixels = 13 * scale.x;
	ImGui::GetIO().Fonts->AddFontDefault(&cfg)->DisplayOffset = scale;

    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 410 core");
#endif

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	ImGui::StyleColorsDark();
	ImVec2 scale;
	glfwGetWindowContentScale(mWindow, &scale.x, &scale.y);
	assert(scale.x == scale.y);
	ImFontConfig cfg;
	cfg.SizePixels = 13 * scale.x;
	ImGui::GetIO().Fonts->AddFontDefault(&cfg)->DisplayOffset = scale;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 410 core"); //ImGui_ImplOpenGL3_Init(glsl_version);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

}

void Application::updateWindowSize(int x, int y)
{
	mWidth = x;
	mHeight = y;
	//float xScale, yScale;
	//glfwGetWindowContentScale(mWindow, &xScale, &yScale);
	glViewport(0, 0, mWidth, mHeight);
}

bool Application::shouldClose()
{
	return glfwWindowShouldClose(mWindow);
}

void Application::update()
{
	glfwPollEvents();
	mShaderManager.UpdatePrograms();
}

void Application::swapBuffers()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
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


namespace Utils {

	std::string StringFromFile(const std::string& filename)
	{
		std::ifstream fs(filename);
		if (fs.bad())
		{
			return "";
		}

		std::string s(
			std::istreambuf_iterator<char>{fs},
			std::istreambuf_iterator<char>{});

		return s;
	}

}
