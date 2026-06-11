#include "LogManager.h"
#include "JsonSerializer.h"
#include <fstream>
#include <sstream>

namespace HapiColi
{
    bool LogManager::SaveFrames(const std::string& filepath, const std::vector<FrameData>& frames)
    {
        std::ofstream file(filepath);
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
