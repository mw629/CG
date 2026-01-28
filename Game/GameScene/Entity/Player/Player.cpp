#include "Player.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif

#include "../../Map/MapChipField.h"
#include "../../Map/MapStruct.h"
#include "../Enemy/Enemy.h"
#include <numbers>
#include <cmath>

void Player::ImGui() {
#ifdef USE_IMGUI

	if (ImGui::CollapsingHeader("Player")) {
		ImGui::DragInt("HP", &HP_);
		if (ImGui::CollapsingHeader("Transform")) {
			ImGui::DragFloat3("Pos", &transform_.translate.x);
			ImGui::DragFloat3("Size", &transform_.scale.x);
			ImGui::DragFloat3("Rotate", &transform_.rotate.x);
		}
		if (ImGui::CollapsingHeader("Parameter")) {
			ImGui::DragFloat3("Velocity", &velocity_.x);//速度
			ImGui::DragFloat("Speed", &speed_, 0.0f, 2.0f);//速さ
			ImGui::DragFloat("WallFrictio", &wallFrictio, 0.0f, 1.0f);//壁との摩擦
			// Debug info for run animation
			ImGui::Text("runTimer: %.3f", runAnimationTimer_);
			ImGui::Text("vel.x: %.3f", velocity_.x);
			ImGui::Text("isJump: %d", isJump_);
			ImGui::Text("isKnockback: %d", isKnockback_);
			ImGui::Text("onGround: %d", onGround_);
		}
		if (ImGui::CollapsingHeader("Jump")) {
			ImGui::DragFloat("GravityAccleration", &kGravityAccleration);//重力
			ImGui::Checkbox("OnGround", &onGround_);//地面にいるフラグ
			ImGui::Checkbox("DoubleJump", &doubleJump);//ダブルジャンプフラグ
			ImGui::DragFloat("JumpAcceleration", &kJumpAcceleration);//ジャンプの初速度

		}
		if (ImGui::CollapsingHeader("Death")) {
			ImGui::Checkbox("IsDead", &isDead_);
			ImGui::DragFloat("DeathTimer", &deathAnimationTimer_);
		}
		if (ImGui::CollapsingHeader("Clear")) {
			ImGui::Checkbox("IsClear", &isClear_);
			ImGui::DragFloat("ClearTimer", &clearAnimationTimer_);
			ImGui::Text("Phase: %d", static_cast<int>(clearPhase_));
		}
	}
#endif
}

void Player::Initialize(const Vector3& position, Matrix4x4 viewMatrix) {
	ModelData modelData = LoadObjFile("resources/Player", "Player.obj");
	model_ = std::make_unique<Model>();
	transform_.translate = position;
	model_->Initialize(modelData);
	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
	model_.get()->SetBlend(kBlendModeNormal);
	// キャラクターの当たり判定サイズ
	float kWidth = 0.9f;
	float kHeight = 0.9f;

	jumpSE_ = Audio::Load("resources/Audio/SE/Jump.mp3");
	damageSE_ = Audio::Load("resources/Audio/SE/damage.mp3");

}

void Player::Update(Matrix4x4 viewMatrix) {

	// 死亡演出中は通常の更新処理をスキップ
	if (isDead_) {
		DeathAnimation();
		model_->SetTransform(transform_);
		model_->SettingWvp(viewMatrix);
		return;
	}

	// クリア演出中
	if (isClear_) {
		// クリア演出中は無敵の点滅を無効化し、通常色に戻す
		model_->GetMatrial()->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		ClearAnimation();
		model_->SetTransform(transform_);
		model_->SettingWvp(viewMatrix);
		return;
	}

	// 射撃フラグを毎フレームリセット（ノックバック中も含めて）
	isShot_ = false;

	// ノックバック中の処理
	if (isKnockback_) {
		knockbackTimer_ += 1.0f / 60.0f;

		// ノックバック時間が終了したら通常状態に戻る
		if (knockbackTimer_ >= kKnockbackDuration) {
			isKnockback_ = false;
			knockbackTimer_ = 0.0f;
			knockbackVelocity_ = { 0.0f, 0.0f, 0.0f };
			// 通常の色に戻す
			model_->GetMatrial()->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		}
		else {
			// ノックバック速度を減衰させながら適用
			float decayRate = 1.0f - (knockbackTimer_ / kKnockbackDuration);
			velocity_.x = knockbackVelocity_.x * decayRate;
			velocity_.y = knockbackVelocity_.y * decayRate;

			// ノックバック時の点滅効果（赤色）
			float blinkRate = (std::sin)(knockbackTimer_ * 30.0f) * 0.5f + 0.5f;
			model_->GetMatrial()->SetColor({ 1.0f, blinkRate * 0.3f, blinkRate * 0.3f, 1.0f });
		}
	}
	else {
		MoveInput();
		// 無敵時間中の点滅
		if (invincibleFream > 0) {
			float blinkRate = (invincibleFream % 10 < 5) ? 0.5f : 1.0f;
			model_->GetMatrial()->SetColor({ 1.0f, 1.0f, 1.0f, blinkRate });
		}
		else {
			// 通常時の色
			model_->GetMatrial()->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		}
	}

	invincibleFream--;

	MapCollision();

	// 走っているときのスケールアニメーション（先に適用し、ジャンプで上書き）
	// 待機アニメーションを更新
	StandbyAnimation();
	RunAnimation();
	JumpAnimation();

	if (turnTimer_ > 0.0f) {
		// 旋回タイマーを1/60秒だけカウントダウンする
		turnTimer_ -= 1.0f / 60.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = { 0.0f,std::numbers::pi_v<float> };
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		float t = 1.0f - (turnTimer_ / currentTurnDuration_);
		transform_.rotate.y = Lerp(turnFirstRotationY_, destinationRotationY, t);
	}

	model_->SetTransform(transform_);
	model_->SettingWvp(viewMatrix);
}

void Player::Draw() {
	Draw::DrawModel(model_.get());
}

void Player::MoveInput() {

	velocity_.x = 0;
	if (Input::PressKey(DIK_A) || Input::PressKey(DIK_D)) {
		if (Input::PressKey(DIK_A)) {
			velocity_.x += speed_ * -1.0f;
			lrDirection_ = LRDirection::kLeft;
		}
		if (Input::PressKey(DIK_D)) {
			velocity_.x += speed_;
			lrDirection_ = LRDirection::kRight;
		}
		// 回転開始処理
		if (standbyRotateActive_) {
			currentTurnDuration_ = kTimeTurnQuick;
		}
		else {
			currentTurnDuration_ = kTimeTurn;
		}
		turnTimer_ = currentTurnDuration_;
		turnFirstRotationY_ = transform_.rotate.y;
	}

	// コヨーテタイムの更新
	if (onGround_) {
		coyoteTime_ = kCoyoteTimeDuration; // 地面にいる間はリセット
	}
	else {
		coyoteTime_ -= 1.0f / 60.0f; // 空中では減らす
	}

	if (Input::PushKey(DIK_W) || GamePadInput::PushButton(XINPUT_GAMEPAD_A)) {
		// 地面にいるか、コヨーテタイム内ならジャンプ可能
		if (onGround_ || coyoteTime_ > 0.0f) {
			velocity_.y = kJumpAcceleration; // 上向きの速度
			isJump_ = true;
			coyoteTime_ = 0.0f; // ジャンプしたらコヨーテタイムを消費
			// ジャンプ効果音を再生
			if (jumpSE_ >= 0) {
				Audio::Play(jumpSE_, false);
			}
		}
		else {
			if (!doubleJump) {
				velocity_.y = kJumpAcceleration; // 上向きの速度
				doubleJump = true;
				isJump_ = true;
				// ダブルジャンプ時も効果音を再生
				if (jumpSE_ >= 0) {
					Audio::Play(jumpSE_, false);
				}
			}
		}
	}

	isShot_ = false;
	if (Input::PushKey(DIK_SPACE) || GamePadInput::PushButton(XINPUT_GAMEPAD_RIGHT_SHOULDER)) {
		isShot_ = true;
	}


	if (!onGround_) {
		// 落下速度
		velocity_ = velocity_ + Vector3(0.0f, -kGravityAccleration / 60.0f, 0.0f);
		// 落下速度制限
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}
}


void Player::StandbyAnimation()
{
	//何もしていない時の処理
	if (standbyRotateCooldown_ > 0) {
		standbyRotateCooldown_--;
	}

	if (turnTimer_ > 0.0f) {
		if (standbyRotateActive_) {
			standbyRotateActive_ = false;
			standbyRotatePhase_ = 0;
			standbyRotateTimer_ = 0.0f;
		}
		if (0 == velocity_.x + velocity_.y + velocity_.z) {
			standbyFream_++;
		}
		else {
			standbyFream_ = 0;
		}
		if (standbyAnimationCoolTime_ > standbyFream_) {
			standbyAnimationFream_++;
		}
		return;
	}

	if (0 == velocity_.x + velocity_.y + velocity_.z) {
		standbyFream_++;
	}
	else {
		standbyFream_ = 0;
	}
	if (standbyAnimationCoolTime_ > standbyFream_) {
		standbyAnimationFream_++;
	}

	if (standbyFream_ >= standbyAnimationCoolTime_ && !standbyRotateActive_ && standbyRotateCooldown_ <= 0 && !isJump_ && !isKnockback_ && !isDead_ && !isClear_) {
		standbyRotateActive_ = true;
		standbyRotatePhase_ = 1; 
		standbyRotateTimer_ = 0.0f;
		if (lrDirection_ == LRDirection::kRight) {
			standbyRotateStartY_ = 0.0f;
			standbyRotateTargetY_ = std::numbers::pi_v<float> / 2.0f;
		}
		else {
			standbyRotateStartY_ = std::numbers::pi_v<float>;
			standbyRotateTargetY_ = std::numbers::pi_v<float> / 2.0f;
		}
		transform_.rotate.y = standbyRotateStartY_;
	}

	if (standbyRotateActive_) {
		const float delta = 1.0f / 60.0f;
		standbyRotateTimer_ += delta;

		const int totalPhases = kStandbyRotateRepeat * 2;

		const float duration = (standbyRotateDuration_ > 0.0f) ? standbyRotateDuration_ : 0.01f;
		float t = (std::min)(standbyRotateTimer_ / duration, 1.0f);

		// stronger ease-out (quintic): faster start, much slower end
		// t in [0,1] -> ease-out: 1 - (1 - t)^5
		t = 1.0f - std::pow(1.0f - t, 5.0f);

		float a0, a1;
		bool isGoPhase = (standbyRotatePhase_ % 2 == 1);
		if (isGoPhase) {
			a0 = standbyRotateStartY_;
			a1 = standbyRotateTargetY_;
		}
		else {
			a0 = standbyRotateTargetY_;
			a1 = standbyRotateStartY_;
		}

		transform_.rotate.y = Lerp(a0, a1, t);

		if (standbyRotateTimer_ >= duration) {
			standbyRotatePhase_++;
			standbyRotateTimer_ = 0.0f;

			if (standbyRotatePhase_ > totalPhases) {
				standbyRotateActive_ = false;
				standbyRotatePhase_ = 0;
				standbyRotateTimer_ = 0.0f;
				standbyAnimationFream_ = 0;
				standbyRotateCooldown_ = kStandbyRotateCooldownFrames;
			}
		}
	}

}

void Player::RunAnimation()
{
	if (isJump_ || isKnockback_ || isDead_ || isClear_) {
		transform_.scale.x = Lerp(transform_.scale.x, 1.0f, 0.25f);
		transform_.scale.y = Lerp(transform_.scale.y, 1.0f, 0.25f);
		transform_.scale.z = Lerp(transform_.scale.z, 1.0f, 0.25f);
		runAnimationTimer_ = 0.0f;
		return;
	}

	const float moveThreshold = 0.001f;
	bool isMoving = std::abs(velocity_.x) > moveThreshold;

	if (!isMoving) {
		if (Input::PressKey(DIK_A) || Input::PressKey(DIK_D)) {
			isMoving = true;
		}
		else {
			float lx = GamePadInput::GetLeftStickX();
			if (std::abs(lx) > 0.2f) {
				isMoving = true;
			}
		}
	}

	if (isMoving) {
		runAnimationTimer_ += 1.0f / 60.0f;

		float bob = std::sin(runAnimationTimer_ * 12.0f) * 0.15f;

		float squash = 1.0f - bob * 0.6f;

		float targetY = (std::max)(0.5f, 1.0f + bob);
		float targetX = (std::max)(0.6f, squash);
		float targetZ = 1.0f;

		transform_.scale.y = Lerp(transform_.scale.y, targetY, 0.45f);
		transform_.scale.x = Lerp(transform_.scale.x, targetX, 0.45f);
		transform_.scale.z = Lerp(transform_.scale.z, targetZ, 0.35f);
	}
	else {
		transform_.scale.x = Lerp(transform_.scale.x, 1.0f, 0.25f);
		transform_.scale.y = Lerp(transform_.scale.y, 1.0f, 0.25f);
		transform_.scale.z = Lerp(transform_.scale.z, 1.0f, 0.25f);
		runAnimationTimer_ = 0.0f;
	}
}

void Player::JumpAnimation()
{
	if (!isJump_) {
		animationFream_ = 0;
		return;
	}
	else {
		animationFream_++;
	}

	// 縮む・伸びるアニメーション時間
	const int shrinkTime = 5;   // 縮むフレーム
	const int stretchTime = 10; // 伸びるフレーム
	const int totalTime = shrinkTime + stretchTime + 10; // 空中→戻り時間も含めて調整

	const float minY = 0.5f;
	const float maxY = 1.1f;
	const float maxX = 1.1f;
	const float minX = 0.5f;


	if (animationFream_ < shrinkTime) {
		float t = (float)animationFream_ / shrinkTime;
		transform_.scale.y = Lerp(1.0f, minY, t); // 縮む
		transform_.scale.x = Lerp(maxX, maxX, t); // 横に広がる
	}
	else if (animationFream_ < shrinkTime + stretchTime) {
		float t = (float)(animationFream_ - shrinkTime) / stretchTime;
		transform_.scale.y = Lerp(minY, maxY, t); // 伸びる
		transform_.scale.x = Lerp(maxX, minX, t); // 横が細くなる
	}
	else {
		// 空中で少しずつ元に戻る
		transform_.scale.y = Lerp(transform_.scale.y, 1.0f, 0.15f);
		transform_.scale.x = Lerp(transform_.scale.x, 1.0f, 0.15f);
	}

	if (animationFream_ >= totalTime) {
		isJump_ = false;
		animationFream_ = 0;
		transform_.scale = { 1.0f, 1.0f, 1.0f };
	}
}

void Player::DeathAnimation()
{
	// タイマーを進める
	deathAnimationTimer_ += 1.0f / 60.0f;

	// 正規化された時間 (0.0 ~ 1.0)
	float t = deathAnimationTimer_ / kDeathAnimationDuration;
	t = (std::min)(t, 1.0f);

	// 死亡時の色変化（白→黒にフェード、透明度も下げる）
// 	float colorValue = Lerp(1.0f, 0.0f, t);
// 	float alpha = Lerp(1.0f, 0.0f, t);
// 	model_->GetMatrial()->SetColor({ colorValue, colorValue, colorValue, alpha });

	// 演出パターン1: 縮みながら回転して消える
	//transform_.scale = Vector3(
	//	Lerp(1.0f, 0.0f, t),
	//	Lerp(1.0f, 0.0f, t),
	//	Lerp(1.0f, 0.0f, t)
	//);
	//transform_.rotate.y += 0.1f; // 回転	

	// 演出パターン2: 倒れる演出（X軸回転）
	transform_.rotate.x = Lerp(0.0f, std::numbers::pi_v<float> / 2.0f, t);

	// 演出パターン3: 下に沈む演出
	// transform_.translate.y -= 0.02f;
}

void Player::ClearAnimation()
{
	const float deltaTime = 1.0f / 60.0f;

	switch (clearPhase_) {
	case ClearPhase::kWaitLanding:
		// 空中にいる場合は重力を適用して落下させる
		if (!onGround_) {
			velocity_.x = 0.0f; // 横移動を止める
			velocity_ = velocity_ + Vector3(0.0f, -kGravityAccleration / 60.0f, 0.0f);
			velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
			MapCollision();
		}
		else {
			// 着地したら次のフェーズへ
			velocity_ = { 0.0f, 0.0f, 0.0f };
			clearPhase_ = ClearPhase::kTurnToCamera;
			clearAnimationTimer_ = 0.0f;
			clearTurnStartRotationY_ = transform_.rotate.y;
		}
		break;

	case ClearPhase::kTurnToCamera:
		// 画面の方向（Z軸正方向＝カメラ側）を向く
		clearAnimationTimer_ += deltaTime;
		{
			float t = clearAnimationTimer_ / kTurnToCameraDuration;
			t = (std::min)(t, 1.0f);
			// 画面向き（Y回転 = π/2 で手前を向く）
			const float targetRotationY = std::numbers::pi_v<float> / 2.0f;
			transform_.rotate.y = Lerp(clearTurnStartRotationY_, targetRotationY, t);

			if (t >= 1.0f) {
				clearPhase_ = ClearPhase::kBackflip;
				clearAnimationTimer_ = 0.0f;
				transform_.rotate.x = 0.0f;
			}
		}
		break;

	case ClearPhase::kBackflip:
		// バク転アニメーション（後方回転 + ジャンプ）
		clearAnimationTimer_ += deltaTime;
		{
			float t = clearAnimationTimer_ / kBackflipDuration;
			t = (std::min)(t, 1.0f);

			// バク転は後ろ向きに回転（X軸で-2π回転）
			transform_.rotate.x = Lerp(0.0f, -std::numbers::pi_v<float> *2.0f, t);

			// ジャンプの高さ（放物線）
			// sin(π * t) で 0→1→0 の動きを作る
			float jumpHeight = std::sin(std::numbers::pi_v<float> *t) * 1.5f;

			// 初回のみ基準位置を保存
			static float baseY = 0.0f;
			if (clearAnimationTimer_ <= deltaTime) {
				baseY = transform_.translate.y;
			}
			transform_.translate.y = baseY + jumpHeight;

			if (t >= 1.0f) {
				clearPhase_ = ClearPhase::kFinished;
				transform_.rotate.x = 0.0f;
				transform_.translate.y = baseY;
			}
		}
		break;

	case ClearPhase::kFinished:
		// 演出完了
		break;
	}
}


void Player::HitWall()
{
}

void Player::OnGround()
{
	doubleJump = false;
}

void Player::OnCollision(const Goal* goal)
{
	(goal);
	// ゴールに触れたらクリア状態にする
	if (!isClear_) {
		SetClear(true);
	}
}

void Player::OnCollision(const Enemy* enemy)
{
	// クリア演出中は敵との当たり判定によるダメージを無効化
	if (isClear_) {
		return;
	}

	if (invincibleFream < 0) {
		HP_--;
		invincibleFream = invincibleTime;

		// ダメージ効果音を再生
		if (damageSE_ >= 0) {
			Audio::Play(damageSE_, false);
		}

		// ノックバック処理
		isKnockback_ = true;
		knockbackTimer_ = 0.0f;

		// 敵の位置に応じてノックバック方向を決定
		Vector3 enemyPos = enemy->GetTransform().translate;
		Vector3 playerPos = transform_.translate;

		// プレイヤーが敵の右側にいるか左側にいるかで方向を決定
		if (playerPos.x > enemyPos.x) {
			// 右にノックバック
			knockbackVelocity_.x = kKnockbackForceX;
		}
		else {
			// 左にノックバック
			knockbackVelocity_.x = -kKnockbackForceX;
		}

		// 上方向にも少し飛ばす
		knockbackVelocity_.y = kKnockbackForceY;
	}
	if (HP_ == 0) {
		isDead_ = true;
		deathAnimationTimer_ = 0.0f; // タイマー初期化
	}
}

