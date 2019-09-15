
layout (location = 0) in vec4 vPos;
layout (location = 1) in vec2 vUV;

uniform mat4 uMVP;
uniform vec3 uCamPosMS;

out vec3 FragPositionMS;
out vec3 FragToCamDirMS;
out vec2 UV;

void main()
{
	gl_Position = uMVP * vPos;
    FragToCamDirMS = uCamPosMS - vPos.xyz;
    FragPositionMS = vPos.xyz;

    UV = vUV;
}