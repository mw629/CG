#include "Player.h"

#include "StageSettings.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Initialize()
{
}

void Player::Update(Matrix4x4 view)
{
	PlayerMove();


	model_.get()->SettingWvp(view);
}

void Player::PlayerMove()
{
	//レーンの移動中ではなかったら
	if (laneIndex_ == targetLaneIndex_) {
		//キー入力でレーン移動
		if (Input::PushKey(DIK_A)) { targetLaneIndex_ = laneIndex_ - 1; }
		if (Input::PushKey(DIK_D)) { targetLaneIndex_ = laneIndex_ + 1; }
	}
	//レーンの移動中だったら
	else {

		int maxLaneIndex = 1;
		int minLaneIndex = -1;

		//目標のレーン位置に向かって移動
		//レーンの範囲内だったら
		if (minLaneIndex < targetLaneIndex_ && targetLaneIndex_ < maxLaneIndex)
		{
			
		}


	}



}

void Player::Draw()
{
	Draw::DrawObj(model_.get());
}
