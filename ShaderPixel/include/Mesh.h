#pragma  once

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Material.h"

struct Mesh
{
    VertexBuffer		mVertexBuffer;
    IndexBuffer			mIndexBuffer;
    VertexArray			mVertexArray;
    Material            mMaterial;
	unsigned int		mCount = 0; // if zero then it's indexed so use IndexBuffer
};