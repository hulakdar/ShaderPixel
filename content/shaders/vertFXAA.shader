
layout(location = 0) in  vec2 vPos;

out vec4 FragPos;

uniform vec2 rcpFrame;

void main(void)
{
    const float FXAA_SUBPIX_SHIFT = 1.0/4.0;

    gl_Position = vec4(vPos, .5, 1.);

    vec2 UV = vPos * 0.5 + 0.5;
    FragPos.xy = UV;
    FragPos.zw = UV - (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));
}