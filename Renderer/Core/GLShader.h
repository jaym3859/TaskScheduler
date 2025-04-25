#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include "GLDebug.h"

// You don't need to include Shader.cpp in here!
struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
    std::string GeometrySource;
};

enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 , GEOMETRY = 2 };

class GLShader {
public:
    GLShader(const std::string& filepath);
    ~GLShader();

    void Bind() const;
    void Unbind() const;
    unsigned int CreateShader(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    ShaderProgramSource ParseShader(const std::string& filepath);
    int GetUniformLocation(const std::string& name) const;
    unsigned int getID();
    void SetUniform1i(const std::string& name, const int& value);
    void SetUniform2i(const std::string& name, const int& x, const int& y);
    void SetUniform3i(const std::string& name, const int& x, const int& y, const int& z);
    void SetUniform4i(const std::string& name, const int& x, const int& y, const int& z, const int& r);

    void SetUniform1iv(const std::string& name, int count, const int* values);

    void SetUniform1ui(const std::string& name, const unsigned int& value);
    void SetUniform2ui(const std::string& name, const unsigned int& x, const unsigned int& y);
    void SetUniform3ui(const std::string& name, const unsigned int& x, const unsigned int& y, const unsigned int& z);
    void SetUniform4ui(const std::string& name, const unsigned int& x, const unsigned int& y, const unsigned int& z, const unsigned int& r);

    void SetUniform1f(const std::string& name, const float& value);
    void SetUniform2f(const std::string& name, const float& x, const float& y);
    void SetUniform3f(const std::string& name, const float& x, const float& y, const float& z);
    void SetUniform4f(const std::string& name, const float& x, const float& y, const float& z, const float& r);

    void SetUniform1d(const std::string& name, const double& value);
    void SetUniform2d(const std::string& name, const double& x, const double& y);
    void SetUniform3d(const std::string& name, const double& x, const double& y, const double& z);
    void SetUniform4d(const std::string& name, const double& x, const double& y, const double& z, const double& r);

    void SetUniformMat2f(const std::string& name, const glm::mat2& matrix);
    void SetUniformMat3f(const std::string& name, const glm::mat3& matrix);
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

private:

    unsigned int m_RendererID;
    std::string m_FilePath;
    mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;
};