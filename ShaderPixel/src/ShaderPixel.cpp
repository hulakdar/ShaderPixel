#include "ShaderPixel.h"
#include "Host.h"
#include "Resources.h"

#include <iostream>
#include <thread>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <stb_image.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>

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

	Host* host = staticHost();
	AppMemory* mem = host->mMemory;

	static float RunningOffset;

	static float Near = 1;
	static float Far = 90000;
	static float uPow = 1;

	Shader *defaultShader = Resources::GetShader(0);

	ImGui::SliderFloat("Near", &Near, 0.1f, 3.f);
	ImGui::SliderFloat("Far", &Far, 6.f, 90000.f);
	
	glm::mat4 cameraRotation = glm::rotate(
		glm::rotate(glm::mat4(1),
		mCameraAngles.y, glm::vec3(1, 0, 0)),
		mCameraAngles.x, glm::vec3(0, 1, 0)
	);

	glm::mat4 cameraTranslation = glm::translate(glm::mat4(1), mCameraPosition);
	glm::mat4 mViewProjectionMatrix =
		glm::perspective(glm::radians(65.0f), mAspectRatio, Near, Far)
		* cameraRotation * cameraTranslation;

	Renderer::Draw(mem->scene, defaultShader, mViewProjectionMatrix);
	RunningOffset += 0.01f;




	// camera movement 
	{
		mCameraForward = glm::vec3(0, 0, 1) * glm::mat3(cameraRotation);
		mCameraRight = glm::cross(mCameraUp, mCameraForward);

		bool SHIFT = glfwGetKey(host->mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

		float speed = mSpeed * ((float)SHIFT + 1.f);

		bool W = glfwGetKey(host->mWindow, GLFW_KEY_W) == GLFW_PRESS;
		bool S = glfwGetKey(host->mWindow, GLFW_KEY_S) == GLFW_PRESS;
		bool A = glfwGetKey(host->mWindow, GLFW_KEY_A) == GLFW_PRESS;
		bool D = glfwGetKey(host->mWindow, GLFW_KEY_D) == GLFW_PRESS;
		bool Q = glfwGetKey(host->mWindow, GLFW_KEY_Q) == GLFW_PRESS;
		bool E = glfwGetKey(host->mWindow, GLFW_KEY_E) == GLFW_PRESS;

		if (Q)
			mCameraPosition -= mCameraUp * speed;
		if (E)
			mCameraPosition += mCameraUp * speed;
		if (W)
			mCameraPosition += mCameraForward * speed;
		if (S)
			mCameraPosition -= mCameraForward * speed;
		if (A)
			mCameraPosition += mCameraRight * speed;
		if (D)
			mCameraPosition -= mCameraRight * speed;


		bool UP		= glfwGetKey(host->mWindow, GLFW_KEY_UP)	== GLFW_PRESS;
		bool DOWN	= glfwGetKey(host->mWindow, GLFW_KEY_DOWN)	== GLFW_PRESS;
		bool LEFT	= glfwGetKey(host->mWindow, GLFW_KEY_LEFT)	== GLFW_PRESS;
		bool RIGHT	= glfwGetKey(host->mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS;

		if (LEFT)
			mCameraAngles.x -= 0.1f;
		if (RIGHT)
			mCameraAngles.x += 0.1f;
		if (DOWN)
			mCameraAngles.y += 0.1f;
		if (UP)
			mCameraAngles.y -= 0.1f;
	}
}

ShaderPixel::~ShaderPixel()
{
	Resources::Clear();
}

void ShaderPixel::onMouseMove(float x, float y, float dX, float dY)
{
	(void)x;
	(void)y;

	if (glfwGetMouseButton(staticHost()->mWindow, GLFW_MOUSE_BUTTON_2) != GLFW_PRESS)
		return;

	mCameraAngles.x += dX / mWindowSize.x;
	mCameraAngles.y += dY / mWindowSize.y;
}

void ShaderPixel::onScroll(float x, float y)
{
	(void)x;

	mSpeed += y;
	mSpeed = glm::clamp(mSpeed, 0.f, 20.f);
}

void ShaderPixel::onKey(int key, int scancode, int action, int mods)
{
	ImGui::Text("%d %d %d %d", key, scancode, action, mods);
}

TextureData	LoadTextureData(const std::string& filename)
{
	uint8_t		*LocalBuffer;
	glm::ivec2	Size;
	int			ComponentCount;

	std::string CompleteFilepath = Resources::BaseFilepath + filename;
	LocalBuffer = stbi_load(CompleteFilepath.c_str(), &Size.x, &Size.y, &ComponentCount, 0);
	if (!LocalBuffer)
		__debugbreak();
	return { LocalBuffer, Size, ComponentCount };
}

Uniform PushTexture(const std::string& filename, TextureUsage usage)
{
	Uniform tmp;
	tmp.tex.id = Resources::PromisedTextureID();
	tmp.tex.usage = usage;
	tmp.type = UniformType::TEX;
	switch (usage)
	{
	case Diffuse:
		tmp.name = "uDiffuse";
		break;
	case Alpha:
		tmp.name = "uAlpha";
		break;
	case Noise:
		tmp.name = "uNoise";
		break;
	case Specular:
		break;
	case Specular_highlight:
		break;
	case Reflection:
		break;
	case Ambient:
		break;
	case Bump:
		break;
	case Displacement:
		break;
	default:
		break;
	}

	Resources::QueuedTextures.push_back(LoadTextureData(filename));
	return tmp;
}

void LoadMaterials(const std::vector<tinyobj::material_t>& materials)
{
	for (auto& It : materials)
	{
		MaterialID currentID = (MaterialID)Resources::Materials.size();
		Resources::Materials.emplace_back();
		Material* current = Resources::GetMaterial(currentID);

		FeatureMask mask = 0;
		if (!It.diffuse_texname.empty())
			current->uniforms.push_back(PushTexture(It.diffuse_texname, TextureUsage::Diffuse));
		else
			std::cerr << "Currently only textured materials supported.\n";

		if (!It.alpha_texname.empty())
		{
			current->uniforms.push_back(PushTexture(It.alpha_texname, TextureUsage::Alpha));
			current->blendMode = BlendMode::Masked;
			mask |= Feature::AlphaTexture;
			mask |= Feature::Masked;
			mask |= Feature::Dithered;
		}
		else
			current->blendMode = BlendMode::Opaque;

		if (mask)
			current->shaderOverride = Shader::GetShaderWithFeatures(mask);
	}
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
	Renderer::Init();
	assert(Resources::Textures.size() == 1);
	Resources::Textures.emplace_back("../content/BlueNoise64.tga");

	// hack. need to figure out async shader compilation
	Shader::GetShaderWithFeatures(7);

	mFXAA = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"../content/shaders/vertFXAA.shader",
	"../content/shaders/fragFXAA.shader");
	
	Scene &scene = host->mMemory->scene;

	tinyobj::ObjReader objReader;
	tinyobj::ObjReaderConfig config;
	config.vertex_color = false;
	objReader.ParseFromFile("../content/sponza/sponza.obj", config);

	std::string OldPath = "../content/sponza/";

	std::swap(OldPath, Resources::BaseFilepath);
	assert(objReader.Valid());

	auto& shapes = objReader.GetShapes();
	auto& attributes = objReader.GetAttrib();
	auto& materials = objReader.GetMaterials();

	std::thread MaterialLoader(&LoadMaterials, materials);
	//LoadMaterials(materials);

	scene.models.emplace_back();

	Resources::Meshes.reserve(shapes.size() * 20);
	for (size_t i = 0; i < shapes.size(); i++)
	{
		scene.models[0].mName = shapes[i].name;
		LoadMesh(shapes[i].mesh, attributes, scene.models[0]);
	}
	MaterialLoader.join();

	// undo the base path change
	std::swap(OldPath, Resources::BaseFilepath);
	Resources::FlushTextureData();
}

void ShaderPixel::deinit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void ShaderPixel::updateWindowSize(int x, int y)
{
	mWindowSize = {x,y};
	glViewport(0, 0, x, y);
	mAspectRatio = x / (float)y;
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
