#version 410

layout (location = 0) in vec4 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vUV;

out VS_OUT
{
    vec4 FragPos;
    vec2 TexCoord;
    vec3 Normal;
}   vs_out;

uniform mat4 uMVP;

void main()
{
	vs_out.FragPos = uMVP * vPos;
	vs_out.TexCoord = vUV;
	vs_out.Normal = vNorm;
	gl_Position = vs_out.FragPos;
}
