#include "Texture.h"
#include "Shader.h"
#include <memory>

class Material
{
private:
    Shader                  m_Shader;
    std::vector<std::shared_ptr<Texture>> m_Textures;
    //std::vector<Uniform>    m_Uniforms;
public:
	Material();
};