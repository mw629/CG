#include "Recorder.h"
#include <algorithm>

namespace HapiColi
{
    Recorder::Recorder()
        : m_isRecording(false)
        , m_currentFrameIndex(0)
    {
    }

    void Recorder::Start()
    {
        m_isRecording = true;
        m_currentFrameIndex = 0;
        Clear();
    }

    void Recorder::Stop()
    {
        m_isRecording = false;
    }

    bool Recorder::IsRecording() const
    {
        return m_isRecording;
    }

    void Recorder::BeginFrame(float deltaTime)
    {
        if (!m_isRecording) return;

        m_currentFrameData = FrameData();
        m_currentFrameData.frame = m_currentFrameIndex++;
        m_currentFrameData.deltaTime = deltaTime;
    }

    void Recorder::RecordObject(const ObjectData& objData)
    {
        if (!m_isRecording) return;

        // If targets are specified and this object is not in the targets, ignore it.
        // If targets list is empty, record everything.
        if (!m_targetIds.empty() && !IsTarget(objData.id))
        {
            return;
        }

        m_currentFrameData.objects.push_back(objData);
    }

    void Recorder::EndFrame()
    {
        if (!m_isRecording) return;
        m_recordedFrames.push_back(m_currentFrameData);
    }

    const std::vector<FrameData>& Recorder::GetRecordedFrames() const
    {
        return m_recordedFrames;
    }

    void Recorder::Clear()
    {
        m_recordedFrames.clear();
    }

    void Recorder::AddTargetId(const std::string& id)
    {
        if (!IsTarget(id))
        {
            m_targetIds.push_back(id);
        }
    }

    void Recorder::RemoveTargetId(const std::string& id)
    {
        m_targetIds.erase(std::remove(m_targetIds.begin(), m_targetIds.end(), id), m_targetIds.end());
    }

    bool Recorder::IsTarget(const std::string& id) const
    {
        return std::find(m_targetIds.begin(), m_targetIds.end(), id) != m_targetIds.end();
    }

    void Recorder::ClearTargets()
    {
        m_targetIds.clear();
    }
}
