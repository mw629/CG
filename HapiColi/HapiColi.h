#pragma once

#include "HapiColiManager/HapiColiManager.h"
#include "HapiColiManager/FrameData.h"
#include <memory>
#include <vector>
#include <functional>
#include "HapiColiRenderer.h"
namespace HapiColi
{
    class HapiColiRenderer;
    class PlaybackManager;

    class HapiColi
    {
    public:
        static HapiColi& GetInstance()
        {
            static HapiColi instance;
            return instance;
        }

        void Initialize();
        void Update(); // Call every frame to render ImGui

        void BeginFrame(float deltaTime);
        void RecordObject(const ObjectData& objData);
        void EndFrame();
        
        void RegisterFuzzTarget(const std::string& name, const ObjectData& baseA, const ObjectData& baseB, std::function<void(ObjectData&, ObjectData&)> func);
        void UpdateFuzzTarget(const std::string& name, const ObjectData& baseA, const ObjectData& baseB);
        
        // 描画コマンド取得（DirectX非依存）
        void BuildRenderCommands();
        const std::vector<RenderCommand>& GetRenderCommands() const;
        const std::vector<SolidRenderCommand>& GetSolidRenderCommands() const;
        
        const char* GetActiveSubjectId() const;
        const char* GetActiveTargetId() const;
        HapiColiManager* GetManager() { return m_manager.get(); }
        PlaybackManager* GetPlaybackManager() const;

    private:
        HapiColi();
        ~HapiColi();
        
        std::unique_ptr<HapiColiManager> m_manager;
        std::unique_ptr<HapiColiRenderer> renderer_;
    };
}
