#pragma once

#include <glm/glm.hpp>

struct Pass
{
	glm::mat4					viewProjection = glm::mat4(1);
	ShaderID					shader = 0;
	std::vector<RenderTarget>	outputs;
};
