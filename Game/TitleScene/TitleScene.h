#pragma once
#include <Engine.h>
#include "../IScene.h"
#include "../SkyDome.h"

class TitleScene:public IScene
{
private:

	std::unique_ptr<Sprite> backImage_;
	std::unique_ptr<Sprite> SpaceSprite_ = std::make_unique<Sprite>();

	std::unique_ptr<Sprite>	se_ = std::make_unique<Sprite>();

	std::unique_ptr<SkyDome> skyDome_;

	// 決定効果音
	int decideSE_ = -1;

	// タイトル落下アニメーション用
	float titleCurrentY_ = -200.0f;   // 現在のY座標（画面外から開始）
	float titleTargetY_ = 100.0f;     // 目標のY座標（着地位置）
	float titleVelocityY_ = 0.0f;     // Y方向の速度
	float titleGravity_ = 0.8f;       // 重力加速度
	float titleBounceRate_ = 0.5f;    // 反発係数（0.0〜1.0）
	bool titleLanded_ = false;        // 着地完了フラグ

	// タイトル着地後の上下（ボビング）アニメーション用
	float titleBobTimer_ = 0.0f;      // ボビング用タイマー（ラジアン単位）
	float titleBobAmplitude_ = 8.0f;  // 振幅（ピクセル）
	float titleBobSpeed_ = 0.10f;     // 1フレームあたりの角速度（ラジアン）

public:

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

