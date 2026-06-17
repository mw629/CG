#include "PlaybackManager.h"

namespace HapiColi
{
    PlaybackManager::PlaybackManager()
        : m_isReplayMode(false)
        , m_replayFrameIndex(0)
        , m_isReplayPlaying(false)
        , m_replayTimer(0.0f)
        , m_isSimulationPaused(false)
        , m_timeScale(1.0f)
        , m_stepRequested(false)
    {
    }

    void PlaybackManager::Update(float deltaTime, int maxFrames)
    {
        if (m_isReplayMode && m_isReplayPlaying && maxFrames > 0)
        {
            m_replayTimer += deltaTime * m_timeScale;
            float frameDuration = 1.0f / 60.0f; // 60fps
            
            while (m_replayTimer >= frameDuration)
            {
                m_replayTimer -= frameDuration;
                m_replayFrameIndex++;
                if (m_replayFrameIndex >= maxFrames)
                {
                    m_replayFrameIndex = 0; // ループして最初に戻る
                }
            }
        }
    }

    bool PlaybackManager::ConsumeStepRequest()
    {
        if (m_stepRequested)
        {
            m_stepRequested = false;
            return true;
        }
        return false;
    }
}
