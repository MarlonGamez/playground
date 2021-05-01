#pragma once

#include <GL/glew.h>

#include <iostream>

#include "util/GL.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer
{
public:
    void Clear() const;
    void Draw(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const;
};