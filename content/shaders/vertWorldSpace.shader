
layout (location = 0) in vec4 vPos;
layout (location = 1) in vec2 vUV;
layout (location = 2) in vec3 vNormal;

uniform mat4 uMVP;
uniform vec3 uCamPosMS;

out vec3 FragPositionMS;
out vec3 FragToCamDirMS;
out vec2 UV;
out vec3 Normal;

void main()
{
	gl_Position = uMVP * vPos;
    FragToCamDirMS = uCamPosMS - vPos.xyz;
    FragPositionMS = vPos.xyz;

    UV = vUV;
    Normal = vNormal;
}