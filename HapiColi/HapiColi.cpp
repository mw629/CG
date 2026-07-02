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

    bool HapiColi::CheckAABBAndRecord(
        const std::string& idA, const Vector3& posA, const Vector3& sizeA,
        const std::string& idB, const Vector3& posB, const Vector3& sizeB,
        const std::string& typeA, const std::string& typeB)
    {
        // 1. AABB交差判定 (底面中央を基準として計算)
        float minX_A = posA.x - sizeA.x / 2.0f;
        float maxX_A = posA.x + sizeA.x / 2.0f;
        float minY_A = posA.y;
        float maxY_A = posA.y + sizeA.y;
        float minZ_A = posA.z - sizeA.z / 2.0f;
        float maxZ_A = posA.z + sizeA.z / 2.0f;

        float minX_B = posB.x - sizeB.x / 2.0f;
        float maxX_B = posB.x + sizeB.x / 2.0f;
        float minY_B = posB.y;
        float maxY_B = posB.y + sizeB.y;
        float minZ_B = posB.z - sizeB.z / 2.0f;
        float maxZ_B = posB.z + sizeB.z / 2.0f;

        bool hit = true;
        if (maxX_A < minX_B || minX_A > maxX_B) hit = false;
        if (maxY_A < minY_B || minY_A > maxY_B) hit = false;
        if (maxZ_A < minZ_B || minZ_A > maxZ_B) hit = false;

        // 2. 描画用に中心座標を計算 (HapiColiRendererは中心座標を基準に描画するため)
        Vector3 centerA = { posA.x, posA.y + sizeA.y / 2.0f, posA.z };
        Vector3 centerB = { posB.x, posB.y + sizeB.y / 2.0f, posB.z };

        // 3. データ生成
        ObjectData objA = ObjectData::CreateBox(idA, centerA, sizeA);
        objA.type = typeA;
        ObjectData objB = ObjectData::CreateBox(idB, centerB, sizeB);
        objB.type = typeB;

        if (hit) {
            objA.SetCollision(idB);
            objB.SetCollision(idA);
        }

        // 4. 重複をマージしながら記録
        auto playback = m_manager->GetPlaybackManager();
        if (!playback || !playback->IsReplayMode()) {
            m_manager->GetRecorder()->RecordObjectMerge(objA);
            m_manager->GetRecorder()->RecordObjectMerge(objB);
        }

        return hit;
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
