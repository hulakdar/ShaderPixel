
layout (location = 0) in vec4 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vUV;

out VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec4 FragPosLightSpace;
    vec3 FragPosWorldSpace;
}   vs_out;

uniform mat4 uMVP;
uniform mat4 uModelToWorld;
uniform mat4 uLightSpaceMatrix;

layout(std140) uniform global
{
    mat4    lightView;
    vec4    lightDir;
    vec4    cameraPosition;
    float   time;
}           g;

void main()
{
	gl_Position = uMVP * vPos;

	vs_out.TexCoord = vUV;
	vs_out.Normal = vNorm;
	vs_out.FragPosWorldSpace = vec3(uModelToWorld * vPos);
	vs_out.FragPosLightSpace = g.lightView * vPos;
}
