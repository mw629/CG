#include "TestRunner.h"

namespace HapiColi
{
    void TestRunner::SetTestStepCallback(std::function<void()> callback)
    {
        m_testStepCallback = callback;
    }

    void TestRunner::RunTests(int iterations)
    {
        if (!m_testStepCallback) return;

        for (int i = 0; i < iterations; ++i)
        {
            // Execute the engine test simulation step
            m_testStepCallback();
        }
    }

    void TestRunner::ResetTest()
    {
        // コールバックはそのままにして、必要に応じてエンジン側でリセット処理を行う
        // 呼び出し元(UIManager)でRecorder/Analyzerのクリアも行う
    }
}
