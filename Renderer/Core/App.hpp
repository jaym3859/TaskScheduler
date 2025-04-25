#pragma once

class App {
public:
    virtual ~App() {}

    // Common initialization
    void Initialize(const unsigned int& scr_width = 800, const unsigned int& scr_height = 600) {
        InitializeWindow(scr_width, scr_height);
        InitializeGraphics();  // Initialize graphics API-specific setup
        SetupResources();
    }

    virtual void Execute() = 0;  // The main loop, must be implemented in derived class

    virtual void ProcessInput() = 0;  // Handle user input, platform-specific

    virtual void Cleanup() = 0;  // Cleanup resources

protected:
    virtual void InitializeWindow(const unsigned int& scr_width, const unsigned int& scr_height) = 0;
    virtual void InitializeGraphics() = 0;  // Initialize graphics API (OpenGL/DirectX)
    virtual void SetupResources() = 0;  // Setup resources like shaders, buffers, etc.

    // General utilities for window interaction
    void resize(const unsigned int& new_width, const unsigned int& new_height) {
        // Platform-independent resizing logic
    }

    // Window object (this could be a platform-specific type)
    void* window_;  // Could be a GLFW window for OpenGL, a Win32 window handle for Win32 apps, etc.
};