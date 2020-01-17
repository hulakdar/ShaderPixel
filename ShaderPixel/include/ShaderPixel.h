#include "Renderer.h"
#include "RenderTarget.h"

#include <GLFW/glfw3.h>

class ShaderPixel
{
public:
	ShaderPixel();
	void init();
	void deinit();
	bool shouldClose();

	void update();
	void updateWindowSize(int x, int y);
	void preframe();
	void onKey(int key, int scancode, int action, int mods);
	void onMouseMove(float x, float y);
	void onScroll(float x, float y);
	void swapBuffers();
	~ShaderPixel();

	static ShaderPixel *FromWindow(GLFWwindow* window);
private:

	bool mEnvMapDirty = true;
	bool mShadowPassDirty = true;

	void shadowPass();
	void captureEnvMap();
	void markEnvMapAsDirty();
	void drawMandelbrot(glm::mat4 viewProjection);
	void drawCloud(glm::mat4 viewProjection);
	void drawMandelbox(glm::mat4 viewProjection);


	ShaderID	mBox;
	ShaderID	mFXAA;
	ShaderID	mCloud;
	ShaderID	mFullscreenTest;
	ShaderID	mCubeMapTest;
	ShaderID	mBrightnessFilter;
	ShaderID	mBlur;
	ShaderID	mMandelbrot;

	Scene			scene;

	RenderTarget mSceneColorMS;
	RenderTarget mShadow;
	RenderTarget mPingPong[2];
	RenderTarget mHalfRes[2];
	RenderTarget mQuarterRes[2];
	RenderTarget mEightsRes[2];
	RenderTarget mSixteenthsRes[2];

	RenderTarget mEnvProbe;
	int	mFaceIndex = 0;

	struct GlobalUniformBuffer {
		glm::mat4	lightView;
		glm::vec4	lightDir;
		glm::vec4	cameraPosition;
		float		time;
	} mGlobalBuffer;

	GLuint		mGlobalBufferID;

	bool mCaptureMouse = true;

	float		mAspectRatio = 1.f;
	float		mSpeed = 1.f;

	double		mCurrentTime = 0.0;

	glm::vec3	mLightAngles	{ 1., 1.5, 0. };
	glm::vec3	mCameraPosition {0,10,50};
	glm::vec3	mCameraForward	{0,0,1};
	glm::vec3	mCameraRight	{1,0,0};
	glm::vec3	mCameraUp		{0,1,0};
	glm::vec2	mCameraAngles	{ 0,0 };

	glm::vec2	mWindowSize		{ 0,0 };

	GLFWwindow	*mWindow;
	float		mScale;
	int			mWidth, mHeight;
	float		mMouseX, mMouseY;
};
