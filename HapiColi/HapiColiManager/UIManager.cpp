#include "UIManager.h"
#include "HapiColiManager.h"
#include "Recorder.h"
#include "Analyzer.h"
#include "LogManager.h"
#include "Optimizer.h"
#include "Fuzzer.h"
#include "TestRule.h"
#include "PlaybackManager.h"
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
                    AddLog("Recording stopped.");
                }
                ImGui::Text(GetText("Recording...", u8"記録中..."));
                ImGui::Text(GetText("Frames recorded: %d", u8"記録フレーム数: %d"), (int)recorder->GetRecordedFrames().size());
            }
            else
            {
                if (ImGui::Button(GetText("Start Recording", u8"記録開始")))
                {
                    recorder->Start();
                    AddLog("Recording started.");
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

        ImGui::End(); // End Debugger window temporarily

        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 250), ImGuiCond_FirstUseEver);
        ImGui::Begin(GetText("HapiColi Playback", u8"HapiColi 再生 (Playback)"));
        if (true) // Just an always-true block to keep the scope clean, replacing CollapsingHeader
        {
            auto playback = m_manager->GetPlaybackManager();
            if (playback)
            {
                bool isReplay = playback->IsReplayMode();
                if (ImGui::Checkbox(GetText("Enable Replay Mode", u8"リプレイモードを有効化"), &isReplay))
                {
                    playback->SetReplayMode(isReplay);
                    if (isReplay) {
                        playback->SetSimulationPaused(true);
                    } else {
                        playback->SetSimulationPaused(false); // リプレイ終了時は一時停止を解除
                    }
                }

                if (isReplay)
                {
                    int totalFrames = (int)m_manager->GetRecorder()->GetRecordedFrames().size();
                    if (totalFrames > 0)
                    {
                        int maxFrame = totalFrames - 1;
                        int currentFrame = playback->GetReplayFrameIndex();
                        if (currentFrame > maxFrame) currentFrame = maxFrame;
                        
                        bool isReplayPlaying = playback->IsReplayPlaying();
                        if (ImGui::Button(isReplayPlaying ? GetText("Pause Replay", u8"リプレイ一時停止") : GetText("Play Replay", u8"リプレイ再生")))
                        {
                            if (!isReplayPlaying && currentFrame >= maxFrame) {
                                playback->SetReplayFrameIndex(0);
                            }
                            playback->SetReplayPlaying(!isReplayPlaying);
                        }
                        
                        ImGui::SameLine();
                        float timeScale = playback->GetTimeScale();
                        ImGui::SetNextItemWidth(100.0f);
                        if (ImGui::SliderFloat(GetText("Speed", u8"速度"), &timeScale, 0.1f, 3.0f, "%.2fx"))
                        {
                            playback->SetTimeScale(timeScale);
                        }

                        if (ImGui::SliderInt(GetText("Timeline", u8"タイムライン"), &currentFrame, 0, maxFrame))
                        {
                            playback->SetReplayFrameIndex(currentFrame);
                            playback->SetReplayPlaying(false);
                        }
                    }
                    else
                    {
                        ImGui::Text(GetText("No recorded frames available.", u8"記録されたフレームがありません。"));
                    }
                }
            }
        }
        ImGui::End(); // End Playback window

        ImGui::Begin("HapiColi Debugger"); // Resume Debugger window

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

            // --- 現在のルール一覧（開閉可能） ---
            const auto& rules = m_manager->GetAnalyzer()->GetRules();
            char ruleHeader[64];
            snprintf(ruleHeader, sizeof(ruleHeader),
                GetText("Current Rules (%d)###RuleList", u8"現在のルール (%d)###RuleList"),
                (int)rules.size());

            if (ImGui::CollapsingHeader(ruleHeader))
            {
                if (rules.empty())
                {
                    ImGui::TextDisabled(GetText("  No rules set.", u8"  ルールが設定されていません。"));
                }
                else
                {
                    ImGui::Indent(8.0f);
                    if (ImGui::BeginTable("RuleTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
                    {
                        ImGui::TableSetupColumn(GetText("Type",    u8"種別"),    ImGuiTableColumnFlags_WidthFixed, 90.0f);
                        ImGui::TableSetupColumn(GetText("Subject", u8"主語ID"),  ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn(GetText("Target",  u8"対象ID"),  ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableHeadersRow();

                        for (int i = 0; i < (int)rules.size(); ++i)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);

                            bool isHit = (rules[i]->GetName() == "ExpectedHitRule");
                            if (isHit)
                                ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), GetText("Hit",   u8"当たるべき"));
                            else
                                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), GetText("NoHit", u8"当たらないべき"));

                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted(rules[i]->GetSubjectId().c_str());

                            ImGui::TableSetColumnIndex(2);
                            ImGui::TextUnformatted(rules[i]->GetTargetId().c_str());
                        }
                        ImGui::EndTable();
                    }
                    ImGui::Unindent(8.0f);
                }
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
            ImGui::Text(GetText("Total Frames: %d", u8"総記録フレーム数: %d"), summary.totalTests);
            ImGui::TextColored(ImVec4(0, 1, 0, 1), GetText("Happy Frames: %d", u8"成功フレーム(Happy): %d"), summary.happyCount);
            ImGui::TextColored(ImVec4(1, 0, 0, 1), GetText("Unhappy Frames: %d", u8"失敗フレーム(Unhappy): %d"), summary.unhappyCount);
            ImGui::Text(GetText("Success Rate: %.1f%%", u8"成功率(フレーム割合): %.1f%%"), summary.successRate * 100.0f);
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
                
                auto framesToSave  = m_manager->GetRecorder()->GetRecordedFrames();
                auto resultsToSave = m_manager->GetAnalyzer()->GetResults(); // Unhappy抽出用
                LogManager* logManager = m_manager->GetLogManager();
                
                std::thread([logManager, framesToSave, resultsToSave, this]() {
                    auto now = std::chrono::system_clock::now();
                    auto time = std::chrono::system_clock::to_time_t(now);
                    struct tm timeinfo;
#ifdef _MSC_VER
                    localtime_s(&timeinfo, &time);
#else
                    timeinfo = *std::localtime(&time);
#endif
                    char timeBuf[64];
                    snprintf(timeBuf, sizeof(timeBuf), "%02d_%02d_%02d_%02d_%02d",
                             timeinfo.tm_year % 100,
                             timeinfo.tm_mon + 1,
                             timeinfo.tm_mday,
                             timeinfo.tm_hour,
                             timeinfo.tm_min);

                    // 実行環境の相対パスで保存するように変更
                    std::string logDir  = "HapiColi\\Log";
                    std::string logPath = logDir + "\\" + timeBuf + ".json";
                    std::string unhappyPath = logDir + "\\" + timeBuf + "_Unhappy.json";
                    std::string summaryPath = logDir + "\\" + timeBuf + "_CollisionSummary.json";
                    
                    system(("mkdir \"" + logDir + "\" 2> nul").c_str());

                    // 全フレームを保存
                    logManager->SaveFrames(logPath, framesToSave);

                    // Unhappy な結果 + 対応フレームを別ファイルに保存
                    logManager->SaveUnhappyReport(unhappyPath, resultsToSave, framesToSave);

                    // 何フレーム目から何フレーム目まで当たっていたかのサマリーを保存
                    logManager->SaveCollisionSummary(summaryPath, framesToSave);
                    
                    AddLog("Saved JSON log files successfully.");
                    m_isSaving = false;
                }).detach();
            }
        }

        ImGui::Separator();
        if (ImGui::CollapsingHeader(GetText("Fuzzer (Auto Test)", u8"ファザー (自動テスト)")))
        {
            Fuzzer* fuzzer = m_manager->GetFuzzer();
            
            auto& config = fuzzer->GetConfig();
            ImGui::Checkbox(GetText("Enable Micro-offset Test", u8"微小ゆらぎテストを有効化"), &config.enableMicroOffsetTest);
            ImGui::SliderFloat(GetText("Offset Range", u8"ゆらぎの範囲 (レベル)"), &config.microOffsetRange, 0.001f, 0.5f, "%.3f");
            ImGui::SliderInt(GetText("Trials", u8"試行回数"), &config.microOffsetTrials, 1, 500);

            if (ImGui::Button(GetText("Run Fuzzing", u8"ファジング実行"))) {
                fuzzer->RunAll();
                AddLog("Fuzzing run completed.");
            }
            ImGui::SameLine();
            if (ImGui::Button(GetText("Clear Fuzz Results", u8"結果クリア"))) {
                fuzzer->ClearResults();
            }

            const auto& fuzzResults = fuzzer->GetResults();
            if (!fuzzResults.empty()) {
                ImGui::Text(GetText("Fuzz Results: %d", u8"ファジング結果: %d件"), (int)fuzzResults.size());
                ImGui::BeginChild("FuzzRes", ImVec2(0, 150), true);
                for (const auto& res : fuzzResults) {
                    if (res.isBugCaught) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1)); // Red for bugs
                    } else {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1)); // Green for passed
                    }
                    ImGui::Text("[%s] %s", res.testName.c_str(), res.description.c_str());
                    ImGui::PopStyleColor();
                }
                ImGui::EndChild();
            }
        }

        ImGui::Separator();
        if (ImGui::CollapsingHeader(GetText("Analyzer Warnings", u8"アナライザー警告")))
        {
            Analyzer* analyzer = m_manager->GetAnalyzer();
            if (ImGui::Button(GetText("Analyze Recorded Frames", u8"記録フレームを分析"))) {
                analyzer->AnalyzeWarnings(m_manager->GetRecorder()->GetRecordedFrames());
                AddLog("Analyzer run completed.");
            }

            const auto& warnings = analyzer->GetWarnings();
            if (warnings.empty()) {
                ImGui::Text(GetText("No warnings.", u8"警告はありません。"));
            } else {
                ImGui::BeginChild("WarnRes", ImVec2(0, 150), true);
                for (const auto& w : warnings) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1)); // Yellow
                    ImGui::Text("Frame %d: %s", w.frame, w.message.c_str());
                    ImGui::PopStyleColor();
                }
                ImGui::EndChild();
            }
        }

        ImGui::End();
        
        // --- HapiColi Log Window ---
        ImGui::SetNextWindowPos(ImVec2(50, 350), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin(GetText("HapiColi Log", u8"HapiColi ログ (Log)"));
        if (ImGui::Button(GetText("Clear Log", u8"ログクリア"))) {
            std::lock_guard<std::mutex> lock(m_logMutex);
            m_logLines.clear();
        }
        ImGui::Separator();
        ImGui::BeginChild("LogRegion", ImVec2(0, 0), true);
        {
            std::lock_guard<std::mutex> lock(m_logMutex);
            for (const auto& line : m_logLines) {
                ImGui::TextWrapped("%s", line.c_str());
            }
        }
        ImGui::EndChild();
        ImGui::End();

        // Pop all pushed colors
        ImGui::PopStyleColor(14);
    }

    void UIManager::AddLog(const std::string& msg)
    {
        std::lock_guard<std::mutex> lock(m_logMutex);
        m_logLines.push_back(msg);
        if (m_logLines.size() > 500) {
            m_logLines.erase(m_logLines.begin());
        }
    }
}
