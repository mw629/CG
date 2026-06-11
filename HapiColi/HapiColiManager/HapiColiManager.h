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

    class HapiColiManager
    {
    public:
        HapiColiManager();
        ~HapiColiManager();

        void Initialize();
        void Update(); // Called every frame to render UI and manage state

        Recorder* GetRecorder() const { return m_recorder.get(); }
        Analyzer* GetAnalyzer() const { return m_analyzer.get(); }
        LogManager* GetLogManager() const { return m_logManager.get(); }
        Optimizer* GetOptimizer() const { return m_optimizer.get(); }
        UIManager* GetUIManager() const { return m_uiManager.get(); }
        TestRunner* GetTestRunner() const { return m_testRunner.get(); }

    private:
        std::unique_ptr<Recorder> m_recorder;
        std::unique_ptr<Analyzer> m_analyzer;
        std::unique_ptr<LogManager> m_logManager;
        std::unique_ptr<Optimizer> m_optimizer;
        std::unique_ptr<UIManager> m_uiManager;
        std::unique_ptr<TestRunner> m_testRunner;
    };
}
