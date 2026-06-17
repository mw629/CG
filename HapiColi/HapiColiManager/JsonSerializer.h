#pragma once

#include "FrameData.h"
#include "TestResult.h"
#include <string>
#include <vector>

namespace HapiColi
{
    class JsonSerializer
    {
    public:
        // Serialize a single FrameData to JSON string
        static std::string SerializeFrameData(const FrameData& data);

        // Serialize a TestResult to JSON string
        static std::string SerializeTestResult(const TestResult& result);

        // Deserialize a JSON string to FrameData (basic implementation for known format)
        static bool DeserializeFrameData(const std::string& jsonStr, FrameData& outData);
        
        // Serialize multiple frames
        static std::string SerializeFrames(const std::vector<FrameData>& frames);

        // Serialize collision summary (start/end frames)
        static std::string SerializeCollisionSummary(const std::vector<FrameData>& frames);

        // Serialize unhappy results + their corresponding frames
        static std::string SerializeUnhappyReport(
            const std::vector<TestResult>& results,
            const std::vector<FrameData>& frames);

    private:
        static std::string EscapeString(const std::string& str);
    };
}
