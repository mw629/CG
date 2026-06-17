#include "HapiColi.h"
#include "HapiColiManager/Recorder.h"
#include "HapiColiRenderer.h"

namespace HapiColi
{
    HapiColi::HapiColi()
    {
        m_manager = std::make_unique<HapiColiManager>();
        renderer_ = std::make_unique<HapiColiRenderer>();
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

    void HapiColi::BuildRenderCommands()
    {
        renderer_->BuildCommands(m_manager->GetRecorder()->GetRealtimeObjects());
    }

    const std::vector<RenderCommand>& HapiColi::GetRenderCommands() const
    {
        return renderer_->GetCommands();
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
