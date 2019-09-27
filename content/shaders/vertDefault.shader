
layout (location = 0) in vec4 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vUV;

out VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec4 FragPosLightSpace;
}   vs_out;

uniform mat4 uMVP;
uniform mat4 uLightSpaceMatrix;

void main()
{
	gl_Position = uMVP * vPos;

	vs_out.TexCoord = vUV;
	vs_out.Normal = vNorm;
    vs_out.FragPosLightSpace = uLightSpaceMatrix * vec4(vPos.xyz, 1.0);
}
