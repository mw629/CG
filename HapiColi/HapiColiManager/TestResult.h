#pragma once

#include <string>

namespace HapiColi
{
    struct TestResult
    {
        bool happy = false;
        uint32_t frame = 0;
        std::string expected;
        std::string actual;
        std::string reason;
        std::string targetId;
    };

    struct TestSummary
    {
        uint32_t totalTests = 0;
        uint32_t happyCount = 0;
        uint32_t unhappyCount = 0;
        float successRate = 0.0f;
        
        void Calculate()
        {
            if (totalTests > 0)
            {
                successRate = static_cast<float>(happyCount) / totalTests;
            }
            else
            {
                successRate = 0.0f;
            }
        }
    };
}
