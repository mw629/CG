#include "Analyzer.h"
#include "LogManager.h"
#include <string>

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
        LogManager::PrintOutput("Analyzer::Analyze() started. Frame count: " + std::to_string(frames.size()));
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
        LogManager::PrintOutput("Analyzer::Analyze() completed. Happy: " + std::to_string(m_summary.happyCount) + " Unhappy: " + std::to_string(m_summary.unhappyCount));
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

    void Analyzer::AnalyzeWarnings(const std::vector<FrameData>& frames)
    {
        m_warnings.clear();
        if (frames.empty()) return;

        // Detect Jitter and Deep Penetration
        for (size_t i = 1; i < frames.size(); ++i) {
            const auto& prevFrame = frames[i - 1];
            const auto& currFrame = frames[i];

            for (const auto& currObj : currFrame.objects) {
                auto prevObj = prevFrame.GetObjectById(currObj.id);
                if (!prevObj) continue;

                // Jitter Detection: If collision state toggles continuously
                if (currObj.collision.isColliding != prevObj->collision.isColliding) {
                    if (i > 1) {
                        const auto& prevPrevFrame = frames[i - 2];
                        auto prevPrevObj = prevPrevFrame.GetObjectById(currObj.id);
                        if (prevPrevObj && prevPrevObj->collision.isColliding == currObj.collision.isColliding) {
                            m_warnings.push_back({ currFrame.frame, "ガタつき（Jitter）を検知： " + currObj.id, "警告" });
                        }
                    }
                }

                // Deep Penetration Detection
                if (currObj.collision.isColliding && currObj.collision.hasContactInfo) {
                    // This is a proxy for deep penetration: if velocity was very high right before collision
                    float speedSq = prevObj->velocity.x * prevObj->velocity.x + 
                                    prevObj->velocity.y * prevObj->velocity.y + 
                                    prevObj->velocity.z * prevObj->velocity.z;
                    if (speedSq > 100.0f) { // Arbitrary high speed threshold
                        m_warnings.push_back({ currFrame.frame, "高速衝突（すり抜けリスクあり）： " + currObj.id, "警告" });
                        LogManager::PrintOutput("Analyzer::AnalyzeWarnings() Detected High Speed Collision: " + currObj.id);
                    }
                }
            }
        }
        
        LogManager::PrintOutput("Analyzer::AnalyzeWarnings() completed. Warnings found: " + std::to_string(m_warnings.size()));
    }
}
