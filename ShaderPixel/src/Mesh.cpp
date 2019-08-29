#include "Mesh.h"

void FinalizeMesh(Mesh& mesh, const VertexBufferLayout& vbl)
{
	mesh.mVertexArray.AddBuffer(mesh.mVertexBuffer, vbl);
}
