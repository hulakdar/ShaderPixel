#version 410

in VS_OUT
{
    vec4 FragPos;
    vec2 TexCoord;
    vec3 Normal;
}   vs_out;

out vec4 fragColor;

uniform float uTime;
uniform float uPow = 1;
uniform sampler2D uDiffuse;

void main()
{
	vec3 albedo = texture(uDiffuse, vs_out.TexCoord).xyz;

	fragColor = vec4(vec3(dot(vec3(1), vs_out.Normal)), 1);
}
