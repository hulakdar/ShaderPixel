#pragma  once

#include "Texture.h"
#include "Shader.h"
#include <memory>
#include <vector>

struct Material
{
    std::vector<Uniform>    mUniforms;
};