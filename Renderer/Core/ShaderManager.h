#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "GLShader.h"
#include <iostream>

class ShaderManager {
public:
    static ShaderManager& Instance() {
        static ShaderManager instance;
        return instance;
    }

    std::shared_ptr<GLShader> LoadShader(const std::string& shaderName, const std::string& filePath) {
        // Check if the shader is already loaded
        if (shaders.find(shaderName) != shaders.end()) {
            return shaders[shaderName];
        }

        // Create and load the shader
        std::shared_ptr<GLShader> newShader = std::make_shared<GLShader>(filePath);
        shaders[shaderName] = newShader;
        return newShader;
    }

    std::shared_ptr<GLShader> GetShader(const std::string& shaderName) {
        auto it = shaders.find(shaderName);
        if (it != shaders.end()) {
            return it->second;
        }
        std::cerr << "Shader not found: " << shaderName << std::endl;
        return nullptr;
    }

private:
    ShaderManager() {}
    std::unordered_map<std::string, std::shared_ptr<GLShader>> shaders;
};