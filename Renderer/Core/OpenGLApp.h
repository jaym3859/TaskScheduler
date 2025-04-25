#pragma once
#include <chrono>
#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "GLShader.h" 
#include "GLTexture2D.h"
#include "App.hpp"
#include "GLDebug.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include "../RenderableObject.h"
#include "iRenderer.h"
#include <algorithm>
#include <vector>

class OpenGL_App {
public:
    OpenGL_App();
    OpenGL_App(const unsigned int width, const unsigned int height, 
        const unsigned int numBatches);
    virtual OpenGL_App operator=(const OpenGL_App& other) = delete; //non movable
    OpenGL_App(const OpenGL_App& other) = delete; //non movable
    virtual ~OpenGL_App(); 
    void Initialize(const unsigned int width, const unsigned int height,
        const unsigned int numBatches);
    void Execute();
private:
    static void fbSizeCallback(GLFWwindow* window, int width, int height);
    void Cleanup();
    void ProcessInput();
    void EnableImGui();
    void DisableImGui();
    void SetupImGui(GLFWwindow* window);
    void CleanupImGui();
    void RenderImGui();
    void SetupResources();
    void InitializeWindow(const unsigned int scr_width, const unsigned int scr_height);
    void InitializeGraphics(unsigned int numBatches);
    float getDpiScale(GLFWwindow* window);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void UpdateProjectionMatrix(int width, int height);
    void WindowSizeCallback(GLFWwindow* window, int width, int height);
    void UpdateDpiScale(GLFWwindow* window);
    // Mouse Move Callback (for mouseover detection)
    void HandleCursorPos(GLFWwindow* window, double xpos, double ypos);


    void InitCallbacks();

    // Comparator function for sorting GameObjects by dep
    GLFWwindow* window_;
    std::shared_ptr<GLShader> shader_;
    std::shared_ptr<GLTexture2D> texture_;
    std::unordered_map<std::string, unsigned int> textureIDs;
    std::vector<Rect> vRects;

    bool enableImGui = false;
    GLfloat clearColor[4] = { 0.45f, 0.55f, 0.60f, 1.00f };
    ImVec4 imguiClearColor = ImVec4(clearColor[0],clearColor[1], clearColor[2], clearColor[3]);
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 transform;
 
    unsigned int screen_width, screen_height;
    std::chrono::steady_clock::time_point lastTabPressTime;
    std::chrono::milliseconds debounceTime = std::chrono::milliseconds(300); // Adjust as needed
    float x = 250.0f, y = 100.0f, bx = 100, by = 100;
};


