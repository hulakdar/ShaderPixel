
in vec3 color;
out vec4 fragColor;

uniform float time;

void main()
{
	fragColor = vec4(vec3(fract(time)), 1);
}