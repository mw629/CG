#pragma once

#include "FrameData.h"
#include "TestResult.h"
#include "Fuzzer.h"
#include <string>
#include <vector>

namespace HapiColi
{
    class LogManager
    {
    public:
        bool SaveCombinedReport(
            const std::string& filepath,
            const std::vector<TestResult>& results,
            const std::vector<FrameData>& frames,
            const std::vector<FuzzResult>& fuzzResults,
            Language language = Language::English);

        // Load frames from a JSON file
        bool LoadFrames(const std::string& filepath, std::vector<FrameData>& outFrames);
    };
}
