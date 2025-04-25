#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/common.hpp> //vec3, vec4, ivec4, mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glcorearb.h>
#include <unordered_map>
#include "GLDebug.h"
#include "GLShader.h"
#include "GLTexture2D.h"
#include "../RenderableObject.h"
class Renderer
{
public:

    Renderer(); // Constructor
    Renderer(uint32_t numBatches, uint32_t maximumTextures);
    ~Renderer(); // Destructor

    void Init(uint32_t numBatches, uint32_t maximumTextures = 16);
    void Shutdown();
    void BeginBatch(uint32_t batchID);
    void EndBatch(uint32_t batchID);
    void Flush(uint32_t batchID);

    void SetShader(uint32_t batchID, GLShader* batchShader);
    void DrawQuad(uint32_t batchID, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    void DrawQuad(uint32_t batchID, const glm::vec2& position, const glm::vec2& size, uint32_t textureId);
    void DrawQuad(uint32_t batchID, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, uint32_t textureId);
    void Draw(uint32_t batchID, std::shared_ptr<RenderableObject> obj);

    struct Stats
    {
        uint32_t DrawCount = 0;
        uint32_t QuadCount = 0;
    };

    const Stats& GetStats(uint32_t batchID);
    void ResetStats(uint32_t batchID);

private:
    static std::vector<uint32_t> TextureSlots;
    static std::unordered_map<uint32_t, uint32_t> TextureSlotLookup;
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec2 TexCoords;
        glm::vec4 Color;
        float TexIndex;
    };

    struct RendererData
    {
        GLuint QuadVA = 0;
        GLuint QuadVB = 0;
        GLuint QuadIB = 0;

        GLuint WhiteTexture = 0;
        uint32_t WhiteTextureSlot = 0;

        uint32_t IndexCount = 0;

        Vertex* QuadBuffer = nullptr;
        Vertex* QuadBufferPtr = nullptr;
        GLShader* shader = nullptr;

        uint32_t TextureSlotIndex = 1;
        Stats RenderStats = { 0,0 };

    };

    RendererData* m_RendererData;
    static const size_t MaxQuadCount = 1000;
    static const size_t MaxVertexCount = MaxQuadCount * 4;
    static const size_t MaxIndexCount = MaxQuadCount * 6;
    size_t maxTextures;
    bool initialized = false;
    int numBatches = 0;
};
