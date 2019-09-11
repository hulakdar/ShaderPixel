#pragma  once

#include "Texture.h"
#include "Shader.h"

#include <memory>
#include <vector>

typedef unsigned int MaterialID;

enum class BlendMode : uint8_t
{
	Opaque,
	Masked,
	Translucent, // need?
};

struct Material
{
	BlendMode				blendMode = BlendMode::Opaque;
	ShaderID				shaderOverride = 0;
    std::vector<Uniform>    uniforms;
};