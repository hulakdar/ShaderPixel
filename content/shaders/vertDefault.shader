
layout (location = 0) in vec4 vPos;
layout (location = 1) in vec3 vNorm;
layout (location = 2) in vec2 vUV;

out VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    float VertexOffset;
}   vs_out;

uniform mat4 uMVP;

void main()
{
	vs_out.VertexOffset = float(gl_VertexID);
	vs_out.TexCoord = vUV;
	vs_out.Normal = vNorm;
	gl_Position = uMVP * vPos;
}
