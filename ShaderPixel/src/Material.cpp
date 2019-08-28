#include "Material.h"

Material::Material() { }

void Material::Apply()
{
	for (auto& It : mUniforms)
	{
		mShader.SetUniform(It);
	}
}
