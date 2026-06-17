#include "HapiColi.h"
#include "HapiColiManager/Recorder.h"
#include "HapiColiManager/Fuzzer.h"
#include "HapiColiManager/PlaybackManager.h"
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
        auto playback = m_manager->GetPlaybackManager();
        if (playback && playback->IsReplayMode()) {
            int maxFrames = (int)m_manager->GetRecorder()->GetRecordedFrames().size();
            playback->Update(deltaTime, maxFrames);
            return;
        }
        m_manager->GetRecorder()->BeginFrame(deltaTime);
    }

    void HapiColi::RecordObject(const ObjectData& objData)
    {
        auto playback = m_manager->GetPlaybackManager();
        if (playback && playback->IsReplayMode()) return;
        m_manager->GetRecorder()->RecordObject(objData);
    }

    void HapiColi::EndFrame()
    {
        auto playback = m_manager->GetPlaybackManager();
        if (playback && playback->IsReplayMode()) return;

        if (m_manager && m_manager->GetRecorder())
        {
            m_manager->GetRecorder()->EndFrame();
        }
    }

    void HapiColi::RegisterFuzzTarget(const std::string& name, const ObjectData& baseA, const ObjectData& baseB, std::function<void(ObjectData&, ObjectData&)> func)
    {
        if (m_manager && m_manager->GetFuzzer())
        {
            m_manager->GetFuzzer()->RegisterTarget(name, baseA, baseB, func);
        }
    }

    void HapiColi::UpdateFuzzTarget(const std::string& name, const ObjectData& baseA, const ObjectData& baseB)
    {
        if (m_manager && m_manager->GetFuzzer())
        {
            m_manager->GetFuzzer()->UpdateTarget(name, baseA, baseB);
        }
    }

    void HapiColi::BuildRenderCommands()
    {
        auto playback = m_manager->GetPlaybackManager();
        if (playback && playback->IsReplayMode())
        {
            const auto& frames = m_manager->GetRecorder()->GetRecordedFrames();
            int idx = playback->GetReplayFrameIndex();
            if (!frames.empty() && idx >= 0 && idx < frames.size())
            {
                renderer_->BuildCommands(frames[idx].objects);
            }
            else
            {
                renderer_->ClearCommands();
            }
        }
        else
        {
            renderer_->BuildCommands(m_manager->GetRecorder()->GetRealtimeObjects());
        }
    }

    const std::vector<RenderCommand>& HapiColi::GetRenderCommands() const
    {
        return renderer_->GetCommands();
    }

    const std::vector<SolidRenderCommand>& HapiColi::GetSolidRenderCommands() const
    {
        return renderer_->GetSolidRenderCommands();
    }

    const char* HapiColi::GetActiveSubjectId() const
    {
        return m_manager ? m_manager->GetActiveSubjectId() : "";
    }

    const char* HapiColi::GetActiveTargetId() const
    {
        return m_manager ? m_manager->GetActiveTargetId() : "";
    }

    PlaybackManager* HapiColi::GetPlaybackManager() const
    {
        return m_manager ? m_manager->GetPlaybackManager() : nullptr;
    }
}
