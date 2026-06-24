#include "LogManager.h"
#include "JsonSerializer.h"
#include <fstream>
#include <sstream>
#include <filesystem>

namespace HapiColi
{
    bool LogManager::SaveCombinedReport(
        const std::string& filepath,
        const std::vector<TestResult>& results,
        const std::vector<FrameData>& frames,
        const std::vector<FuzzResult>& fuzzResults,
        Language language)
    {
        std::ofstream file(filepath);
        if (!file.is_open())
            return false;

        std::string reportStr = JsonSerializer::SerializeMarkdownReport(results, frames, fuzzResults, language);
        file << reportStr;
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
