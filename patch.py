import os

path = r'c:\TechnicalSchool\MyEngine\HapiColi\HapiColiManager\UIManager.cpp'

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

content = content.replace('#include "TestRule.h"', '#include "TestRule.h"\n#include "TestRunner.h"')

testRunner = '''        if (ImGui::CollapsingHeader(GetText("Test Runner", u8"自動テスト実行 (Test Runner)")))
        {
            ImGui::InputInt(GetText("Iterations", u8"試行回数"), &m_testIterations);
            if (m_testIterations < 1) m_testIterations = 1;

            if (ImGui::Button(GetText("Run Tests", u8"テスト実行")))
            {
                auto recorder = m_manager->GetRecorder();
                recorder->Start(false); // 記録開始（以前のデータをクリアせずに追記）
                m_manager->GetTestRunner()->RunTests(m_testIterations);
                recorder->Stop(); // 記録終了
            }
            ImGui::SameLine();
            if (ImGui::Button(GetText("Reset Test", u8"リセット")))
            {
                m_manager->GetTestRunner()->ResetTest();
                m_manager->GetRecorder()->Clear(); // データも初期化する
                m_manager->GetAnalyzer()->ClearResults(); // 解析結果も初期化する
                m_cachedSuggestions.clear();
            }
        }

        if (ImGui::CollapsingHeader(GetText("Analyzer", u8"解析 (Analyzer)")))'''

content = content.replace('        if (ImGui::CollapsingHeader(GetText("Analyzer", u8"解析 (Analyzer)"))), testRunner)

runAnalysisOld = '''            if (ImGui::Button(GetText("Run Analysis", u8"解析実行")))
            {
                m_manager->GetAnalyzer()->Analyze(m_manager->GetRecorder()->GetRecordedFrames());
                m_cachedSuggestions.clear();
            }

            ImGui::SameLine();
            if (ImGui::Button(GetText("Clear Results", u8"結果クリア")))
            {
                m_manager->GetAnalyzer()->ClearResults();
                m_cachedSuggestions.clear();
            }'''

runAnalysisNew = '''            if (ImGui::Button(GetText("Run Analysis", u8"解析実行")))
            {
                auto recorder = m_manager->GetRecorder();
                recorder->Start(false); // データを追記
                m_manager->GetTestRunner()->RunTests(m_testIterations);
                recorder->Stop();

                m_manager->GetAnalyzer()->Analyze(m_manager->GetRecorder()->GetRecordedFrames());
                m_cachedSuggestions.clear();
            }

            ImGui::SameLine();
            if (ImGui::Button(GetText("Clear Results", u8"結果クリア")))
            {
                m_manager->GetAnalyzer()->ClearResults();
                m_cachedSuggestions.clear();
            }'''

content = content.replace(runAnalysisOld, runAnalysisNew)

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)
