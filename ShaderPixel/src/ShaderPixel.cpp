#include <glad/glad.h>
// for some reason it should be first

#include "ShaderPixel.h"
#include "Resources.h"
#include "Collision.h"
#include "Utility.h"

#include <iostream>
#include <thread>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <stb_image.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <tiny_obj_loader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <imgui_impl_glfw.h>

#define _USE_MATH_DEFINES 
#include <math.h>

glm::mat4 MakeInfReversedZProjRH(float fovY_radians, float aspectWbyH, float zNear)
{
    float f = 1.0f / tan(fovY_radians / 2.0f);
    return glm::mat4(
        f / aspectWbyH, 0.0f,  0.0f,  0.0f,
                  0.0f,    f,  0.0f,  0.0f,
                  0.0f, 0.0f,  0.0f, -1.0f,
                  0.0f, 0.0f, zNear,  0.0f);
}

void ShaderPixel::update()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static float Near = 1;
	ImGui::SliderFloat("Near", &Near, .1f, 90000.f);

	double lastTime = mCurrentTime;

	mCurrentTime = glfwGetTime();

	float deltaTime = float(mCurrentTime - lastTime);

	ImGui::Text("%f", deltaTime);

	glm::mat4 cameraRotation = glm::rotate(
		glm::rotate(glm::mat4(1),
			mCameraAngles.y, glm::vec3(1, 0, 0)),
		mCameraAngles.x, glm::vec3(0, 1, 0)
	);

	GLFWwindow *window = glfwGetCurrentContext();

	// camera movement 
	{
		mCameraForward = glm::vec3(0, 0, 1) * glm::mat3(cameraRotation);
		mCameraRight = glm::cross(mCameraUp, mCameraForward);

		bool SHIFT = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

		float speed = mSpeed * ((float)SHIFT + 1.f);

		bool W = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
		bool S = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
		bool A = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
		bool D = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
		bool Q = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
		bool E = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

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
	}

	glm::mat4 cameraTranslation = glm::translate(glm::mat4(1), -mCameraPosition);
	glm::mat4 viewProjection =
		MakeInfReversedZProjRH(glm::radians(65.0f), mAspectRatio, Near) // glm::perspective(glm::radians(65.0f), mAspectRatio, Near, Far)
		* cameraRotation * cameraTranslation;

	shadowPass();

	glm::vec4 camvec4 = glm::vec4(mCameraPosition.x, mCameraPosition.y, mCameraPosition.z, 1.f);
	mGlobalBuffer.cameraPosition = camvec4;
	mGlobalBuffer.time = float(mCurrentTime);

	glBindBuffer(GL_UNIFORM_BUFFER, mGlobalBufferID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GlobalUniformBuffer), &mGlobalBuffer);

	if (mEnvMapDirty)
		captureEnvMap();

	setRenderTarget(&mSceneColorMS);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glCullFace(GL_BACK);
	Renderer::Draw(scene, viewProjection);

	drawMandelbrot(viewProjection);
	drawMandelbox(viewProjection);
	drawCloud(viewProjection);

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
	ImGui::Begin("Post-processing");
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

		static bool bDownsampledBloom = true;
		ImGui::Checkbox("bDownsampledBloom", &bDownsampledBloom);

		Shader* blurShader = Resources::GetShader(mBlur);

		glActiveTexture(GL_TEXTURE0);
		blurShader->SetUniform("uInputTex", 0);

		Shader* passthrough = Resources::GetShader(mFullscreenTest);
		passthrough->Bind();

		static float alpha = 0.597f;
		ImGui::SliderFloat("alpha", &alpha, 0, 1);
		glBindTexture(GL_TEXTURE_2D, mPingPong[1].textures[0]);
		passthrough->SetUniform("uInputTex", 0);
		passthrough->SetUniform("uAlpha", alpha);

		static float blurOffset = 1.f;

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

			// blend 1/2 on 1/1
			setRenderTarget(&mPingPong[1]);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			passthrough->Bind();
			glBindTexture(GL_TEXTURE_2D, mHalfRes[0].textures[0]);
			Renderer::DrawQuadFS();

			glDisable(GL_BLEND);

			// blur 1/1

			// horizontally
			setRenderTarget(&mPingPong[0]);
			glBindTexture(GL_TEXTURE_2D, mPingPong[0].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(0, blurOffset));
			Renderer::DrawQuadFS();

			// vertically
			setRenderTarget(&mPingPong[1]);
			glBindTexture(GL_TEXTURE_2D, mPingPong[1].textures[0]);
			blurShader->SetUniform("uBlurDir", glm::vec2(blurOffset, 0));
			glBindTexture(GL_TEXTURE_2D, mPingPong[1].textures[0]);
			Renderer::DrawQuadFS();

			// blend 1/1 on viewport
			setRenderTarget(&Renderer::viewport);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

			passthrough->Bind();
			glBindTexture(GL_TEXTURE_2D, mPingPong[1].textures[0]);
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

    ImGui_ImplGlfw_Shutdown();
	glfwDestroyWindow(mWindow);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwTerminate();
}

void ShaderPixel::shadowPass()
{
	ImGui::Begin("Lights");
	mShadowPassDirty |= ImGui::SliderAngle("lightX", &mLightAngles.x);
	mShadowPassDirty |= ImGui::SliderAngle("lightY", &mLightAngles.y);
	mShadowPassDirty |= ImGui::SliderAngle("lightZ", &mLightAngles.z);

	static glm::vec2	LR = {-2650, 2650};
	static glm::vec2	BT = {-2100, 2100};
	static glm::vec2	NF = {-2800, 2600};

	mShadowPassDirty |= ImGui::DragFloat2("LR", &LR[0]);
	mShadowPassDirty |= ImGui::DragFloat2("BT", &BT[0]);
	mShadowPassDirty |= ImGui::DragFloat2("NF", &NF[0]);
	ImGui::End();

	if (!mShadowPassDirty)
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
	Renderer::Draw(scene, shadowTransform, Feature::ShadowPass);

	//drawMandelbox(shadowTransform);  //??????????????????????
	//drawMandelbrot(shadowTransform); //??????????????????????
	//drawCloud(shadowTransform);      //??????????????????????

	glActiveTexture(GL_TEXTURE15);
	glBindTexture(GL_TEXTURE_2D, mShadow.textures[RenderTarget::Depth]);

	static const glm::vec4 LIGHT_DIR{ 0,0,1,0 };

	mGlobalBuffer.lightDir = glm::normalize(glm::inverse(shadowView) * LIGHT_DIR);
	mGlobalBuffer.lightView = shadowTransform;
	mShadowPassDirty = false;
	markEnvMapAsDirty();
}

void ShaderPixel::markEnvMapAsDirty()
{
	mEnvMapDirty = true;
	mFaceIndex = 0;
}

void ShaderPixel::captureEnvMap()
{
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

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X + mFaceIndex, mEnvProbe.textures[RenderTarget::Color], 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	const static glm::vec3 probePosition{ 0, 400, 0 };
	glm::mat4 projection = MakeInfReversedZProjRH(glm::radians(90.0f), 1.f, 1.f); //glm::perspective(glm::radians(90.0f), 1.f, .1f, 9000.f);
	glm::mat4 view = glm::lookAt(probePosition, probePosition + targetVectors[mFaceIndex], upVectors[mFaceIndex]);
	glm::mat4 viewProjection = projection * view;
	Renderer::Draw(scene, viewProjection , Feature::Dithered);
	//drawMandelbrot(viewProjection);
	//drawMandelbox(viewProjection);
	drawCloud(viewProjection);

	glActiveTexture(GL_TEXTURE14);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mEnvProbe.textures[RenderTarget::Color]);

	mFaceIndex++;
	if (mFaceIndex >= 6)
	{
		mFaceIndex = 0;
		mEnvMapDirty = false;
	}
}

void ShaderPixel::onMouseMove(float x, float y)
{
	float dX = x - mMouseX;
	float dY = y - mMouseY;

	mMouseX = x;
	mMouseY = y;

	if (mCaptureMouse || glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		mCameraAngles.x += dX / mWindowSize.x;
		mCameraAngles.y += dY / mWindowSize.y;

		if (mCameraAngles.x > 2 * M_PI)
			mCameraAngles.x -= float(2 * M_PI);

		if (mCameraAngles.x <= -2 * M_PI)
			mCameraAngles.x += float(2 * M_PI);

		mCameraAngles.y = glm::clamp(mCameraAngles.y, -float(M_PI_2 * 0.8), float(M_PI_2 * 0.8));
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
	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE)
		mCaptureMouse = !mCaptureMouse;
	glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, mCaptureMouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

TextureData	LoadTextureData(const std::string& filename, int desiredComponentCount = 0)
{
	uint8_t		*LocalBuffer;
	glm::ivec2	Size;
	int			ComponentCount;

	std::string CompleteFilepath = Resources::BaseFilepath + filename;
	std::replace(CompleteFilepath.begin(), CompleteFilepath.end(), '\\', '/');
	LocalBuffer = stbi_load(CompleteFilepath.c_str(), &Size.x, &Size.y, &ComponentCount, desiredComponentCount);
	if (desiredComponentCount)
		ComponentCount = desiredComponentCount;
	if (!LocalBuffer)
	{
		BREAK();
	}
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

		if (It.dissolve < 1.0f)
		{
			mask |= Feature::Refraction;
			mask |= Feature::Reflection;

			Uniform ior;
			ior.type = UniformType::FLOAT;
			ior.scalar = It.ior;
			ior.name = "uIor";
			current->uniforms.push_back(ior);

			Uniform dissolve;
			ior.type = UniformType::FLOAT;
			ior.scalar = It.dissolve;
			ior.name = "uDissolve";
			current->uniforms.push_back(dissolve);
		}

		if (!It.diffuse_texname.empty())
		{
			mask |= Feature::DiffuseTexture;
			current->uniforms.push_back(PushTexture(It.diffuse_texname, TextureUsage::Diffuse));
		}
		else
		{
			Uniform diffuse;
			diffuse.type = UniformType::VEC4;
			diffuse.v4 = glm::vec4(It.diffuse[0], It.diffuse[1], It.diffuse[2], It.dissolve);
			diffuse.name = "uDiffuseColor";
			current->uniforms.push_back(diffuse);
		}

		if (!It.alpha_texname.empty())
		{
			current->uniforms.push_back(PushTexture(It.alpha_texname, TextureUsage::Alpha));
			current->blendMode = BlendMode::Masked;
			mask |= Feature::AlphaTexture;
			mask |= Feature::Masked;
		}
		else
			current->blendMode = BlendMode::Opaque;

		current->features = mask;
	}
}

void ShaderPixel::init()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

	ImGui::StyleColorsDark();
	ImFontConfig cfg;
	cfg.SizePixels = mScale;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplOpenGL3_Init("#version 410 core");

	Renderer::Init();
	assert(Resources::Textures.size() == 1);
	stbi_set_flip_vertically_on_load(false);
	TextureData CloudTexture = LoadTextureData("VolumeCloud.tga", 1);
	stbi_set_flip_vertically_on_load(true);
	CloudTexture.Slices = 12;
	CloudTexture.Type = GL_TEXTURE_3D;
	Resources::Textures.emplace_back(CloudTexture);

	assert(Resources::Textures.size() == 2);
	TextureData NoiseTexture = LoadTextureData("PerlinNoise.png", 1);
	CloudTexture.Type = GL_TEXTURE_2D;
	Resources::Textures.emplace_back(CloudTexture);

	mShadow = makeRenderTargetShadow(glm::ivec2(1024));

	const glm::ivec2 resolution(1280, 720);

	mSceneColorMS = makeRenderTargetMultisampled(resolution, GL_RGB, 8);
	mPingPong[0] = makeRenderTarget(resolution, GL_RGB, false);
	mPingPong[1] = makeRenderTarget(resolution, GL_RGB, false);
	mHalfRes[0] = makeRenderTarget(resolution/2, GL_RGB, false);
	mHalfRes[1] = makeRenderTarget(resolution/2, GL_RGB, false);
	mQuarterRes[0] = makeRenderTarget(resolution/4, GL_RGB, false);
	mQuarterRes[1] = makeRenderTarget(resolution/4, GL_RGB, false);
	mEightsRes[0] = makeRenderTarget(resolution/8, GL_RGB, false);
	mEightsRes[1] = makeRenderTarget(resolution/8, GL_RGB, false);
	mSixteenthsRes[0] = makeRenderTarget(resolution/16, GL_RGB, false);
	mSixteenthsRes[1] = makeRenderTarget(resolution/16, GL_RGB, false);
	mEnvProbe = makeRenderTargetCube(glm::ivec2(128), true);

	GLCall(glGenBuffers(1, &mGlobalBufferID));
	GLCall(glBindBuffer(GL_UNIFORM_BUFFER, mGlobalBufferID));
	GLCall(glBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalUniformBuffer),
		&mGlobalBuffer, GL_DYNAMIC_DRAW));
	GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, 0, mGlobalBufferID));


	// special shaders
	mMandelbrot = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"shaders/vertWorldSpace.shader",
	"shaders/fragMandelbrot.shader");

	mBox = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"shaders/vertWorldSpace.shader",
	"shaders/fragMandelbox.shader");

	mCloud = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"shaders/vertWorldSpace.shader",
	"shaders/fragCloud.shader");

	mFXAA = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"shaders/vertFXAA.shader",
	"shaders/fragFXAA.shader");

	mBrightnessFilter = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"shaders/vertFullscreen.shader",
	"shaders/fragBrightFilter.shader");

	mBlur = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"shaders/vertFullscreen.shader",
	"shaders/fragBlur.shader");

	mFullscreenTest = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"shaders/vertFullscreen.shader",
	"shaders/fragFullscreenTest.shader");

	mCubeMapTest = Resources::Shaders.size();
	Resources::Shaders.emplace_back(
	"shaders/vertWorldSpace.shader",
	"shaders/fragReadCubemap.shader");

	auto AddModelToScene = [](Scene& scene, std::string Filepath, glm::mat4 transform = glm::mat4(1.f)) {

		size_t MaterialIdOffset = Resources::Materials.size();
		tinyobj::ObjReader objReader;
		tinyobj::ObjReaderConfig config;
		config.vertex_color = false;
		config.triangulate = false;
		objReader.ParseFromFile(Resources::BaseFilepath + Filepath, config);
		assert(objReader.Valid());

		Filepath.resize(Filepath.rfind('/') + 1);
		// temporarily set base filepath to dir with file. needed for correct texture paths
		std::string OldPath = Resources::BaseFilepath + Filepath;
		std::swap(OldPath, Resources::BaseFilepath);

		auto& shapes = objReader.GetShapes();
		auto& attributes = objReader.GetAttrib();
		auto& materials = objReader.GetMaterials();

		std::thread MaterialLoader(&LoadMaterials, &materials);
		//LoadMaterials(materials);

		size_t ModelID = scene.models.size();
		scene.models.emplace_back();
		scene.models[ModelID].mModelSpace = transform;

		Resources::Meshes.reserve(shapes.size() + Resources::Meshes.size());
		for (size_t i = 0; i < shapes.size(); i++)
		{
			LoadMesh(shapes[i].mesh, attributes, scene.models[ModelID], MaterialIdOffset);
		}
		MaterialLoader.join();

		// undo the base path change
		std::swap(OldPath, Resources::BaseFilepath);
	};
	AddModelToScene(scene, "sponza/sponza.obj");
	AddModelToScene(scene, "sphere/sphere.obj", glm::translate(glm::mat4(1.f), glm::vec3{0, 400, 0}));

	Resources::FlushTextureData();
}

void ShaderPixel::drawMandelbox(glm::mat4 viewProjection)
{
	static glm::vec3 boxPos{ 487, 142, 144 };
	static float boxScale = 1.1f;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	Shader* manbox = Resources::GetShader(mBox);


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
		ImGui::Begin("Mandelbrot");
		ImGui::DragFloat2("MandelPos", &MandelPos[0], 0.001f);
		ImGui::DragFloat("MandelScale", &MandelScale, 0.0001f);
		ImGui::DragFloat("MandelIter", &MandelIter);
		ImGui::End();
	}
	mandel->Bind();
	mandel->SetUniform("uCenter", MandelPos);
	mandel->SetUniform("uScale", MandelScale);
	mandel->SetUniform("uIter", MandelIter);

	Renderer::DrawQuadWS(QuadPos, QuadScale, mandel, viewProjection);
}

void ShaderPixel::drawCloud(glm::mat4 viewProjection)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	glDepthMask(false);

	Shader* cloud = Resources::GetShader(mCloud);

	glm::vec3 boxPos{ 0,2350,0 };
	float boxScale = 50;

	Texture* CloudTexture = Resources::GetTexture(Texture::Cloud);
	CloudTexture->Bind();
	cloud->SetUniform("uVolume", (GLint)0);
	cloud->SetUniform("uCamPosMS", (mCameraPosition - boxPos) / boxScale);

	static float uDensity = 0.1f;
	static float uShadowDensity = 0.9f;
	ImGui::DragFloat("uDensity", &uDensity, .01f);
	ImGui::DragFloat("uShadowDensity", &uShadowDensity, .01f);
	cloud->SetUniform("uDensity", uDensity);
	cloud->SetUniform("uShadowDensity", uShadowDensity);

	Renderer::DrawCubeWS(boxPos, boxScale, cloud, viewProjection);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDepthMask(true);
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

void ShaderPixel::preframe()
{
	ImGui_ImplOpenGL3_NewFrame();
}

namespace { namespace Callbacks {
	void Error(int error, const char* description)
	{
		std::cerr << "GLFW Sent error " << error << ": " << description << "\n";
	}

	void MousePosition(GLFWwindow* window, double x, double y)
	{
		ShaderPixel* app = ShaderPixel::FromWindow(window);
		app->onMouseMove(float(x), float(y));
	}
	void Scroll(GLFWwindow* window, double x, double y)
	{
		ShaderPixel* app = ShaderPixel::FromWindow(window);
		app->onScroll(float(x), float(y));
	}
	void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		ShaderPixel* app = ShaderPixel::FromWindow(window);
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		app->onKey(key, scancode, action, mods);
	}
	void FramebufferSize(GLFWwindow* window, int width, int height)
{
		ShaderPixel* app = ShaderPixel::FromWindow(window);
		app->updateWindowSize(width, height);
	}
} }

ShaderPixel* ShaderPixel::FromWindow(GLFWwindow* window)
{
	return (ShaderPixel*)glfwGetWindowUserPointer(window);
}

ShaderPixel::ShaderPixel()
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

	mWindow = glfwCreateWindow(1024, 1024, "ShaderPixel", nullptr, nullptr);
	glfwSetWindowUserPointer(mWindow, this);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
	glfwSetKeyCallback(mWindow, Callbacks::Keyboard);
	glfwSetCursorPosCallback(mWindow, Callbacks::MousePosition);
	glfwSetScrollCallback(mWindow, Callbacks::Scroll);
	glfwSetFramebufferSizeCallback(mWindow, Callbacks::FramebufferSize);

	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(1);

	glfwGetWindowContentScale(mWindow, &mScale, &mScale);

	gladLoadGL();

	init();
	updateWindowSize(mWidth, mHeight);

	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
	glfwFocusWindow(mWindow);

	double dmousex, dmousey;
	glfwGetCursorPos(mWindow, &dmousex, &dmousey);
	mMouseX = float(dmousex);
	mMouseY = float(dmousey);
}

bool ShaderPixel::shouldClose()
{
	return glfwWindowShouldClose(mWindow);
}

void ShaderPixel::swapBuffers()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwMakeContextCurrent(mWindow);
	glfwSwapBuffers(mWindow);
}

