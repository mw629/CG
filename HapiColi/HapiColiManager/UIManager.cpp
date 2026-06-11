#include "UIManager.h"
#include "HapiColiManager.h"
#include "Recorder.h"
#include "Analyzer.h"
#include "LogManager.h"
#include "Optimizer.h"
#include "TestRule.h"
#include <imgui.h>
#include <string>

namespace HapiColi
{
    void UIManager::Initialize(HapiColiManager* manager)
    {
        m_manager = manager;
    }

    void UIManager::Draw()
    {
        if (!m_manager) return;

        ImGui::Begin("HapiColi Debugger");

        if (ImGui::CollapsingHeader("Recorder", ImGuiTreeNodeFlags_DefaultOpen))
        {
            Recorder* recorder = m_manager->GetRecorder();
            if (recorder->IsRecording())
            {
                if (ImGui::Button("Stop Recording"))
                {
                    recorder->Stop();
                }
                ImGui::Text("Recording...");
                ImGui::Text("Frames recorded: %d", (int)recorder->GetRecordedFrames().size());
            }
            else
            {
                if (ImGui::Button("Start Recording"))
                {
                    recorder->Start();
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear Data"))
                {
                    recorder->Clear();
                }
                ImGui::Text("Frames recorded: %d", (int)recorder->GetRecordedFrames().size());

                ImGui::Separator();
                ImGui::InputText("Add Target ID", m_targetBuffer, sizeof(m_targetBuffer));
                ImGui::SameLine();
                if (ImGui::Button("Add"))
                {
                    if (m_targetBuffer[0] != '\0')
                    {
                        recorder->AddTargetId(m_targetBuffer);
                        m_targetBuffer[0] = '\0';
                    }
                }
                if (ImGui::Button("Clear Targets"))
                {
                    recorder->ClearTargets();
                }
            }
        }

        if (ImGui::CollapsingHeader("Analyzer"))
        {
            ImGui::InputText("Subject ID", m_subjectBuffer, sizeof(m_subjectBuffer));
            ImGui::RadioButton("Expected Hit", &m_ruleType, 0); ImGui::SameLine();
            ImGui::RadioButton("Expected No Hit", &m_ruleType, 1);
            ImGui::InputText("Target ID", m_ruleTargetBuffer, sizeof(m_ruleTargetBuffer));

            if (ImGui::Button("Add Rule"))
            {
                std::string subject = m_subjectBuffer;
                std::string target = m_ruleTargetBuffer;
                if (!subject.empty() && !target.empty())
                {
                    if (m_ruleType == 0)
                        m_manager->GetAnalyzer()->AddRule(std::make_shared<ExpectedHitRule>(subject, target));
                    else
                        m_manager->GetAnalyzer()->AddRule(std::make_shared<ExpectedNoHitRule>(subject, target));
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Clear Rules"))
            {
                m_manager->GetAnalyzer()->ClearRules();
            }

            if (ImGui::Button("Run Analysis"))
            {
                m_manager->GetAnalyzer()->Analyze(m_manager->GetRecorder()->GetRecordedFrames());
            }

            const auto& summary = m_manager->GetAnalyzer()->GetSummary();
            ImGui::Text("Total Tests: %d", summary.totalTests);
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Happy: %d", summary.happyCount);
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Unhappy: %d", summary.unhappyCount);
            ImGui::Text("Success Rate: %.1f%%", summary.successRate * 100.0f);
        }

        if (ImGui::CollapsingHeader("Optimizer"))
        {
            if (ImGui::Button("Generate Suggestions"))
            {
                const auto& results = m_manager->GetAnalyzer()->GetResults();
                auto suggestions = m_manager->GetOptimizer()->GenerateSuggestions(results);
                for (const auto& sug : suggestions)
                {
                    ImGui::TextWrapped("- %s", sug.c_str());
                }
            }
        }

        if (ImGui::CollapsingHeader("File I/O"))
        {
            if (ImGui::Button("Save JSON"))
            {
                m_manager->GetLogManager()->SaveFrames("hapicoli_log.json", m_manager->GetRecorder()->GetRecordedFrames());
            }
        }

        ImGui::End();
    }
}
