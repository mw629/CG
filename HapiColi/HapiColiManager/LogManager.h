#pragma once

#include "FrameData.h"
#include <string>
#include <vector>

namespace HapiColi
{
    class LogManager
    {
    public:
        // Save frames to a JSON file
        bool SaveFrames(const std::string& filepath, const std::vector<FrameData>& frames);

        // Load frames from a JSON file
        bool LoadFrames(const std::string& filepath, std::vector<FrameData>& outFrames);
    };
}
