
#include "GLShader.h"


// Modify the constructor to accept geometry shader source
GLShader::GLShader(const std::string& filepath)
    : m_FilePath(filepath), m_RendererID(0)
{
    // Parse the vertex and fragment shader sources
    ShaderProgramSource source = ParseShader(filepath);

    std::cout << "VERTEX" << std::endl << source.VertexSource << std::endl;
    std::cout << "FRAGMENT" << std::endl << source.FragmentSource << std::endl;


    // If there is a geometry shader, print its source too for debugging
    if (!source.GeometrySource.empty()) {
        std::cout << "GEOMETRY" << std::endl << source.GeometrySource << std::endl;
    }

    // Create the shader program with the vertex, fragment, and geometry shaders
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource, source.GeometrySource);

    GLCall(glUseProgram(m_RendererID));
}

GLShader::~GLShader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void GLShader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void GLShader::Unbind() const
{
    GLCall(glUseProgram(0));
}

int GLShader::GetUniformLocation(const std::string& name) const
{
    auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end())
        return it->second;

    GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        std::cerr << "Warning: Uniform '" << name << "' not found in shader!" << std::endl;

    m_UniformLocationCache[name] = location; // Still needs to be mutable or method not const
    return location;
}
unsigned int GLShader::getID() {
    return m_RendererID;
}

struct ShaderProgramSource GLShader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[3];  // For vertex, fragment, and geometry shaders
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
            else if (line.find("geometry") != std::string::npos)
                type = ShaderType::GEOMETRY;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str(), ss[2].str() };  // Return vertex, fragment, and geometry source
}

unsigned int GLShader::CompileShader(unsigned int type, const std::string& source)
{
    GLCall(unsigned int id = glCreateShader(type));
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    // Error handling
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        std::vector<char> log(length);
        GLCall(glGetShaderInfoLog(id, length, &length, log.data()));
        std::cerr << "ERROR: Shader compile failed with log: " << log.data() << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}
GLuint GLShader::CreateShader(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexSourceCStr = vertexSource.c_str();
    glShaderSource(vertexShader, 1, &vertexSourceCStr, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetShaderInfoLog(vertexShader, logLength, &logLength, log.data());
        std::cerr << "ERROR: Vertex shader compilation failed:\n" << log.data() << std::endl;
        glDeleteShader(vertexShader);
        return 0;  // Return an invalid ID
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentSourceCStr = fragmentSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentSourceCStr, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetShaderInfoLog(fragmentShader, logLength, &logLength, log.data());
        std::cerr << "ERROR: Fragment shader compilation failed:\n" << log.data() << std::endl;
        glDeleteShader(fragmentShader);
        return 0;
    }

    GLuint geometryShader = 0;
    if (!geometrySource.empty()) {
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        const char* geometrySourceCStr = geometrySource.c_str();
        glShaderSource(geometryShader, 1, &geometrySourceCStr, nullptr);
        glCompileShader(geometryShader);

        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            GLint logLength;
            glGetShaderiv(geometryShader, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> log(logLength);
            glGetShaderInfoLog(geometryShader, logLength, &logLength, log.data());
            std::cerr << "ERROR: Geometry shader compilation failed:\n" << log.data() << std::endl;
            glDeleteShader(geometryShader);
            return 0;
        }
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);  // Attach vertex shader first
    if (geometryShader != 0) {
        glAttachShader(program, geometryShader);  // Attach geometry shader second (if provided)
    }
    glAttachShader(program, fragmentShader);  // Attach fragment shader last

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetProgramInfoLog(program, logLength, &logLength, log.data());
        std::cerr << "ERROR: Program linking failed:\n" << log.data() << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (geometryShader != 0) {
        glDeleteShader(geometryShader);
    }

    return program;
}
#pragma region uniform_setters
//set uniform functions 
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
void GLShader::SetUniform4f(const std::string& name, const float& x, const float& y, const float& z, const float& r) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform4f(location, x, y, z, r));
}
void GLShader::SetUniform3f(const std::string& name, const float& x, const float& y, const float& z) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform3f(location, x, y, z));
}
void GLShader::SetUniform2f(const std::string& name, const float& x, const float& y) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform2f(location, x, y));
}
void GLShader::SetUniform1f(const std::string& name, const float& value) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform1f(location, value));
}
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
void GLShader::SetUniform4i(const std::string& name, const int& x, const int& y, const int& z, const int& r) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform4i(location, x, y, z, r));
}
void GLShader::SetUniform3i(const std::string& name, const int& x, const int& y, const int& z) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform3i(location, x, y, z));
}
void GLShader::SetUniform2i(const std::string& name, const int& x, const int& y) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform2i(location, x, y));
}
void GLShader::SetUniform1i(const std::string& name, const int& value) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform1i(location, value));
}

void GLShader::SetUniform1iv(const std::string& name, int count, const int* values) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform1iv(location, count, values));
}
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
void GLShader::SetUniform4ui(const std::string& name, const unsigned int& x, const unsigned int& y, const unsigned int& z, const unsigned int& r) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform4ui(location, x, y, z, r));
}
void GLShader::SetUniform3ui(const std::string& name, const unsigned int& x, const unsigned int& y, const unsigned int& z) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform3ui(location, x, y, z));
}
void GLShader::SetUniform2ui(const std::string& name, const unsigned int& x, const unsigned int& y) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform2ui(location, x, y));
}
void GLShader::SetUniform1ui(const std::string& name, const unsigned int& value) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform1ui(location, value));
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/

void GLShader::SetUniform4d(const std::string& name, const double& x, const double& y, const double& z, const double& r) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform4d(location, x, y, z, r));
}
void GLShader::SetUniform3d(const std::string& name, const double& x, const double& y, const double& z) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform3d(location, x, y, z));
}
void GLShader::SetUniform2d(const std::string& name, const double& x, const double& y) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform2d(location, x, y));
}
void GLShader::SetUniform1d(const std::string& name, const double& value) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniform1d(location, value));
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
void GLShader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]));
}
void GLShader::SetUniformMat3f(const std::string& name, const glm::mat3& matrix) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniformMatrix3fv(location, 1, GL_FALSE, &matrix[0][0]));
}
void GLShader::SetUniformMat2f(const std::string& name, const glm::mat2& matrix) {
    GLint location = GetUniformLocation(name);
    GLCall(glUniformMatrix2fv(location, 1, GL_FALSE, &matrix[0][0]));
}
/*--------------------------------------------------------------------------------------------------------------------------------------------------*/
#pragma endregion
