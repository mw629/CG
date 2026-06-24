#include "HapiColiManager.h"
#include "Recorder.h"
#include "Analyzer.h"
#include "LogManager.h"
#include "Optimizer.h"
#include "UIManager.h"
#include "TestRunner.h"
#include "PlaybackManager.h"
#include "Fuzzer.h"

namespace HapiColi
{
    HapiColiManager::HapiColiManager()
    {
        m_recorder = std::make_unique<Recorder>();
        m_analyzer = std::make_unique<Analyzer>();
        m_logManager = std::make_unique<LogManager>();
        m_optimizer = std::make_unique<Optimizer>();
        m_uiManager = std::make_unique<UIManager>();
        m_testRunner = std::make_unique<TestRunner>();
        m_playbackManager = std::make_unique<PlaybackManager>();
        m_fuzzer = std::make_unique<Fuzzer>();
    }

    HapiColiManager::~HapiColiManager()
    {
    }

    void HapiColiManager::Initialize()
    {
        m_uiManager->Initialize(this);

        // Register the UI log callback
        LogManager::SetLogCallback([this](const std::string& msg) {
            if (this->m_uiManager) {
                this->m_uiManager->AddLog(msg);
            }
        });
    }

    void HapiColiManager::Update()
    {
        m_uiManager->Draw();
    }
    const char* HapiColiManager::GetActiveSubjectId() const
    {
        return m_uiManager->GetActiveSubjectId();
    }

    const char* HapiColiManager::GetActiveTargetId() const
    {
        return m_uiManager->GetActiveTargetId();
    }
}
