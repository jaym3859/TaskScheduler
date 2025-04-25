#include "ConfigLoader.h"

ConfigLoader::ConfigLoader(const std::string& file_path) {
    std::ifstream file(file_path);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            configMap[key] = value;
        }
    }
}

std::string ConfigLoader::Get(const std::string& key) {
    return configMap[key];
}