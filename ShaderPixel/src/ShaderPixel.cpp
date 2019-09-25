#include "ShaderPixel.h"
#include "Host.h"
#include "Resources.h"

#include <iostream>
#include <thread>

#define _USE_MATH_DEFINES 
#include <math.h>
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <stb_image.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include "Collision.h"

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

	static float Near = 1;
	static float Far = 90000;
	static float uPow = 1;

	Shader* defaultShader = Resources::GetShader(0);

	ImGui::SliderFloat("Near", &Near, 0.1f, 3.f);
	ImGui::SliderFloat("Far", &Far, 6.f, 90000.f);

	glm::mat4 cameraRotation = glm::rotate(
		glm::rotate(glm::mat4(1),
			mCameraAngles.y, glm::vec3(1, 0, 0)),
		mCameraAngles.x, glm::vec3(0, 1, 0)
	);

	glm::mat4 cameraTranslation = glm::translate(glm::mat4(1), -mCameraPosition);
	glm::mat4 viewProjection =
		glm::perspective(glm::radians(65.0f), mAspectRatio, Near, Far)
		* cameraRotation * cameraTranslation;

	setRenderTarget(&mSceneColorMS);
	//setRenderTarget(&Renderer::viewport);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// scene
	Renderer::Draw(mem->scene, defaultShader, viewProjection);
	if (1)
		Renderer::DrawMandelbrot(mCameraPosition, viewProjection);

	// Mandelbox
	if (1)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		Shader* manbox = Resources::GetShader(mBox);

		static glm::vec3 boxPos{ 487, 142, 144 };
		ImGui::DragFloat3("Mandelbox position", &boxPos[0]);
		static float boxScale = 1.1f;
		ImGui::DragFloat("Mandelbox scale", &boxScale, 0.1f);


		static float uInvDistThreshold = 0;
		ImGui::SliderFloat("uInvDistThreshold", &uInvDistThreshold, 0.000001f, .2f);
		manbox->SetUniform("uInvDistThreshold", uInvDistThreshold);

		manbox->SetUniform("uCamPosMS", (mCameraPosition - boxPos) / boxScale);

		Renderer::DrawCubeWS(boxPos, boxScale, manbox, viewProjection);
	}

	// cloud
	if (1)
	{
		//glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glDepthMask(false);

		Shader* cloud = Resources::GetShader(mCloud);

		glm::vec3 boxPos{ 0,750,0 };
		float boxScale = 5;

		Texture* CloudTexture = Resources::GetTexture(Texture::Cloud);
		CloudTexture->Bind();
		cloud->SetUniform("uVolume", (GLint)0);
		cloud->SetUniform("uCamPosMS", (mCameraPosition - boxPos) / boxScale);

		Renderer::DrawCubeWS(boxPos, boxScale, cloud, viewProjection);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDepthMask(true);
	}

	glBindFramebuffer(GL_READ_FRAMEBUFFER, mSceneColorMS.rendererID); // src FBO (multi-sample)
glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSceneColor.rendererID);     // dst FBO (single-sample)

glBlitFramebuffer(0, 0, mSceneColorMS.size.x, mSceneColorMS.size.x, // src rect
                  0, 0, mSceneColor.size.x, mSceneColor.size.x,     // dst rect
                  GL_COLOR_BUFFER_BIT,								// buffer mask
                  GL_LINEAR);

	setRenderTarget(&Renderer::viewport);

	static bool bFxaa;
	ImGui::Checkbox("FXAA", &bFxaa);
	if (bFxaa)
	{
		Shader* fxaa = Resources::GetShader(mFXAA);
		fxaa->Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mSceneColor.textures[0]);
		fxaa->SetUniform("uInputTex", 0);
		fxaa->SetUniform("rcpFrame", 1.f / glm::vec2(512.f));

		static float FXAA_SPAN_MAX = 8;
		ImGui::DragFloat("FXAA_SPAN_MAX", &FXAA_SPAN_MAX, 0.1f);
		fxaa->SetUniform("FXAA_SPAN_MAX", FXAA_SPAN_MAX);

		static float FXAA_REDUCE_MUL = 0.1f;
		ImGui::SliderFloat("FXAA_REDUCE_MUL", &FXAA_REDUCE_MUL, 0.f, 1.f);
		fxaa->SetUniform("FXAA_REDUCE_MUL", FXAA_REDUCE_MUL);
	}
	else
	{
		Shader* passthrough = Resources::GetShader(mFullscreenTest);
		passthrough->Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mSceneColor.textures[0]);
		passthrough->SetUniform("uInputTex", 0);
		passthrough->SetUniform("rcpFrame", 1.f / glm::vec2(512.f));
	}

	Renderer::DrawQuadFS();

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
			mCameraPosition -= mCameraForward * speed;
		if (S)
			mCameraPosition += mCameraForward * speed;
		if (A)
			mCameraPosition -= mCameraRight * speed;
		if (D)
			mCameraPosition += mCameraRight * speed;


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

		if (mCameraAngles.x > 2 * M_PI)
			mCameraAngles.x -= float(2 * M_PI);

		if (mCameraAngles.x <= -2 * M_PI)
			mCameraAngles.x += float(2 * M_PI);

		mCameraAngles.y = glm::clamp(mCameraAngles.y, -float(M_PI_2), float(M_PI_2));
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

	mSpeed += y * 0.01f;
	mSpeed = glm::clamp(mSpeed, 0.001f, 20.f);
}

void ShaderPixel::onKey(int key, int scancode, int action, int mods)
{
	//ImGui::Text("%d %d %d %d", key, scancode, action, mods);
}

TextureData	LoadTextureData(const std::string& filename, int desiredComponentCount = 0)
{
	uint8_t		*LocalBuffer;
	glm::ivec2	Size;
	int			ComponentCount;

	std::string CompleteFilepath = Resources::BaseFilepath + filename;
	LocalBuffer = stbi_load(CompleteFilepath.c_str(), &Size.x, &Size.y, &ComponentCount, desiredComponentCount);
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

void LoadMaterials(const std::vector<tinyobj::material_t>* materials)
{
	for (auto& It : *materials)
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
	stbi_set_flip_vertically_on_load(false);
	TextureData CloudTexture = LoadTextureData("../content/VolumeCloud.tga", 1);
	stbi_set_flip_vertically_on_load(true);
	CloudTexture.Slices = 12;
	CloudTexture.Type = GL_TEXTURE_3D;
	Resources::Textures.emplace_back(CloudTexture);


	// hack. need to figure out async shader compilation (look at how Textures are loaded. should be pretty similar)
	Shader::GetShaderWithFeatures(7);
	// hack.

	mSceneColorMS = makeRenderTargetMultisampled(glm::ivec2(1024,1024), GL_RGB, 8);
	mSceneColor = makeRenderTarget(glm::ivec2(1024,1024), GL_RGB, true);

	mBox = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"../content/shaders/vertWorldSpace.shader",
	"../content/shaders/fragMandelbox.shader");

	mCloud = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"../content/shaders/vertWorldSpace.shader",
	"../content/shaders/fragCloud.shader");

	mFXAA = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"../content/shaders/vertFXAA.shader",
	"../content/shaders/fragFXAA.shader");


	mFullscreenTest = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"../content/shaders/vertFullscreen.shader",
	"../content/shaders/fragFullscreenTest.shader");

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

	std::thread MaterialLoader(&LoadMaterials, &materials);
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
	//glViewport(0, 0, x, y);
	mAspectRatio = x / (float)y;
	Renderer::Update(x, y);
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
