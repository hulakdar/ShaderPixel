#version 410

in vec2 UV;
out vec4 fragColor;

uniform float uTime;
uniform sampler2D uTex;

void main()
{
	vec3 color = texture(uTex, UV).xyz;
	fragColor = vec4(color, 1);
}
