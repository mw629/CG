#pragma once
#include <Engine.h>
#include "../IScene.h"
#include "../SkyDome.h"

class StageSelect :public IScene
{
private:

	std::unique_ptr<Sprite> sprite_[9];
	std::unique_ptr<Sprite> SpaceSprite_ = std::make_unique<Sprite>();

	std::unique_ptr<SkyDome> skyDome_;

	int stageNum_ = 0;

	// イージング用変数
	float displayOffset_ = 0.0f;      // 現在の表示オフセット
	float targetOffset_ = 0.0f;       // 目標オフセット
	float easingSpeed_ = 0.1f;        // イージング速度

	// 表示設定
	 int kMaxStage = 9;
	 int kVisibleCount = 3;
	 float kSpriteWidth = 256.0f;  // スプライトの幅
	 float kCenterX = 640.0f;      // 画面中央X

	// 決定効果音
	int decideSE_ = -1;

	// イージング関数
	float EaseOutCubic(float t);

public:

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

