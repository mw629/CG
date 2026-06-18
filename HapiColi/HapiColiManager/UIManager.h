#pragma once

#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include "HapiColiTypes.h"
#include "FrameData.h"

namespace HapiColi
{
    class HapiColiManager;

    class UIManager
    {
    public:
        void Initialize(HapiColiManager* manager);
        void Draw();

        const char* GetActiveSubjectId() const { return m_subjectBuffer; }
        const char* GetActiveTargetId() const { return m_targetBuffer; }

        void AddLog(const std::string& message);

    private:
        HapiColiManager* m_manager = nullptr;
        
        char m_targetBuffer[128] = {0};
        char m_subjectBuffer[128] = {0};
        char m_ruleTargetBuffer[128] = {0};
        int m_ruleType = 0; // 0: Hit, 1: NoHit
        int m_activeInputTarget = 0; // 0: Subject, 1: Target
        int m_testIterations = 100;

        Language m_language = Language::English;
        const char* GetText(const char* en, const void* jp) const {
            return m_language == Language::Japanese ? static_cast<const char*>(jp) : en;
        }

        std::vector<std::string> m_cachedSuggestions;
        std::atomic<bool> m_isSaving{false};

        std::mutex m_logMutex;
        std::vector<std::string> m_logLines;
    };
}
