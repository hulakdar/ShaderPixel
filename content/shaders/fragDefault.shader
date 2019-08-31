#version 410

in vec2 UV;
in vec4 Pos;
out vec4 fragColor;

uniform float uTime;
uniform float uPow = 1;
uniform sampler2D uTex;


void main()
{
	vec3 tex = texture(uTex, UV).xyz + abs(sin(uTime * 2));
	fragColor = Pos * uPow;
}
