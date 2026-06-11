#pragma once

#include "TestResult.h"
#include <vector>
#include <string>

namespace HapiColi
{
    class Optimizer
    {
    public:
        // Analyze test results and generate suggestions
        std::vector<std::string> GenerateSuggestions(const std::vector<TestResult>& results);
    };
}
