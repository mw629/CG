#include "HapiColi.h"
#include "HapiColiManager/Recorder.h"

namespace HapiColi
{
    HapiColi::HapiColi()
    {
        m_manager = std::make_unique<HapiColiManager>();
    }

    HapiColi::~HapiColi()
    {
    }

    void HapiColi::Initialize()
    {
        m_manager->Initialize();
    }

    void HapiColi::Update()
    {
        m_manager->Update();
    }

    void HapiColi::BeginFrame(float deltaTime)
    {
        m_manager->GetRecorder()->BeginFrame(deltaTime);
    }

    void HapiColi::RecordObject(const ObjectData& objData)
    {
        m_manager->GetRecorder()->RecordObject(objData);
    }

    void HapiColi::EndFrame()
    {
        m_manager->GetRecorder()->EndFrame();
    }

    void HapiColi::DrawDebug3D(const float* viewProjMatrix)
    {
        if (m_manager) {
            m_manager->DrawDebug3D(viewProjMatrix);
        }
    }

    const char* HapiColi::GetActiveSubjectId() const
    {
        return m_manager ? m_manager->GetActiveSubjectId() : "";
    }

    const char* HapiColi::GetActiveTargetId() const
    {
        return m_manager ? m_manager->GetActiveTargetId() : "";
    }
}
