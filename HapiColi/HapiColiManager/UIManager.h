#pragma once

namespace HapiColi
{
    class HapiColiManager;

    class UIManager
    {
    public:
        void Initialize(HapiColiManager* manager);
        void Draw();

    private:
        HapiColiManager* m_manager = nullptr;
        
        char m_targetBuffer[128] = {0};
        char m_subjectBuffer[128] = {0};
        char m_ruleTargetBuffer[128] = {0};
        int m_ruleType = 0; // 0: Hit, 1: NoHit
    };
}
