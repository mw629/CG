#pragma once

namespace HapiColi
{
    class PlaybackManager
    {
    public:
        PlaybackManager();
        ~PlaybackManager() = default;

        // Replay Mode
        bool IsReplayMode() const { return m_isReplayMode; }
        void SetReplayMode(bool replay) { m_isReplayMode = replay; }
        
        bool IsReplayPlaying() const { return m_isReplayPlaying; }
        void SetReplayPlaying(bool playing) { m_isReplayPlaying = playing; }

        int GetReplayFrameIndex() const { return m_replayFrameIndex; }
        void SetReplayFrameIndex(int index) { m_replayFrameIndex = index; }

        void Update(float deltaTime, int maxFrames);

        // Simulation Time Control
        bool IsSimulationPaused() const { return m_isSimulationPaused; }
        void SetSimulationPaused(bool paused) { m_isSimulationPaused = paused; }

        float GetTimeScale() const { return m_timeScale; }
        void SetTimeScale(float scale) { m_timeScale = scale; }

        void RequestStep() { m_stepRequested = true; }
        bool ConsumeStepRequest();

    private:
        bool m_isReplayMode;
        int m_replayFrameIndex;
        bool m_isReplayPlaying;
        float m_replayTimer;
        
        bool m_isSimulationPaused;
        float m_timeScale;
        bool m_stepRequested;
    };
}
