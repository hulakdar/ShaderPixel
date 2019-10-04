#include "ShaderPixel.h"
#include "Host.h"
#include "Resources.h"
#include "Wrapper.h"

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Host* host = staticHost();
	AppMemory* mem = host->mMemory;

	static float Near = 1;
	static float Far = 90000;

	ImGui::SliderFloat("Near", &Near, 0.1f, 3.f);
	ImGui::SliderFloat("Far", &Far, 6.f, 90000.f);

	glm::mat4 cameraRotation = glm::rotate(
		glm::rotate(glm::mat4(1),
			mCameraAngles.y, glm::vec3(1, 0, 0)),
		mCameraAngles.x, glm::vec3(0, 1, 0)
	);

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

	glm::mat4 cameraTranslation = glm::translate(glm::mat4(1), -mCameraPosition);
	glm::mat4 viewProjection =
		glm::perspective(glm::radians(65.0f), mAspectRatio, Near, Far)
		* cameraRotation * cameraTranslation;

	shadowPass();

	static float fogParamA = .005f;
	static float fogParamB = .009f;

	ImGui::DragFloat("fogParamA",&fogParamA, 0.01);
	ImGui::DragFloat("fogParamB",&fogParamB, 0.01);

	glm::vec4 camvec4 = glm::vec4(mCameraPosition.x, mCameraPosition.y, mCameraPosition.z, 1.f);
	mGlobalBuffer.cameraPosition = camvec4;
	mGlobalBuffer.time = float(glfwGetTime());
	mGlobalBuffer.fogParamA = fogParamA;
	mGlobalBuffer.fogParamB = fogParamB;

	glBindBuffer(GL_UNIFORM_BUFFER, mGlobalBufferID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GlobalUniformBuffer), &mGlobalBuffer);

	if (mEnvMapDirty)
		captureEnvMap();

	setRenderTarget(&mSceneColorMS);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glCullFace(GL_BACK);
	Renderer::Draw(mem->scene, viewProjection);

	drawMandelbrot(viewProjection);
	drawMandelbox(viewProjection);

	// cloud
	if (1)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glDepthMask(false);

		Shader* cloud = Resources::GetShader(mCloud);

		glm::vec3 boxPos{ 0,750,0 };
		float boxScale = 5;

		Texture* CloudTexture = Resources::GetTexture(Texture::Cloud);
		CloudTexture->Bind();
		cloud->SetUniform("uVolume", (GLint)0);
		cloud->SetUniform("uCamPosMS", (mCameraPosition - boxPos) / boxScale);

		static float uDensity = 10.f;
		static float uShadowDensity = 1.f;
		ImGui::DragFloat("uDensity", &uDensity, .1f);
		ImGui::DragFloat("uShadowDensity", &uShadowDensity, .1f);
		cloud->SetUniform("uDensity", uDensity);
		cloud->SetUniform("uShadowDensity", uShadowDensity);

		Renderer::DrawCubeWS(boxPos, boxScale, cloud, viewProjection);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDepthMask(true);
	}


	glBindFramebuffer(GL_READ_FRAMEBUFFER, mSceneColorMS.rendererID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mPingPong[0].rendererID); 

	glBlitFramebuffer(0, 0, mSceneColorMS.size.x, mSceneColorMS.size.y,
					  0, 0, mPingPong[0].size.x, mPingPong[0].size.y,  
					  GL_COLOR_BUFFER_BIT,							
					  GL_LINEAR);

	glDepthMask(false);
	glDisable(GL_DEPTH_TEST);

	setRenderTarget(&Renderer::viewport);

	static bool bFxaa = true;
	ImGui::Begin("Postprocess");
	ImGui::Checkbox("FXAA", &bFxaa);
	if (bFxaa)
	{
		Shader* shader = Resources::GetShader(mFXAA);
		shader->Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mPingPong[0].textures[0]);
		shader->SetUniform("uInputTex", 0);
		shader->SetUniform("rcpFrame", 1.f / glm::vec2(mPingPong[0].size));
	}
	else
	{
		Shader* passthrough = Resources::GetShader(mFullscreenTest);
		passthrough->Bind();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mPingPong[0].textures[0]);
		passthrough->SetUniform("uInputTex", 0);
	}

	Renderer::DrawQuadFS();

	static bool bBloom = true;
	ImGui::Checkbox("Bloom", &bBloom);
	if (bBloom)
	{
		ImGui::BeginGroup();

		// bright filter
		setRenderTarget(&mPingPong[1]);
		Shader* shader = Resources::GetShader(mBrightnessFilter);
		shader->Bind();

		static float brightThreshold = 0.472f;
		ImGui::SliderFloat("brightThreshold", &brightThreshold, 0.f, 1.f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mPingPong[0].textures[0]);
		shader->SetUniform("uInputTex", 0);
		shader->SetUniform("uBrightnessThreshold", brightThreshold);

		Renderer::DrawQuadFS();

		static bool bDownsampledBloom = false;
		ImGui::Checkbox("bDownsampledBloom", &bDownsampledBloom);

		Shader* blurShader = Resources::GetShader(mBlur);

		glActiveTexture(GL_TEXTURE0);
		blurShader->SetUniform("uInputTex", 0);

		static float blurOffset = 2.f;
		ImGui::SliderFloat("blurOffset ", &blurOffset , 1, 10);

		Shader* passthrough = Resources::GetShader(mFullscreenTest);
		passthrough->Bind();

		static float alpha = 0.597f;
		ImGui::SliderFloat("alpha", &alpha, 0, 1);
		glBindTexture(GL_TEXTURE_2D, mPingPong[1].textures[0]);
		passthrough->SetUniform("uInputTex", 0);
		passthrough->SetUniform("uAlpha", alpha);

		if (bDownsampledBloom)
		{
			glBindFramebuffer(GL_READ_FRAMEBUFFER, mPingPong[1].rendererID);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mHalfRes[0].rendererID);

			glBlitFramebuffer(0, 0, mPingPong[1].size.x, mPingPong[1].size.y,
				0, 0, mHalfRes[0].size.x, mHalfRes[0].size.y,
				GL_COLOR_BUFFER_BIT,
				GL_LINEAR);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, mHalfRes[0].rendererID);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mQuarterRes[0].rendererID);

			glBlitFramebuffer(0, 0, mHalfRes[0].size.x, mHalfRes[0].size.y,
				0, 0, mQuarterRes[0].size.x, mQuarterRes[0].size.y,
				GL_COLOR_BUFFER_BIT,
				GL_LINEAR);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, mQuarterRes[0].rendererID);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mEightsRes[0].rendererID);

			glBlitFramebuffer(0, 0, mQuarterRes[0].size.x, mQuarterRes[0].size.y,
				0, 0, mEightsRes[0].size.x, mEightsRes[0].size.y,
				GL_COLOR_BUFFER_BIT,
				GL_LINEAR);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, mEightsRes[0].rendererID);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mSixteenthsRes[0].rendererID);

			glBlitFramebuffer(0, 0, mEightsRes[0].size.x, mEightsRes[0].size.y,
				0, 0, mSixteenthsRes[0].size.x, mSixteenthsRes[0].size.y,
				GL_COLOR_BUFFER_BIT,
				GL_LINEAR);


			// blur 1/16

			// horizontally
			setRenderTarget(&mSixteenthsRes[1]);
			glBindTexture(GL_TEXTURE_2D, mSixteenthsRes[0].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(0, blurOffset));
			Renderer::DrawQuadFS();

			// vertically
			setRenderTarget(&mSixteenthsRes[0]);
			glBindTexture(GL_TEXTURE_2D, mSixteenthsRes[1].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(blurOffset, 0));
			Renderer::DrawQuadFS();

			// blend 1/16 on 1/8
			setRenderTarget(&mEightsRes[0]);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			passthrough->Bind();
			glBindTexture(GL_TEXTURE_2D, mSixteenthsRes[0].textures[0]);
			Renderer::DrawQuadFS();

			glDisable(GL_BLEND);

			// blur 1/8

			// horizontally
			setRenderTarget(&mEightsRes[1]);
			glBindTexture(GL_TEXTURE_2D, mEightsRes[0].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(0, blurOffset));
			Renderer::DrawQuadFS();

			// vertically
			setRenderTarget(&mEightsRes[0]);
			glBindTexture(GL_TEXTURE_2D, mEightsRes[1].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(blurOffset, 0));
			Renderer::DrawQuadFS();

			// blend 1/8 on 1/4
			setRenderTarget(&mQuarterRes[0]);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			passthrough->Bind();
			glBindTexture(GL_TEXTURE_2D, mEightsRes[0].textures[0]);
			Renderer::DrawQuadFS();

			glDisable(GL_BLEND);

			// blur 1/4

			// horizontally
			setRenderTarget(&mQuarterRes[1]);
			glBindTexture(GL_TEXTURE_2D, mQuarterRes[0].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(0, blurOffset));
			Renderer::DrawQuadFS();

			// vertically
			setRenderTarget(&mQuarterRes[0]);
			glBindTexture(GL_TEXTURE_2D, mQuarterRes[1].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(blurOffset, 0));
			Renderer::DrawQuadFS();

			// blend 1/4 on 1/2
			setRenderTarget(&mHalfRes[0]);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			passthrough->Bind();
			glBindTexture(GL_TEXTURE_2D, mQuarterRes[0].textures[0]);
			Renderer::DrawQuadFS();

			glDisable(GL_BLEND);

			// blur 1/2

			// horizontally
			setRenderTarget(&mHalfRes[1]);
			glBindTexture(GL_TEXTURE_2D, mHalfRes[0].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(0, blurOffset));
			Renderer::DrawQuadFS();

			// vertically
			setRenderTarget(&mHalfRes[0]);
			glBindTexture(GL_TEXTURE_2D, mHalfRes[1].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(blurOffset, 0));
			glBindTexture(GL_TEXTURE_2D, mHalfRes[0].textures[0]);
			Renderer::DrawQuadFS();

			// blend 1/2 on Viewport
			setRenderTarget(&Renderer::viewport);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			passthrough->Bind();
			glBindTexture(GL_TEXTURE_2D, mHalfRes[0].textures[0]);
			Renderer::DrawQuadFS();

			glDisable(GL_BLEND);
		}
		else
		{
			// 1 blur
			setRenderTarget(&mPingPong[0]);

			glBindTexture(GL_TEXTURE_2D, mPingPong[1].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(blurOffset, 0));
			Renderer::DrawQuadFS();

			setRenderTarget(&mPingPong[1]);

			glBindTexture(GL_TEXTURE_2D, mPingPong[0].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(0, blurOffset));
			Renderer::DrawQuadFS();

			// 2 blur
			setRenderTarget(&mPingPong[0]);

			glBindTexture(GL_TEXTURE_2D, mPingPong[1].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(blurOffset, 0));
			Renderer::DrawQuadFS();

			setRenderTarget(&mPingPong[1]);

			glBindTexture(GL_TEXTURE_2D, mPingPong[0].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(0, blurOffset));
			Renderer::DrawQuadFS();

			setRenderTarget(&Renderer::viewport);

			// 3 blur
			setRenderTarget(&mPingPong[0]);

			glBindTexture(GL_TEXTURE_2D, mPingPong[1].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(blurOffset, 0));
			Renderer::DrawQuadFS();

			setRenderTarget(&mPingPong[1]);

			glBindTexture(GL_TEXTURE_2D, mPingPong[0].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(0, blurOffset));
			Renderer::DrawQuadFS();

			// apply bloom effect
			setRenderTarget(&Renderer::viewport);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			glBindTexture(GL_TEXTURE_2D, mPingPong[1].textures[0]);
			passthrough->SetUniform("uAlpha", alpha);
			Renderer::DrawQuadFS();

			glDisable(GL_BLEND);

		}
		
		ImGui::EndGroup();
	}

	ImGui::End();

	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
}

ShaderPixel::~ShaderPixel()
{
	Resources::Clear();
}

void ShaderPixel::shadowPass()
{
	Host* host = staticHost();
	AppMemory* mem = host->mMemory;

	static bool shadowPassDirty = true;
	ImGui::Begin("Lights");
	shadowPassDirty |= ImGui::SliderAngle("lightX", &mLightAngles.x);
	shadowPassDirty |= ImGui::SliderAngle("lightY", &mLightAngles.y);
	shadowPassDirty |= ImGui::SliderAngle("lightZ", &mLightAngles.z);

	static glm::vec2	LR = {-2650, 2650};
	static glm::vec2	BT = {-2100, 2100};
	static glm::vec2	NF = {-2800, 2600};

	ImGui::DragFloat2("LR", &LR[0]);
	ImGui::DragFloat2("BT", &BT[0]);
	ImGui::DragFloat2("NF", &NF[0]);
	ImGui::End();

	if (!shadowPassDirty)
		return;

	glm::mat4 shadowView =
		glm::rotate(
		glm::rotate(
		glm::rotate(
			glm::mat4(1.f),	
			mLightAngles.x, glm::vec3(1,0,0)),
			mLightAngles.y, glm::vec3(0,1,0)),
			mLightAngles.z, glm::vec3(0,0,1));
	glm::mat4 shadowProjection = glm::ortho(LR[0], LR[1], BT[0], BT[1], NF[0], NF[1]);

	glm::mat4 shadowTransform = shadowProjection * shadowView;

	setRenderTarget(&mShadow);
	glClear(GL_DEPTH_BUFFER_BIT);

	glCullFace(GL_FRONT);
	// shadow
	Renderer::Draw(mem->scene, shadowTransform, Feature::ShadowPass);

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, mShadow.textures[1]);

	static const glm::vec4 LIGHT_DIR{ 0,0,1,0 };

	// scene
	mGlobalBuffer.lightDir = glm::normalize(glm::inverse(shadowView) * LIGHT_DIR);
	mGlobalBuffer.lightView = shadowTransform;
	shadowPassDirty = false;
	markEnvMapAsDirty();
}

void ShaderPixel::markEnvMapAsDirty()
{
	mEnvMapDirty = true;
	mFaceIndex = 0;
}

void ShaderPixel::captureEnvMap()
{
	Host* host = staticHost();
	AppMemory* mem = host->mMemory;
	glCullFace(GL_BACK);

	setRenderTarget(&mEnvProbe);
	glm::vec3 targetVectors[6] = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)
	};
	glm::vec3 upVectors[6] = {
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)
	};

	const static glm::vec3 probePosition{ 0, 400, 0 };

	const float fov = 90.f;
	glm::mat4 projection = glm::perspective(glm::radians(fov), 1.f, .1f, 9000.f);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X + mFaceIndex, mEnvProbe.textures[RenderTarget::Color], 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 view = glm::lookAt(probePosition, probePosition + targetVectors[mFaceIndex], upVectors[mFaceIndex]);
	Renderer::Draw(mem->scene, projection * view);

	mFaceIndex++;
	if (mFaceIndex >= 6)
	{
		mFaceIndex = 0;
		mEnvMapDirty = false;
	}
}

void ShaderPixel::onMouseMove(float x, float y, float dX, float dY)
{
	(void)x;
	(void)y;

	if (glfwGetMouseButton(staticHost()->mWindow, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		mCameraAngles.x += dX / mWindowSize.x;
		mCameraAngles.y += dY / mWindowSize.y;
	}
	else if (glfwGetMouseButton(staticHost()->mWindow, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		//mLightAngles.x += dX / mWindowSize.x;
		//mLightAngles.y += dY / mWindowSize.y;
	}
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
	if (desiredComponentCount)
		ComponentCount = desiredComponentCount;
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

		current->features = mask;
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

	assert(Resources::Textures.size() == 2);
	TextureData NoiseTexture = LoadTextureData("../content/PerlinNoise.png", 1);
	CloudTexture.Type = GL_TEXTURE_2D;
	Resources::Textures.emplace_back(CloudTexture);


	// hack. need to figure out async shader compilation (look at how Textures are loaded. should be pretty similar)
	Shader::GetShaderWithFeatures(7);
	// hack.

	mShadow = makeRenderTargetShadow(glm::ivec2(4096*2, 4096*2));

	const glm::ivec2 resolution{1024, 1024};

	mSceneColorMS = makeRenderTargetMultisampled(resolution, GL_RGB, 8);
	mPingPong[0] = makeRenderTarget(resolution, GL_RGB, true);
	mPingPong[1] = makeRenderTarget(resolution, GL_RGB, true);
	mHalfRes[0] = makeRenderTarget(resolution/2, GL_RGB, true);
	mHalfRes[1] = makeRenderTarget(resolution/2, GL_RGB, true);
	mQuarterRes[0] = makeRenderTarget(resolution/4, GL_RGB, true);
	mQuarterRes[1] = makeRenderTarget(resolution/4, GL_RGB, true);
	mEightsRes[0] = makeRenderTarget(resolution/8, GL_RGB, true);
	mEightsRes[1] = makeRenderTarget(resolution/8, GL_RGB, true);
	mSixteenthsRes[0] = makeRenderTarget(resolution/16, GL_RGB, true);
	mSixteenthsRes[1] = makeRenderTarget(resolution/16, GL_RGB, true);
	mEnvProbe = makeRenderTargetCube(glm::ivec2(512), true);

	GLCall(glGenBuffers(1, &mGlobalBufferID));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, mGlobalBufferID));
	GLCall(glBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalUniformBuffer),
		&mGlobalBuffer, GL_DYNAMIC_DRAW));
	GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 0, mGlobalBufferID));


	mMandelbrot = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"../content/shaders/vertWorldSpace.shader",
	"../content/shaders/fragMandelbrot.shader");

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

	mBrightnessFilter = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"../content/shaders/vertFullscreen.shader",
	"../content/shaders/fragBrightFilter.shader");

	mBloom = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"../content/shaders/vertFullscreen.shader",
	"../content/shaders/fragBloom.shader");

	mBlur = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"../content/shaders/vertFullscreen.shader",
	"../content/shaders/fragBlur.shader");

	mFullscreenTest = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"../content/shaders/vertFullscreen.shader",
	"../content/shaders/fragFullscreenTest.shader");

	mCubeMapTest = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"../content/shaders/vertWorldSpace.shader",
	"../content/shaders/fragReadCubemap.shader");

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

void ShaderPixel::drawMandelbox(glm::mat4 viewProjection)
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

void ShaderPixel::drawMandelbrot(glm::mat4 viewProjection)
{
	Shader *mandel = Resources::GetShader(mMandelbrot);

	static glm::vec3 QuadPos{ -50,140,-263 };
	static glm::vec2 QuadScale{ 145,111 };
	static glm::vec2 QuadPos2 = glm::vec2(QuadPos.x, QuadPos.z);

	static glm::vec2 MandelPos{ 0,0 };
	static float MandelScale{ 2 };
	static float MandelIter{ 20 };

	glm::vec2 cameraPos2 = glm::vec2(mCameraPosition.x, mCameraPosition.z);
	if (glm::distance(cameraPos2, QuadPos2) < 450)
	{
		ImGui::DragFloat2("MandelPos", &MandelPos[0], 0.001f);
		ImGui::DragFloat("MandelScale", &MandelScale, 0.0001f);
		ImGui::DragFloat("MandelIter", &MandelIter);
	}
	mandel->Bind();
	mandel->SetUniform("uCenter", MandelPos);
	mandel->SetUniform("uScale", MandelScale);
	mandel->SetUniform("uIter", MandelIter);

	Renderer::DrawQuadWS(QuadPos, QuadScale, mandel, viewProjection);
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
