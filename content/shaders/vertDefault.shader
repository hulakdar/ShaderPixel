#version 410

layout (location = 0) in vec4 vPos;

//out vec3 color;
uniform mat4 uViewProjection;
uniform mat4 uModel;

void main()
{
	gl_Position = uViewProjection * uModel * vPos;
	//color = vec3(gl_VertexID / 3);
}
