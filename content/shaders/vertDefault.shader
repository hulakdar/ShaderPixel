#version 410

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec2 vUV;

out vec2 UV;
out vec4 Pos;
uniform mat4 uMVP;

void main()
{
	Pos = fract(vPos);
	gl_Position = uMVP * vPos;
	UV = vUV;
}
