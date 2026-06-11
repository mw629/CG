#include "LogManager.h"
#include "JsonSerializer.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace HapiColi
{
    bool LogManager::SaveFrames(const std::string& filepath, const std::vector<FrameData>& frames)
    {
        // Handle UTF-8 path properly on Windows in C++20
#if defined(_MSC_VER) && _HAS_CXX17
        std::filesystem::path path_u8 = std::filesystem::path(reinterpret_cast<const char8_t*>(filepath.c_str()));
        std::ofstream file(path_u8);
#else
        std::ofstream file(filepath);
#endif
        if (!file.is_open())
            return false;

        std::string jsonStr = JsonSerializer::SerializeFrames(frames);
        file << jsonStr;
        file.close();
        return true;
    }

    bool LogManager::LoadFrames(const std::string& filepath, std::vector<FrameData>& outFrames)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
            return false;

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string jsonStr = buffer.str();
        file.close();

        // Normally we would parse the JSON array here.
        // Since we are using a minimal placeholder, this won't populate outFrames yet.
        return true;
    }
}
