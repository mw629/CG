#pragma once

#include "FrameData.h"
#include "TestResult.h"
#include <string>
#include <vector>

namespace HapiColi
{
    class LogManager
    {
    public:
        // Save frames to a JSON file
        bool SaveFrames(const std::string& filepath, const std::vector<FrameData>& frames);

        // Save only unhappy results + their frame data to a JSON file
        bool SaveUnhappyReport(
            const std::string& filepath,
            const std::vector<TestResult>& results,
            const std::vector<FrameData>& frames);

        // Save collision summary (start and end frames of collisions)
        bool SaveCollisionSummary(const std::string& filepath, const std::vector<FrameData>& frames);

        // Load frames from a JSON file
        bool LoadFrames(const std::string& filepath, std::vector<FrameData>& outFrames);
    };
}
