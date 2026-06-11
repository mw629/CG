#include "UIManager.h"
#include "HapiColiManager.h"
#include "Recorder.h"
#include "Analyzer.h"
#include "LogManager.h"
#include "Optimizer.h"
#include "TestRule.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace HapiColi
{
    void UIManager::Initialize(HapiColiManager* manager)
    {
        m_manager = manager;
    }

    void UIManager::Draw()
    {
        if (!m_manager) return;

        // Push HapiColi specific theme
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // Pure white text
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.05f, 0.15f, 0.05f, 0.95f)); // Dark green background
        ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.04f, 0.2f, 0.04f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.05f, 0.3f, 0.05f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.1f, 0.4f, 0.1f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.2f, 0.5f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.3f, 0.6f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.4f, 0.1f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.5f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.6f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.05f, 0.25f, 0.05f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.1f, 0.35f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.2f, 0.45f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.4f, 0.9f, 0.4f, 1.0f));

        ImGui::Begin("HapiColi Debugger");

        // Language Selector
        int langInt = static_cast<int>(m_language);
        ImGui::RadioButton("English", &langInt, 0); ImGui::SameLine();
        ImGui::RadioButton((const char*)u8"日本語", &langInt, 1);
        Language oldLanguage = m_language;
        m_language = static_cast<Language>(langInt);
        if (m_language != oldLanguage)
        {
            const auto& results = m_manager->GetAnalyzer()->GetResults();
            m_cachedSuggestions = m_manager->GetOptimizer()->GenerateSuggestions(results, m_language);
        }

        ImGui::Separator();

        if (ImGui::CollapsingHeader(GetText("Recorder", u8"記録 (Recorder)"), ImGuiTreeNodeFlags_DefaultOpen))
        {
            Recorder* recorder = m_manager->GetRecorder();
            if (recorder->IsRecording())
            {
                if (ImGui::Button(GetText("Stop Recording", u8"記録停止")))
                {
                    recorder->Stop();
                }
                ImGui::Text(GetText("Recording...", u8"記録中..."));
                ImGui::Text(GetText("Frames recorded: %d", u8"記録フレーム数: %d"), (int)recorder->GetRecordedFrames().size());
            }
            else
            {
                if (ImGui::Button(GetText("Start Recording", u8"記録開始")))
                {
                    recorder->Start();
                }
                ImGui::SameLine();
                if (ImGui::Button(GetText("Clear Data", u8"データ消去")))
                {
                    recorder->Clear();
                }
                ImGui::Text(GetText("Frames recorded: %d", u8"記録フレーム数: %d"), (int)recorder->GetRecordedFrames().size());

                ImGui::Separator();
                ImGui::InputText(GetText("Add Target ID", u8"対象ID追加"), m_targetBuffer, sizeof(m_targetBuffer));
                ImGui::SameLine();
                if (ImGui::Button(GetText("Add", u8"追加")))
                {
                    if (m_targetBuffer[0] != '\0')
                    {
                        recorder->AddTargetId(m_targetBuffer);
                        m_targetBuffer[0] = '\0';
                    }
                }
                if (ImGui::Button(GetText("Clear Targets", u8"対象クリア")))
                {
                    recorder->ClearTargets();
                }
            }
        }

        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
        ImGui::Text(GetText("=== Current Active Input ===", u8"=== 現在の入力先 ==="));
        if (m_activeInputTarget == 0) {
            ImGui::Text(GetText("-> SUBJECT: %s", u8"-> 主語ID: %s"), m_subjectBuffer[0] ? m_subjectBuffer : "(None)");
        } else {
            ImGui::Text(GetText("-> TARGET: %s", u8"-> 対象ID: %s"), m_ruleTargetBuffer[0] ? m_ruleTargetBuffer : "(None)");
        }
        ImGui::PopStyleColor();
        ImGui::Text(GetText("(Click a Registered Object below to copy its ID to the active input)", u8"(下のリストをクリックすると、現在の入力先にIDがコピーされます)"));
        ImGui::Separator();

        if (ImGui::CollapsingHeader(GetText("Registered Objects", u8"登録済みオブジェクト")))
        {
            const auto& knownIds = m_manager->GetRecorder()->GetKnownObjectIds();
            if (knownIds.empty())
            {
                ImGui::Text(GetText("No objects registered yet.", u8"まだ登録されたオブジェクトはありません。"));
            }
            else
            {
                for (const auto& id : knownIds)
                {
                    bool isSelected = false;
                    if (m_activeInputTarget == 0) isSelected = (id == m_subjectBuffer);
                    else isSelected = (id == m_ruleTargetBuffer);

                    if (ImGui::Selectable(id.c_str(), isSelected))
                    {
                        if (m_activeInputTarget == 1)
                        {
#ifdef _MSC_VER
                            strncpy_s(m_ruleTargetBuffer, id.c_str(), sizeof(m_ruleTargetBuffer));
#else
                            strncpy(m_ruleTargetBuffer, id.c_str(), sizeof(m_ruleTargetBuffer) - 1);
#endif
                        }
                        else
                        {
#ifdef _MSC_VER
                            strncpy_s(m_subjectBuffer, id.c_str(), sizeof(m_subjectBuffer));
#else
                            strncpy(m_subjectBuffer, id.c_str(), sizeof(m_subjectBuffer) - 1);
#endif
                        }
                    }
                }
            }
        }

        if (ImGui::CollapsingHeader(GetText("Analyzer", u8"解析 (Analyzer)")))
        {
            if (m_activeInputTarget == 0) ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.4f, 0.4f, 0.0f, 1.0f));
            ImGui::InputText(GetText("Subject ID", u8"主語ID"), m_subjectBuffer, sizeof(m_subjectBuffer));
            if (m_activeInputTarget == 0) ImGui::PopStyleColor();
            if (ImGui::IsItemFocused() || ImGui::IsItemActivated() || ImGui::IsItemClicked()) m_activeInputTarget = 0;

            ImGui::RadioButton(GetText("Expected Hit", u8"当たるべき"), &m_ruleType, 0); ImGui::SameLine();
            ImGui::RadioButton(GetText("Expected No Hit", u8"当たらないべき"), &m_ruleType, 1);

            if (m_activeInputTarget == 1) ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.4f, 0.4f, 0.0f, 1.0f));
            ImGui::InputText(GetText("Target ID", u8"対象ID"), m_ruleTargetBuffer, sizeof(m_ruleTargetBuffer));
            if (m_activeInputTarget == 1) ImGui::PopStyleColor();
            if (ImGui::IsItemFocused() || ImGui::IsItemActivated() || ImGui::IsItemClicked()) m_activeInputTarget = 1;

            if (ImGui::Button(GetText("Add Rule", u8"ルール追加")))
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
            if (ImGui::Button(GetText("Clear Rules", u8"ルールクリア")))
            {
                m_manager->GetAnalyzer()->ClearRules();
            }

            if (ImGui::Button(GetText("Run Analysis", u8"解析実行")))
            {
                m_manager->GetAnalyzer()->AnalyzeAppend(m_manager->GetRecorder()->GetRecordedFrames());
                m_cachedSuggestions.clear();
            }

            ImGui::SameLine();
            if (ImGui::Button(GetText("Clear Results", u8"結果クリア")))
            {
                m_manager->GetAnalyzer()->ClearResults();
                m_cachedSuggestions.clear();
            }

            const auto& summary = m_manager->GetAnalyzer()->GetSummary();
            ImGui::Text(GetText("Total Tests: %d", u8"総テスト数: %d"), summary.totalTests);
            ImGui::TextColored(ImVec4(0, 1, 0, 1), GetText("Happy: %d", u8"成功(Happy): %d"), summary.happyCount);
            ImGui::TextColored(ImVec4(1, 0, 0, 1), GetText("Unhappy: %d", u8"失敗(Unhappy): %d"), summary.unhappyCount);
            ImGui::Text(GetText("Success Rate: %.1f%%", u8"成功率: %.1f%%"), summary.successRate * 100.0f);
        }

        if (ImGui::CollapsingHeader(GetText("Optimizer", u8"改善提案 (Optimizer)")))
        {
            if (ImGui::Button(GetText("Generate Suggestions", u8"改善案を生成")))
            {
                const auto& results = m_manager->GetAnalyzer()->GetResults();
                m_cachedSuggestions = m_manager->GetOptimizer()->GenerateSuggestions(results, m_language);
            }
            
            for (const auto& sug : m_cachedSuggestions)
            {
                ImGui::TextWrapped("- %s", sug.c_str());
            }
        }

        if (ImGui::CollapsingHeader(GetText("File I/O", u8"ファイル入出力")))
        {
            if (ImGui::Button(GetText("Save JSON", u8"JSON保存")))
            {
                m_isSaving = true;
                
                auto framesToSave = m_manager->GetRecorder()->GetRecordedFrames();
                LogManager* logManager = m_manager->GetLogManager();
                
                std::thread([logManager, framesToSave, this]() {
                    auto now = std::chrono::system_clock::now();
                    auto time = std::chrono::system_clock::to_time_t(now);
                    struct tm timeinfo;
#ifdef _MSC_VER
                    localtime_s(&timeinfo, &time);
#else
                    timeinfo = *std::localtime(&time);
#endif
                    char timeBuf[256];
                    snprintf(timeBuf, sizeof(timeBuf), (const char*)u8"%02d年%02d月%02d日%02d時%02d分DebugLog", 
                             timeinfo.tm_year % 100, 
                             timeinfo.tm_mon + 1, 
                             timeinfo.tm_mday, 
                             timeinfo.tm_hour, 
                             timeinfo.tm_min);
                    
                    std::string logDir = "c:\\TechnicalSchool\\MyEngine\\HapiColi\\Log";
                    std::string logPath = logDir + "\\" + timeBuf + ".json";
                    
                    system(("mkdir \"" + logDir + "\" 2> nul").c_str());
                    logManager->SaveFrames(logPath, framesToSave);
                    
                    m_isSaving = false;
                }).detach();
            }
        }

        ImGui::End();
        
        // Pop all pushed colors
        ImGui::PopStyleColor(14);
    }
}
