#pragma once
#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

class GLTexture2D {
public:
    GLTexture2D();  // Default 1x1 white or empty
    GLTexture2D(const std::string& path);  // Load from file
    ~GLTexture2D();

    GLTexture2D(const GLTexture2D& other);  // Copy constructor
    void operator=(const GLTexture2D& other);  // Assignment operator

    bool operator==(const GLTexture2D& other);
    bool operator!=(const GLTexture2D& other);

    void load(const std::string& path);
    void bind(unsigned int slot = 0) const;
    void unbind() const;

    inline int getWidth() const { return size.x; }
    inline int getHeight() const { return size.y; }
    inline int getBPP() const { return size.z; }
    inline glm::vec3 getSizeV3() const { return size; }
    inline glm::vec2 getSizeV2() const { return glm::vec2(size.x, size.y); }
    inline unsigned int getID() const { return m_RendererID; }
private:
    unsigned int m_RendererID = 0;
    std::string m_FilePath;
    unsigned char* m_LocalBuffer = nullptr;
    glm::vec3 size = { 0, 0, 0 };
};