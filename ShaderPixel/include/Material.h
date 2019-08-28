#pragma  once

#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <vector>

class Material
{
private:
    Shader                  mShader;
    //std::vector<std::shared_ptr<Texture>> mTextures;
    std::vector<Uniform>    mUniforms;
public:
	Material();
	void Apply();
};