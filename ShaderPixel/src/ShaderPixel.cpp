#include "ShaderPixel.h"
#include "Host.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <stb_image.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <tiny_obj_loader.h>
#include "Renderer.h"

Host*& staticHost()
{
	static Host *sHost;
	return sHost;
}

Host& getHost()
{
	return *staticHost();
}

#if HOTLOAD

void *operator new(size_t size)
{
	void * p = getHost().allocate(size);
	return p;
}

void operator delete(void * p)
{
	getHost().deallocate(p);
}
#endif

void ShaderPixel::update()
{
//	AppMemory* mem = getHost().mMemory;
//	glClearColor(1, 1, 1, 1);
//	while (glGetError())
//		continue;
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	if (auto kek = glGetError())
//		std::cout << kek;
//
//	Shader defaultShader = mem->shaderManager.getShader(
//		"../content/shaders/vertDefault.shader",
//		"../content/shaders/fragDefault.shader");
//
//	mem->shaderManager.updatePrograms();
//	defaultShader.Bind();
//	defaultShader.Uniform("time", glfwGetTime());

	const glm::vec3 CubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mViewProjectionMatrix = glm::perspective(glm::radians(65.0f), 1.f, 0.1f, 100.0f) * glm::translate(glm::mat4(1), glm::vec3(0.0f, 5 * sin(mRotationAngle), -3.0f));

	const glm::vec3 LightPosition(0.f, 0.f, -10.f);

	mProgram.SetUniform("uViewProjection", mViewProjectionMatrix);
	mProgram.SetUniform("time", (float)glfwGetTime());
	float angle = mRotationAngle;
	for (auto& Position : CubePositions)
	{
		const glm::mat4 Model = glm::rotate(glm::translate(glm::mat4(1.f), Position), mRotationAngle, glm::vec3(0.f, 1.f, 0.f));

		mProgram.SetUniform("uModel", Model);
		Renderer::Draw(mVAO, mProgram, 36);
	}

	mRotationAngle += 0.001f;
}


void ShaderPixel::onKey(int key, int scancode, int action, int mods)
{
	ImGui::Text("%d %d %d %d", key, scancode, action, mods);
}

void ShaderPixel::init(Host* host)
{
	staticHost() = host;

	if (!host->mMemory)
	{
		host->mMemory = new AppMemory();
	}

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

	ImGui::StyleColorsDark();
	ImFontConfig cfg;
	cfg.SizePixels = 13 * host->mScale;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
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

	{

	tinyobj::ObjReader objReader;
	objReader.ParseFromFile("../content/sibenik/sibenik.obj");
	assert(objReader.Valid());

	auto& shapes = objReader.GetShapes();
	auto& attributes = objReader.GetAttrib();
	auto& materials = objReader.GetMaterials();
	auto& vertices = attributes.GetVertices();
}

	float vertices[] = {
	   -0.6f, -0.4f, 1.f,
		0.6f, -0.4f, 1.f,
		0.f,   0.6f, 1.f
	};

	VertexBufferLayout	mVBL;
	mVBL.Push<float>(3);
	mVBL.Push<float>(2);
	mVBL.Push<float>(3);
	mVAO.AddBuffer(mVBO, mVBL);
}

void ShaderPixel::deinit()
{
    //ImGui_ImplOpenGL3_Shutdown();
    //ImGui::DestroyContext();
}

void ShaderPixel::updateWindowSize(int x, int y)
{
	glViewport(0, 0, x, y);
}

void ShaderPixel::renderUI()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ShaderPixel::preframe()
{
	ImGui_ImplOpenGL3_NewFrame();
}

extern "C"
__declspec(dllexport)
Application* getApplicationPtr()
{
	static ShaderPixel sShaderPixel;
	return &sShaderPixel;
}
