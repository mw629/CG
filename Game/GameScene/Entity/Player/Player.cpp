#include "Player.h"
#include <imgui.h>
#include "../../Map/MapChipField.h"
#include "../../Map/MapStruct.h"

void Player::ImGui() {

	if (ImGui::CollapsingHeader("Player")) {
		if (ImGui::CollapsingHeader("Transform")) {
			ImGui::DragFloat3("Pos", &transform_.translate.x);
			ImGui::DragFloat3("Size", &transform_.scale.x);
			ImGui::DragFloat3("Rotate", &transform_.rotate.x);
		}
		if (ImGui::CollapsingHeader("Parameter")) {
			ImGui::DragFloat3("Velocity", &velocity_.x);//速度
			ImGui::DragFloat("Speed", &speed_, 0.0f, 2.0f);//速さ
			ImGui::DragFloat("WallFrictio", &wallFrictio, 0.0f, 1.0f);//壁との摩擦
		}
		if (ImGui::CollapsingHeader("Jump")) {
			ImGui::DragFloat("GravityAccleration", &kGravityAccleration);//重力
			ImGui::Checkbox("OnGround", &onGround_);//地面にいるフラグ
			ImGui::Checkbox("DoubleJump", &doubleJump);//ダブルジャンプフラグ
			ImGui::DragFloat("JumpAcceleration", &kJumpAcceleration);//ジャンプの初速度
			ImGui::Checkbox("WallKickFlag", &wallKick);//壁キックの可能フラグ
			ImGui::Checkbox("WallKickFlag", &isWallJumping);//壁キック中のフラグ
			ImGui::DragFloat("WallJumpAcceleration", &kWallJumpAcceleration);//壁キックの初速度
		}
	}
}

void Player::Initialize(const Vector3& position, Matrix4x4 viewMatrix) {
	ModelData modelData = LoadObjFile("resources/Player", "Player.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
}

void Player::Update(Matrix4x4 viewMatrix) {

	MoveInput();

	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.velocity = velocity_;
	// マップ衝突チェック
	MapCollisionDetection(collisionMapInfo);
	// ④天井に接触している時の処理
	touchingTheCeiling(collisionMapInfo);
	// ⑤壁に接触している場合の処理
	contactWithAWall(collisionMapInfo);
	// ⑥設置状態の切り替え
	SwitchingInstallationStatus(collisionMapInfo);

	if (transform_.translate.y < -2.0f) {
		isDead_ = true;
	}

	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
}

void Player::Draw() {
	Draw::DrawModel(model_.get());
}

void Player::MoveInput() {

	velocity_.x = 0;
	if (Input::PressKey(DIK_A)) {
		velocity_.x += speed_*-1.0f;
		lrDirection_ = LRDirection::kLeft;
	}
	if (Input::PressKey(DIK_D)) {
		velocity_.x += speed_;
		lrDirection_ = LRDirection::kRight;
	}



	if (Input::PushKey(DIK_W) || GamePadInput::PushButton(XINPUT_GAMEPAD_A)) {
		if (onGround_) {

			velocity_.y = kJumpAcceleration; // 上向きの速度

		}
		else {
			wallKick = false;
			if (wallKick) {
				velocity_.y = kWallJumpAcceleration; // 上向きの速度
				isWallJumping = true;
				if (lrDirection_ == LRDirection::kRight) { lrDirection_ = LRDirection::kLeft; }
				else { lrDirection_ = LRDirection::kRight; }
			}
			else if (!doubleJump) {
				velocity_.y = kJumpAcceleration; // 上向きの速度
				doubleJump = true;
				isWallJumping = false;
			}
		}
	}

	isShot_ = false;
	if (Input::PushKey(DIK_SPACE)) {
		isShot_ = true;
	}


	if (!onGround_) {
		// 落下速度
		velocity_ = velocity_ + Vector3(0.0f, -kGravityAccleration / 60.0f, 0.0f);
		// 落下速度制限
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::MapCollisionDetection(CollisionMapInfo& info) {
	// 衝突情報を初期化
	info.ceilingCollisionFlag = false;
	info.landingFlag = false;
	info.wallContactFlag = false;

	// X軸の移動量のみを考慮して当たり判定を行う
	CollisionMapInfo horizontalInfo{};
	horizontalInfo.velocity = Vector3(info.velocity.x, 0.0f, 0.0f);

	MapCollisionDetectionLeft(horizontalInfo);

	MapCollisionDetectionRight(horizontalInfo);

	// X軸の判定結果を反映
	transform_.translate.x += horizontalInfo.velocity.x;
	info.wallContactFlag = horizontalInfo.wallContactFlag;

	// Y軸の移動量のみを考慮して当たり判定を行う
	CollisionMapInfo verticalInfo{};
	
	verticalInfo.velocity = Vector3(0.0f, info.velocity.y, 0.0f);

	if (wallKick) {
		verticalInfo.velocity = Vector3(0.0f, info.velocity.y * wallFrictio, 0.0f);
	}

	MapCollisionDetectionUp(verticalInfo);
	MapCollisionDetectionDown(verticalInfo);

	// Y軸の判定結果を反映
	transform_.translate.y += verticalInfo.velocity.y;
	info.landingFlag = verticalInfo.landingFlag;
	info.ceilingCollisionFlag = verticalInfo.ceilingCollisionFlag;
}

void Player::MapCollisionDetectionUp(CollisionMapInfo& info) {
	const float kPenetrationBuffer = 0.01f; // ほんの少し余裕をもたせる
	// 上昇アリ？
	if (info.velocity.y <= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, 4> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(transform_.translate + info.velocity, static_cast<Corner>(i));
	}
	MapChipType mapChipType{};
	// 真上の当たり判定を行う
	bool hitLeft = false;
	bool hitRight = false;
	// 左上点の判定
	IndexSet indexSet{};
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hitLeft = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hitRight = true;
	}
	if (hitLeft) {
		// 天井のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		// 現在座標が壁の外か判定
		IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(positionsNew[kLeftTop].x, transform_.translate.y + kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// 天井の矩形情報を取得
			Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// Y方向の移動量を調整（めり込みを防ぐ）
			info.velocity.y = (std::max)(0.0f, rect.bottom - transform_.translate.y - kPenetrationBuffer - kHeight / 2.0f);
			// フラグを立てる
			info.ceilingCollisionFlag = true;
		}
	}
	if (hitRight) {
		// 天井のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		// 現在座標が壁の外か判定
		IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(positionsNew[kRightTop].x, transform_.translate.y + kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// 天井の矩形情報を取得
			Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// Y方向の移動量を調整（めり込みを防ぐ）
			info.velocity.y = (std::max)(0.0f, rect.bottom - transform_.translate.y - kPenetrationBuffer - kHeight / 2.0f);
			// フラグを立てる
			info.ceilingCollisionFlag = true;
		}
	}
}

void Player::MapCollisionDetectionDown(CollisionMapInfo& info) {
	const float kPenetrationBuffer = 0.01f; // ほんの少し余裕をもたせる
	// 下降あり？
	if (info.velocity.y >= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, 4> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(transform_.translate + info.velocity, static_cast<Corner>(i));
	}
	MapChipType mapChipType{};
	// 真下の当たり判定を行う
	bool hitLeft = false;
	bool hitRight = false;
	// 左上点の判定
	IndexSet indexSet{};
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hitLeft = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hitRight = true;
	}
	if (hitLeft) {
		// 地面のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		// 現在座標が壁の外か判定
		IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(positionsNew[kLeftBottom].x, transform_.translate.y - kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// 地面の矩形情報を取得
			Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// Y方向の移動量を調整（めり込みを防ぐ）
			info.velocity.y = (std::min)(0.0f, rect.top - transform_.translate.y + kPenetrationBuffer + kHeight / 2.0f);
			// フラグを立てる
			info.landingFlag = true;
		}
	}
	if (hitRight) {
		// 地面のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		// 現在座標が壁の外か判定
		IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(positionsNew[kRightBottom].x, transform_.translate.y - kHeight / 2.0f, 0));
		if (indexSetNow.yIndex != indexSet.yIndex) {
			// 地面の矩形情報を取得
			Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// Y方向の移動量を調整（めり込みを防ぐ）
			info.velocity.y = (std::min)(0.0f, rect.top - transform_.translate.y + kPenetrationBuffer + kHeight / 2.0f);
			// フラグを立てる
			info.landingFlag = true;
		}
	}
}

void Player::MapCollisionDetectionLeft(CollisionMapInfo& info) {
	const float kPenetrationBuffer = 0.01f; // ほんの少し余裕をもたせる
	// 壁あり？
	if (info.velocity.x >= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, 4> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(transform_.translate + info.velocity, static_cast<Corner>(i));
	}
	MapChipType mapChipType{};
	// 真左の当たり判定を行う
	bool hitTop = false;
	bool hitBottom = false;
	// 左上点の判定
	IndexSet indexSet{};
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hitTop = true;
	}
	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hitBottom = true;
	}
	if (hitTop) {
		// 壁のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		// 現在座標が壁の外か判定
		IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(transform_.translate.x - kWidth / 2.0f, positionsNew[kLeftTop].y, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// 壁の矩形情報を取得
			Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// X方向の移動量を調整（めり込みを防ぐ）
			info.velocity.x = (std::max)(0.0f, rect.right - transform_.translate.x + kPenetrationBuffer - kWidth / 2.0f);
			// フラグを立てる
			info.wallContactFlag = true;
		}
	}
	if (hitBottom) {
		// 壁のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		// 現在座標が壁の外か判定
		IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(transform_.translate.x - kWidth / 2.0f, positionsNew[kLeftBottom].y, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// 壁の矩形情報を取得
			Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// X方向の移動量を調整（めり込みを防ぐ）
			info.velocity.x = (std::max)(0.0f, rect.right - transform_.translate.x + kPenetrationBuffer - kWidth / 2.0f);
			// フラグを立てる
			info.wallContactFlag = true;
		}
	}
}

void Player::MapCollisionDetectionRight(CollisionMapInfo& info) {
	const float kPenetrationBuffer = 0.01f; // ほんの少し余裕をもたせる
	// 壁あり？
	if (info.velocity.x <= 0) {
		return;
	}
	// 移動後の４つの角の座標
	std::array<Vector3, 4> positionsNew{};
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(transform_.translate + info.velocity, static_cast<Corner>(i));
	}
	MapChipType mapChipType{};
	// 真右の当たり判定を行う
	bool hitTop = false;
	bool hitBottom = false;
	// 右上点の判定
	IndexSet indexSet{};
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hitTop = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hitBottom = true;
	}
	if (hitTop) {
		// 壁のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		// 現在座標が壁の外か判定
		IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(transform_.translate.x + kWidth / 2.0f, positionsNew[kRightTop].y, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// 壁の矩形情報を取得
			Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// X方向の移動量を調整（めり込みを防ぐ）
			info.velocity.x = (std::min)(0.0f, rect.left - transform_.translate.x + kPenetrationBuffer + kWidth / 2.0f);
			// フラグを立てる
			info.wallContactFlag = true;
		}
	}
	if (hitBottom) {
		// 壁のどのマスに当たったかを取得
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		// 現在座標が壁の外か判定
		IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(Vector3(transform_.translate.x + kWidth / 2.0f, positionsNew[kRightBottom].y, 0));
		if (indexSetNow.xIndex != indexSet.xIndex) {
			// 壁の矩形情報を取得
			Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// X方向の移動量を調整（めり込みを防ぐ）
			info.velocity.x = (std::min)(0.0f, rect.left - transform_.translate.x + kPenetrationBuffer + kWidth / 2.0f);
			// フラグを立てる
			info.wallContactFlag = true;
		}
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
		{+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
		{-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
		{+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
		{-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeftTop
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

void Player::touchingTheCeiling(const CollisionMapInfo& info) {
	if (info.ceilingCollisionFlag) {
		velocity_.y = 0;
	}
}

void Player::SwitchingInstallationStatus(const CollisionMapInfo& info) {
	if (onGround_) {
		doubleJump = false;
		isWallJumping = false;
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
		else {
			MapChipType mapChipType;
			// 真下の当たり判定を行う
			bool hitLeft = false;
			bool hitRight = false;
			// 移動後の４つの角の座標
			std::array<Vector3, 4> positionsNew{};
			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(transform_.translate + info.velocity - Vector3(0.0f, 0.1f, 0.0f), static_cast<Corner>(i));
			}
			// 左下点の判定
			IndexSet indexSet{};
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hitLeft = true;
			}
			// 右下点の判定
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hitRight = true;
			}
			if (!hitLeft && !hitRight) {
				// 空中状態に切り替える
				onGround_ = false;
			}
		}
	}
	else {
		// 自キャラが設置状態？
		// 接地判定
		if (info.landingFlag) {
			// 着地状態に切り替える（落下を止める）
			onGround_ = true;
			// Y速度をゼロにする
			velocity_.y = 0.0f;
		}
	}

}

void Player::contactWithAWall(const CollisionMapInfo& info) {
	// 壁接触による減速
	if (info.wallContactFlag) {
		if (onGround_) {
			if (lrDirection_ == LRDirection::kRight) {
				lrDirection_ = LRDirection::kLeft;
			}
			else
			{
				lrDirection_ = LRDirection::kRight;
			}
		}
		else {
			wallKick = true;
		}
	}
	else {
		wallKick = false;
	}
}


void Player::OnCollision(const Goal* goal)
{
	(goal);
}

void Player::OnCollision(const Coin* coin)
{
	(coin);
}

AABB Player::GetAABB()
{
	Vector3 worldPos = transform_.translate;
	AABB aabb;
	aabb.min = { worldPos.x - kWidth / 2.0f, worldPos.y - kHeight / 2.0f, worldPos.z - kWidth / 2.0f };
	aabb.max = { worldPos.x + kWidth / 2.0f, worldPos.y + kHeight / 2.0f, worldPos.z + kWidth / 2.0f };

	return aabb;
}

