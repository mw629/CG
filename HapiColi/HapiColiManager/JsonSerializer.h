#pragma once

#include "FrameData.h"
#include "TestResult.h"
#include "Fuzzer.h"
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

        // Serialize combined report (Summary + Unhappy) to Markdown table format
        static std::string SerializeMarkdownReport(
            const std::vector<TestResult>& results,
            const std::vector<FrameData>& frames,
            const std::vector<FuzzResult>& fuzzResults,
            Language language = Language::English);

        // Deserialize a JSON string to FrameData (basic implementation for known format)
        static bool DeserializeFrameData(const std::string& jsonStr, FrameData& outData);

    private:
        static std::string EscapeString(const std::string& str);
    };
}
