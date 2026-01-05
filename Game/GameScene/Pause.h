#pragma once
#include <Engine.h>
#include "System/GameState.h"

class Pause
{
private:
    bool isPause_ = true;
    bool isSelect_ = false;
    bool isRestart_ = false;

    MenuCommand menuCommand_ = kReturnToGame;

    std::unique_ptr<Sprite> Choices_[4];

    std::unique_ptr<Sprite> dimming_ = std::make_unique<Sprite>();
    Vector4 dimmingColor_ = { 0.0f, 0.0f, 0.0f, 0.7f };

    bool isInitialized_ = false;

public:
    void ImGui();
    void Initialize();
    void Update();
    void Draw();

    bool IsPause() { return isPause_; }
    bool IsSelect() { return isSelect_; }
    bool IsRestart() { return isRestart_; }
};

