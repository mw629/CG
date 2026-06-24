#include "TestRunner.h"
#include "LogManager.h"
#include <string>

namespace HapiColi
{
    void TestRunner::SetTestStepCallback(std::function<void()> callback)
    {
        m_testStepCallback = callback;
    }

    void TestRunner::RunTests(int iterations)
    {
        if (!m_testStepCallback) return;

        LogManager::PrintOutput("TestRunner::RunTests() started. Iterations: " + std::to_string(iterations));

        for (int i = 0; i < iterations; ++i)
        {
            // Execute the engine test simulation step
            m_testStepCallback();
        }

        LogManager::PrintOutput("TestRunner::RunTests() completed.");
    }

    void TestRunner::ResetTest()
    {
        // コールバックはそのままにして、必要に応じてエンジン側でリセット処理を行う
        // 呼び出し元(UIManager)でRecorder/Analyzerのクリアも行う
    }
}
