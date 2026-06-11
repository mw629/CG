#include "HapiColiManager.h"
#include "Recorder.h"
#include "Analyzer.h"
#include "LogManager.h"
#include "Optimizer.h"
#include "UIManager.h"
#include "TestRunner.h"

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
    }

    HapiColiManager::~HapiColiManager()
    {
    }

    void HapiColiManager::Initialize()
    {
        m_uiManager->Initialize(this);
    }

    void HapiColiManager::Update()
    {
        m_uiManager->Draw();
    }
}
