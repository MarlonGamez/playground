#pragma once

#include <GL/glew.h>

#include <string>
#include <unordered_map>

#include "util/GL.h"

enum ShaderType { vertex, fragment };

class Shader
{
private:
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& vertFilepath, const std::string& fragFilepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
private:
	int GetUniformLocation(const std::string& name);

	GLuint LoadShaders(const std::string& vertexFilePath, const std::string& fragmentFilePath);
	GLuint LoadSingleShader(const std::string& shaderFilePath, ShaderType type);
};