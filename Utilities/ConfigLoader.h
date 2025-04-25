#include <fstream>
#include <unordered_map>
#include <sstream>
#include <string>

class ConfigLoader {
public:
    ConfigLoader(const std::string& file_path);

    std::string Get(const std::string& key);

private:
    std::unordered_map<std::string, std::string> configMap;
};