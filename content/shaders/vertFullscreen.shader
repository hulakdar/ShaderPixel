
layout (location = 0) in vec2 vPos;

out vec2 UV;

void main()
{
    UV = vPos * 0.5f + 0.5f;
	gl_Position = vec4(vPos, 0.5, 1);
}