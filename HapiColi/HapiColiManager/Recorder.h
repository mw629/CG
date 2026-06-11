#pragma once

#include "FrameData.h"
#include <vector>
#include <chrono>

namespace HapiColi
{
    class Recorder
    {
    public:
        Recorder();

        void Start();
        void Stop();
        bool IsRecording() const;

        void BeginFrame(float deltaTime);
        void RecordObject(const ObjectData& objData);
        void EndFrame();

        const std::vector<FrameData>& GetRecordedFrames() const;
        void Clear();

        // Target filtering
        void AddTargetId(const std::string& id);
        void RemoveTargetId(const std::string& id);
        bool IsTarget(const std::string& id) const;
        void ClearTargets();

    private:
        bool m_isRecording;
        uint32_t m_currentFrameIndex;
        FrameData m_currentFrameData;
        std::vector<FrameData> m_recordedFrames;
        std::vector<std::string> m_targetIds;
    };
}
