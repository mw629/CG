#pragma once

#include "HapiColiManager/HapiColiManager.h"
#include "HapiColiManager/FrameData.h"
#include <memory>

namespace HapiColi
{
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
        
        // デバッグ描画用 (ビュープロジェクション行列を渡す)
        
        const char* GetActiveSubjectId() const;
        const char* GetActiveTargetId() const;
        HapiColiManager* GetManager() { return m_manager.get(); }

    private:
        HapiColi();
        ~HapiColi();
        
        std::unique_ptr<HapiColiManager> m_manager;
    };
}
