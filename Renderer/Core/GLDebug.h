#pragma once
#include <iostream>
#include <glad/glad.h> // or glad, etc

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(::GLLogCall(#x, __FILE__, __LINE__))

inline void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

inline bool GLLogCall(const char* function, const char* file, int line) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cout << "[OpenGL Error] (" << error << ") "
            << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}
