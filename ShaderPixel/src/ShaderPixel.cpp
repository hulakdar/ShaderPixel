#include "ShaderPixel.h"
#include "Host.h"
#include <iostream>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <tiny_obj_loader.h>


extern "C"
__declspec(dllexport)
Application* getApplicationPtr()
{
	static ShaderPixel sShaderPixel;
	return &sShaderPixel;
}

Host*& staticHost()
{
	static Host *sHost;
	return sHost;
}

Host& getHost()
{
	return *staticHost();
}

void *operator new(size_t size)
{
	void * p = getHost().allocate(size);
	return p;
}

void operator delete(void * p)
{
	getHost().deallocate(p);
}

void ShaderPixel::update()
{
	AppMemory *mem = getHost().mMemory;
	glClearColor(1, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Shader defaultShader = mem->shaderManager.getShader(
				"content/shaders/vertDefault.shader",
				"content/shaders/fragDefault.shader");
	mem->shaderManager.updatePrograms();
	defaultShader.Bind();

	ImGui::Text("Test");
	glDrawArrays(GL_TRIANGLES, 0, 3);
}


void ShaderPixel::onKey(int key, int scancode, int action, int mods)
{
	ImGui::TextColored(ImVec4(1, 0.5, 1, 1), "%d %d %d %d", key, scancode, action, mods);
}

void ShaderPixel::init(Host *host, GLADloadproc getProc)
{
	staticHost() = host;
	gladLoadGL(); //gladLoadGLLoader(getProc); 

	host->mMemory = new AppMemory();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
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


	//main
	//mShaderManager.GetShader("kek", "lol");

	/*
	tinyobj::ObjReader objReader;
	objReader.ParseFromFile("content/sponza/sponza.obj");
	assert(objReader.Valid());

	auto& shapes = objReader.GetShapes();
	auto& attributes = objReader.GetAttrib();
	auto& vertices = attributes.GetVertices();
	*/

	float vertices[] = {
	   -0.6f, -0.4f, 0.f,
		0.6f, -0.4f, 0.f,
		0.f,   0.6f, 0.f
	};

	GLuint vertex_buffer, vertex_array;

	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(float) * 3, (void*)0);
}

void ShaderPixel::deinit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
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

