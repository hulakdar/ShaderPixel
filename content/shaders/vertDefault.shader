#version 410

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec2 vUV;

out vec2 UV;
uniform mat4 uMVP;

void main()
{
	gl_Position = uMVP * vPos;
	UV = vUV;
}
