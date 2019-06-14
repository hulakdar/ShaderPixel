#include "Application.h"
#include "UserInterface.h"
#include <stb_image.h>
#include "ShaderManager.h"

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


int main(int argc, const char* argv[])
{
	Application App;

	GLuint vertex_buffer, vertex_array;

	Shader defaultShader = ShaderManager::AddProgram("content/shaders/vertDefault.shader", "content/shaders/fragDefault.shader");

	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)(sizeof(float) * 2));

	while (!App.shouldClose())
	{
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		App.update();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		defaultShader.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);

		App.swapBuffers();
	}
}

