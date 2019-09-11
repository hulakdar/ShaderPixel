#version 410

layout(location = 0) in  vec2 vPos;

out vec4 FragPos;

uniform vec2 rcpFrame;

void main(void)
{
    const float FXAA_SUBPIX_SHIFT = 1.0/4.0;

    gl_Position = vec4(vPos, 0, 0);
    FragPos.xy = vPos;
    FragPos.zw = vPos - (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));
}