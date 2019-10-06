#include "Application.h"
#include "Renderer.h"
#include "RenderTarget.h"

struct AppMemory
{
	//ShaderManager	shaderManager;
	Scene			scene;
};

class ShaderPixel : public Application
{
public:
	virtual void init(Host*) override;
	virtual void deinit() override;

	virtual void update() override;
	virtual void updateWindowSize(int x, int y) override;
	virtual void renderUI() override;
	virtual void preframe() override;
	virtual void onKey(int key, int scancode, int action, int mods) override;
	virtual void onMouseMove(float x, float y, float dX, float dY) override;
	virtual void onScroll(float x, float y) override;
	virtual ~ShaderPixel();
private:

	bool mEnvMapDirty = true;

	void shadowPass();
	void captureEnvMap();
	void markEnvMapAsDirty();
	void drawMandelbrot(glm::mat4 viewProjection);
	void drawMandelbox(glm::vec3 boxPos, float boxScale , glm::mat4 viewProjection);

	ShaderID	mBox;
	ShaderID	mFXAA;
	ShaderID	mCloud;
	ShaderID	mFullscreenTest;
	ShaderID	mCubeMapTest;
	ShaderID	mBrightnessFilter;
	ShaderID	mBlur;
	ShaderID	mBloom;
	ShaderID	mMandelbrot;


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
		float		fogParamA;
		float		fogParamB;
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
};
