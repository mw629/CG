#pragma once

#include <memory>

namespace HapiColi
{
    class Recorder;
    class Analyzer;
    class LogManager;
    class Optimizer;
    class UIManager;
    class TestRunner;
    class PlaybackManager;
    class Fuzzer;

    class HapiColiManager
    {
    public:
        HapiColiManager();
        ~HapiColiManager();

        void Initialize();
        void Update(); // Called every frame to render UI and manage state

        const char* GetActiveSubjectId() const;
        const char* GetActiveTargetId() const;

        Recorder* GetRecorder() const { return m_recorder.get(); }
        Analyzer* GetAnalyzer() const { return m_analyzer.get(); }
        LogManager* GetLogManager() const { return m_logManager.get(); }
        Optimizer* GetOptimizer() const { return m_optimizer.get(); }
        UIManager* GetUIManager() const { return m_uiManager.get(); }
        TestRunner* GetTestRunner() const { return m_testRunner.get(); }
        PlaybackManager* GetPlaybackManager() const { return m_playbackManager.get(); }
        Fuzzer* GetFuzzer() const { return m_fuzzer.get(); }

    private:
        std::unique_ptr<Recorder> m_recorder;
        std::unique_ptr<Analyzer> m_analyzer;
        std::unique_ptr<LogManager> m_logManager;
        std::unique_ptr<Optimizer> m_optimizer;
        std::unique_ptr<UIManager> m_uiManager;
        std::unique_ptr<TestRunner> m_testRunner;
        std::unique_ptr<PlaybackManager> m_playbackManager;
        std::unique_ptr<Fuzzer> m_fuzzer;
    };
}
