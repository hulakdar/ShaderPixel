#version 410

in vec2 UV;
out vec4 fragColor;

uniform float uTime;
uniform sampler2D uTex;

void main()
{
	vec3 tex = texture(uTex, UV).xyz + abs(sin(uTime * 2));
	vec3 color = tex + abs(sin(uTime * 2));
	fragColor = vec4(color, 1);
}
