#pragma once
#include "../IScene.h"
#include <Engine.h>
#include <memory>

class GameOverScene : public IScene
{
private:

    std::unique_ptr<Sprite> sprite_ = std::make_unique<Sprite>();
    std::unique_ptr<Sprite> SpaceSprite_ = std::make_unique<Sprite>();

public:

    ~GameOverScene() override;

    void ImGui() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
};

