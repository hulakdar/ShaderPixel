#include "Application.h"
#include "UserInterface.h"
#include <stb_image.h>

static const struct
{
	float x, y;
	float r, g, b;
} vertices[] =
{
	{-1.f,-1.f, 1.f, 1.f, 0.f },
	{ 1.f,-1.f, 1.f, 1.f, 0.f },
	{-1.f, 1.f, 1.f, 0.f, 1.f },
	{ 1.f,-1.f, 1.f, 1.f, 0.f },
	{-1.f, 1.f, 1.f, 0.f, 1.f },
	{ 1.f, 1.f, 1.f, 1.f, 1.f }
};

static const char* vertex_shader_text = R"(
	#version 410 core

	layout (location = 0) in vec2 vPos;
	layout (location = 1) in vec3 vCol;

	out vec3 color;

	void main()
	{
		gl_Position = vec4(vPos, 0.0, 1.0);
		color = vCol;
	}
)";

static const char* fragment_shader_text = R"(
	#version 410 core

	in vec3 color;
	out vec4 FragColor;

	void main()
	{
		FragColor = vec4(color, 1.0);
	}
)";


int main(int argc, const char* argv[])
{
	Application App;

	GLuint vertex_buffer, vertex_array, vertex_shader, fragment_shader, program;

	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)(sizeof(float) * 2));

	while (!App.shouldClose())
	{
		App.update();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		ImGui::Button("KEK");
		ImGui::Text("This is some useful text.");

		App.swapBuffers();
	}
}

