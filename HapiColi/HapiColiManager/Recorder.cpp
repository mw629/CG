#include "Recorder.h"
#include "LogManager.h"
#include <algorithm>
#include <string>

namespace HapiColi
{
    Recorder::Recorder()
        : m_isRecording(false)
        , m_currentFrameIndex(0)
    {
    }

    void Recorder::Start(bool clearData)
    {
        LogManager::PrintOutput("Recorder::Start() invoked. clearData=" + std::string(clearData ? "true" : "false"));
        m_isRecording = true;
        if (clearData)
        {
            m_currentFrameIndex = 0;
            Clear();
        }
    }

    void Recorder::Stop()
    {
        LogManager::PrintOutput("Recorder::Stop() invoked. Recorded frames: " + std::to_string(m_recordedFrames.size()));
        m_isRecording = false;
    }

    bool Recorder::IsRecording() const
    {
        return m_isRecording;
    }

    void Recorder::BeginFrame(float deltaTime)
    {
        m_realtimeObjects.clear();
        
        if (!m_isRecording) return;

        m_currentFrameData = FrameData();
        m_currentFrameData.frame = m_currentFrameIndex++;
        m_currentFrameData.deltaTime = deltaTime;
    }

    void Recorder::RecordObject(const ObjectData& objData)
    {
        // Keep track of known IDs for UI regardless of recording state
        // so the user can see available objects before starting to record
        if (std::find(m_knownObjectIds.begin(), m_knownObjectIds.end(), objData.id) == m_knownObjectIds.end())
        {
            m_knownObjectIds.push_back(objData.id);
        }

        m_realtimeObjects.push_back(objData);

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

    const std::vector<ObjectData>& Recorder::GetRealtimeObjects() const
    {
        return m_realtimeObjects;
    }

    const std::vector<std::string>& Recorder::GetKnownObjectIds() const
    {
        return m_knownObjectIds;
    }

    void Recorder::Clear()
    {
        LogManager::PrintOutput("Recorder::Clear() invoked. Cleared " + std::to_string(m_recordedFrames.size()) + " frames.");
        m_recordedFrames.clear();
        m_knownObjectIds.clear();
    }

    void Recorder::AddTargetId(const std::string& id)
    {
        if (!IsTarget(id))
        {
            LogManager::PrintOutput("Recorder::AddTargetId() added target: " + id);
            m_targetIds.push_back(id);
        }
    }

    void Recorder::RemoveTargetId(const std::string& id)
    {
        LogManager::PrintOutput("Recorder::RemoveTargetId() removed target: " + id);
        m_targetIds.erase(std::remove(m_targetIds.begin(), m_targetIds.end(), id), m_targetIds.end());
    }

    bool Recorder::IsTarget(const std::string& id) const
    {
        return std::find(m_targetIds.begin(), m_targetIds.end(), id) != m_targetIds.end();
    }

    void Recorder::ClearTargets()
    {
        LogManager::PrintOutput("Recorder::ClearTargets() invoked.");
        m_targetIds.clear();
    }
}
