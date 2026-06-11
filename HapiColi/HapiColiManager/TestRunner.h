#pragma once

#include <functional>

namespace HapiColi
{
    class TestRunner
    {
    public:
        // Registers a callback to execute a game test step
        // Engine will inject this logic since HapiColi can't drive the engine directly
        void SetTestStepCallback(std::function<void()> callback);

        void RunTests(int iterations);

    private:
        std::function<void()> m_testStepCallback;
    };
}
