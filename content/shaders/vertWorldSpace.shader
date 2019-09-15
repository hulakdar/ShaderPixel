
layout (location = 0) in vec4 vPos;
layout (location = 1) in vec2 vUV;

uniform mat4 uMVP;
uniform vec3 uCamPosMS;

out vec3 PositionMS;
out vec3 RayDirMS;
out vec2 UV;

void main()
{
	gl_Position = uMVP * vPos;
    RayDirMS = uCamPosMS - vPos.xyz;
    //PositionMS = vPos.xyz;
    PositionMS = uCamPosMS;

    UV = vUV;
}