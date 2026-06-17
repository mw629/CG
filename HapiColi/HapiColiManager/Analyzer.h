#pragma once

#include "FrameData.h"
#include "TestResult.h"
#include "TestRule.h"
#include <vector>
#include <memory>

namespace HapiColi
{
    struct AnalyzerWarning {
        uint32_t frame;
        std::string message;
        std::string severity;
    };

    class Analyzer
    {
    public:
        void AddRule(std::shared_ptr<TestRule> rule);
        void ClearRules();

        void Analyze(const std::vector<FrameData>& frames);
        void AnalyzeAppend(const std::vector<FrameData>& frames); // 既存結果に追記
        void ClearResults();

        const std::vector<TestResult>& GetResults() const;
        const TestSummary& GetSummary() const;
        const std::vector<std::shared_ptr<TestRule>>& GetRules() const;

        void AnalyzeWarnings(const std::vector<FrameData>& frames);
        const std::vector<AnalyzerWarning>& GetWarnings() const { return m_warnings; }

    private:
        std::vector<std::shared_ptr<TestRule>> m_rules;
        std::vector<TestResult> m_results;
        TestSummary m_summary;
        std::vector<AnalyzerWarning> m_warnings;
    };
}
