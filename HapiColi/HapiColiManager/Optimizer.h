#pragma once

#include "TestResult.h"
#include "Analyzer.h"
#include "Fuzzer.h"
#include "HapiColiTypes.h"
#include <vector>
#include <string>

namespace HapiColi
{
    class Optimizer
    {
    public:
        // Analyze test results and generate suggestions
        std::vector<std::string> GenerateSuggestions(
            const std::vector<TestResult>& results, 
            const std::vector<AnalyzerWarning>& warnings = {},
            const std::vector<FuzzResult>& fuzzResults = {},
            Language lang = Language::English);
    };
}
