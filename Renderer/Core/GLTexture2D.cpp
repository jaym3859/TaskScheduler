#include "GLTexture2D.h"
#include "GLDebug.h"
#include <stb_image.h>

GLTexture2D::GLTexture2D() {
    // Create a 1x1 white texture
    uint32_t whitePixel = 0xffffffff;

    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, &whitePixel));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    size = { 1, 1, 4 };
}

GLTexture2D::GLTexture2D(const std::string& path)
    : m_FilePath(path) {
    stbi_set_flip_vertically_on_load(0);

    int width, height, bpp;
    m_LocalBuffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);
    size = { width, height, bpp };

    if (m_LocalBuffer) {
        GLCall(glGenTextures(1, &m_RendererID));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));

        stbi_image_free(m_LocalBuffer);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
}
void GLTexture2D::load(const std::string& path) {
    // If the texture is still the white placeholder texture, just reload
    if (m_RendererID != 0) {
        // If the texture is not the placeholder white texture, delete the previous one
        GLCall(glDeleteTextures(1, &m_RendererID));
    }

    stbi_set_flip_vertically_on_load(1);

    int width, height, bpp;
    m_LocalBuffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);
    size = { width, height, bpp };

    if (m_LocalBuffer) {
        GLCall(glGenTextures(1, &m_RendererID));
        GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));

        stbi_image_free(m_LocalBuffer);
        m_FilePath = path;  // Update the file path
    }
}
GLTexture2D::~GLTexture2D() {
    GLCall(glDeleteTextures(1, &m_RendererID));
}

GLTexture2D::GLTexture2D(const GLTexture2D& other) {
    m_RendererID = other.m_RendererID;
    m_FilePath = other.m_FilePath;
    size = other.size;
}

void GLTexture2D::operator=(const GLTexture2D& other) {
    if (this != &other) {
        m_RendererID = other.m_RendererID;
        m_FilePath = other.m_FilePath;
        size = other.size;
    }
}

bool GLTexture2D::operator==(const GLTexture2D& other) {
    return m_RendererID == other.m_RendererID;
}

bool GLTexture2D::operator!=(const GLTexture2D& other) {
    return !(*this == other);
}

void GLTexture2D::bind(unsigned int slot) const {
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void GLTexture2D::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

