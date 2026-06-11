#include "Analyzer.h"

namespace HapiColi
{
    void Analyzer::AddRule(std::shared_ptr<TestRule> rule)
    {
        m_rules.push_back(rule);
    }

    void Analyzer::ClearRules()
    {
        m_rules.clear();
    }

    void Analyzer::Analyze(const std::vector<FrameData>& frames)
    {
        m_results.clear();
        m_summary = TestSummary();

        for (const auto& frame : frames)
        {
            for (const auto& rule : m_rules)
            {
                TestResult result;
                if (rule->Evaluate(frame, result))
                {
                    m_results.push_back(result);

                    m_summary.totalTests++;
                    if (result.happy)
                        m_summary.happyCount++;
                    else
                        m_summary.unhappyCount++;
                }
            }
        }

        m_summary.Calculate();
    }

    void Analyzer::AnalyzeAppend(const std::vector<FrameData>& frames)
    {
        // 既存の results/summary はクリアせず、追記する
        for (const auto& frame : frames)
        {
            for (const auto& rule : m_rules)
            {
                TestResult result;
                if (rule->Evaluate(frame, result))
                {
                    m_results.push_back(result);

                    m_summary.totalTests++;
                    if (result.happy)
                        m_summary.happyCount++;
                    else
                        m_summary.unhappyCount++;
                }
            }
        }

        m_summary.Calculate();
    }

    void Analyzer::ClearResults()
    {
        m_results.clear();
        m_summary = TestSummary();
    }

    const std::vector<TestResult>& Analyzer::GetResults() const
    {
        return m_results;
    }

    const TestSummary& Analyzer::GetSummary() const
    {
        return m_summary;
    }

    const std::vector<std::shared_ptr<TestRule>>& Analyzer::GetRules() const
    {
        return m_rules;
    }
}
