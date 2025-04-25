#include "Renderer/Core/OpenGLApp.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

int main() {
    OpenGL_App app;

    try {
        // Initialize OpenGL renderer with screen size (800x600)
       // app.initialize(SCR_WIDTH, SCR_HEIGHT);
        // Enter the main rendering loop
        app.Initialize(SCR_WIDTH, SCR_HEIGHT, 2);
        app.Execute();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
