#include "GameSceneEffect.h"
#include "Graphics/Render/Draw.h"
#include <cmath>
#include <cstdlib>
#include <imgui.h>

GameSceneEffect::GameSceneEffect()
{
	hitEffect_ = std::make_unique<Emitter>();
	dustEffect_ = std::make_unique<Emitter>();
	shockwaveEffect_ = std::make_unique<Emitter>();
	snowEffect_ = std::make_unique<Emitter>();
	barrier_ = std::make_unique<HexBarrier>();
}

void GameSceneEffect::Initialize()
{
	// 六角形バリアの初期化
	if (barrier_) {
		barrier_->Initialize("Resources/Texture/white64x64.png");
		barrier_->SetRadius(0.5f);
		barrier_->SetShape(HexBarrierShape::Honeycomb);
	}
	// ヒットエフェクトの初期化（ヒットスパーク演出）
	EmitterData hitEmitter;
	hitEmitter.transform.scale = { 0.0f, 0.0f, 0.0f }; // 中心の一点から発生させる
	hitEmitter.count = 40; // 粒を増やして派手にする
	hitEmitter.frequency = 9999.0f; // 自動発生させず、手動のEmitのみにする
	EffectDefinitionData hitData;
	hitData.color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白色
	hitData.lifeTime = 1.0f; // ライフ（余韻の長さ）を少し短く調整
	hitData.transform.scale = { 0.1f, 0.8f, 0.1f }; // エミッタ（パーティクル）のサイズを調整して見やすくする
	
	// テクスチャ指定なしなら自動的にcircle.pngが使われます
	hitEffect_->Initialize(hitEmitter, hitData, EffectShape::Plane);
	hitEffect_->SetBlend(BlendMode::kBlendModeAdd); // 黒い部分を透過させるために加算ブレンドに戻す
	hitEffect_->name_ = "Hit Effect";
	hitEffect_->generatorBehavior = [](EffectDefinitionData& p) {
		// 中心から円状に放射状に広がるためのランダムな方向
		float randX = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
		float randY = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
		
		// 位置を固定するために速度を0にする
		p.velocity = { 0.0f, 0.0f, 0.0f }; 
		
		// 長さに少しランダムなばらつきを持たせる
		p.transform.scale.y = 0.6f + ((float)rand() / RAND_MAX) * 0.8f; 

		// Y軸ベースの細長いPlaneを、放射状に向きを合わせる
		p.transform.rotate.z = std::atan2(randY, randX) - 3.14159f / 2.0f;
		
		// X, Yの回転を消して純粋にカメラに対してフラットな星型にする
		p.transform.rotate.x = 0.0f;
		p.transform.rotate.y = 0.0f;
	};

	dustEffect_->Initialize();
	dustEffect_->LoadFromJson("Dustparticle");
	dustEffect_->SetBlend(BlendMode::kBlendModeNormal); // アルファブレンドを強制
	dustEffect_->name_ = "Dust Effect";
	dustEffect_->generatorBehavior = nullptr; // JSONの設定に完全に従う
	dustEffect_->SetStop(true); // 自動発生を停止（スクリプトから手動でのみEmitする）

	// ボーナスヒット時のショックウェーブ（Ring）の初期化
	EmitterData shockwaveEmitter;
	shockwaveEmitter.transform.scale = { 0.1f, 0.1f, 0.1f };
	shockwaveEmitter.count = 1;
	shockwaveEmitter.frequency = 9999.0f; // 手動Emit
	EffectDefinitionData shockwaveData;
	shockwaveData.color = { 1.0f, 1.0f, 0.0f, 1.0f }; // 黄色（Yellow）
	shockwaveData.lifeTime = 0.5f; // スパッと消えるように短め
	shockwaveData.transform.scale = { 0.1f, 0.1f, 0.1f };
	shockwaveEffect_->Initialize(shockwaveEmitter, shockwaveData, EffectShape::Ring);
	shockwaveEffect_->SetBlend(BlendMode::kBlendModeNone); // 加算だと背景と同化して薄くなるため、通常ブレンドで濃く（クッキリ）表示させる
	shockwaveEffect_->name_ = "Bonus Shockwave";
	shockwaveEffect_->generatorBehavior = [](EffectDefinitionData& p) {
		p.velocity = { 0.0f, 0.0f, 0.0f }; // 発生時は移動なし
		p.transform.rotate.x = 3.14159265f / 2.0f; // 盾（縦）になっているリングを90度回転させて地面と平行（横）にする
	};

	// 雪エフェクト（GPUパーティクル）の初期化
	snowEffect_->Initialize();
	snowEffect_->LoadFromJson("snow");
	snowEffect_->SetUseGpuParticle(true); // 確実にGPUパーティクルを有効化
	snowEffect_->SetBlend(BlendMode::kBlendModeAdd); // 加算ブレンドに設定（確実に描画されるように）
	snowEffect_->name_ = "Snow Effect";
	snowEffect_->SetStop(false); // 常に降らせる
	snowEffect_->generatorBehavior = nullptr; // JSONの設定に従う
}

void GameSceneEffect::PlayingUpdate(const Matrix4x4& view, const Vector3& playerPos)
{
	UpdateBarrier(view, playerPos);
}

void GameSceneEffect::PlayerHitUpdate(const Matrix4x4& view)
{
	UpdateBarrier(view, lastPlayerPos_);
}

void GameSceneEffect::EditorUpdate(const Matrix4x4& view)
{
	UpdateBarrier(view, lastPlayerPos_);
}

void GameSceneEffect::AlwaysUpdate(const Matrix4x4& view, const Vector3& cameraPos)
{
	// 雪エフェクトの更新
	// カメラの周囲に常に雪が降るように追従させる
	EmitterData sd = snowEffect_->GetEmitterData();
	sd.transform.translate.x = cameraPos.x;
	sd.transform.translate.y = cameraPos.y + 15.0f; // カメラより少し上から降らせる
	sd.transform.translate.z = cameraPos.z + 20.0f; // カメラの少し前を中心に
	snowEffect_->SetEmitterData(sd);
	
	// Editorモード等でも常に更新されるようにする
	snowEffect_->Update(view);
}

void GameSceneEffect::EmitDust(const Vector3& playerPos)
{
	EmitterData ed = dustEffect_->GetEmitterData();
	ed.transform.translate = playerPos;
	ed.transform.translate.y += 0.5f; // さらに高く調整
	dustEffect_->SetEmitterData(ed);
	dustEffect_->Emit();
}

void GameSceneEffect::EmitShockwave(const Vector3& playerPos)
{
	EmitterData ringData = shockwaveEffect_->GetEmitterData();
	ringData.transform.translate = playerPos;
	ringData.transform.translate.y -= 0.4f; // 足元より少し上（腰から足の間くらい）に設定
	shockwaveEffect_->SetEmitterData(ringData);
	shockwaveEffect_->Emit();
}

void GameSceneEffect::EmitHitEffect(const Vector3& playerPos)
{
	EmitterData emData = hitEffect_->GetEmitterData();
	emData.transform.translate = playerPos;
	emData.transform.translate.y += 4.0f; // プレイヤーの体より上に発生させる
	emData.count = 10; // 数を半分にする
	hitEffect_->SetEmitterData(emData);
	hitEffect_->Emit();
}

void GameSceneEffect::ClearHitParticles()
{
	hitEffect_->ClearParticles();
}

void GameSceneEffect::EmitBarrier(const Vector3& playerPos)
{
	barrierState_ = BarrierEffectState::Deploying;
	barrierDeployTimer_ = 0.0f;
	barrierPulseTimer_ = 0.0f;
	lastPlayerPos_ = playerPos;
}

void GameSceneEffect::BreakBarrier(const Vector3& playerPos)
{
	barrierState_ = BarrierEffectState::Breaking;
	barrierBreakTimer_ = 0.0f;
	lastPlayerPos_ = playerPos;
}

void GameSceneEffect::ClearBarrier()
{
	barrierState_ = BarrierEffectState::Inactive;
}

bool GameSceneEffect::IsBarrierActive() const
{
	return barrierState_ != BarrierEffectState::Inactive;
}

void GameSceneEffect::UpdateBarrier(const Matrix4x4& view, const Vector3& playerPos, float deltaTime)
{
	if (!barrier_ || barrierState_ == BarrierEffectState::Inactive) {
		return;
	}

	lastPlayerPos_ = playerPos;

	Vector3 currentScale = barrierBaseScale_;
	Vector4 currentColor = barrierBaseColor_;

	if (barrierState_ == BarrierEffectState::Deploying) {
		barrierDeployTimer_ += deltaTime;
		float t = std::clamp(barrierDeployTimer_ / barrierDeployDuration_, 0.0f, 1.0f);
		// EaseOutBack風の弾む展開アニメーション
		float s = std::sin(t * 1.5707963f);
		float overshoot = 1.0f + 0.25f * std::sin(t * 3.14159265f);
		float scaleFactor = s * overshoot;
		currentScale.x *= scaleFactor;
		currentScale.y *= scaleFactor;
		currentScale.z *= scaleFactor;

		// 展開時は白フラッシュからベースカラーへ移行
		currentColor.x = barrierFlashColor_.x * (1.0f - t) + barrierBaseColor_.x * t;
		currentColor.y = barrierFlashColor_.y * (1.0f - t) + barrierBaseColor_.y * t;
		currentColor.z = barrierFlashColor_.z * (1.0f - t) + barrierBaseColor_.z * t;
		currentColor.w = barrierBaseColor_.w;

		if (barrierDeployTimer_ >= barrierDeployDuration_) {
			barrierState_ = BarrierEffectState::Active;
		}
	}
	else if (barrierState_ == BarrierEffectState::Active) {
		barrierPulseTimer_ += deltaTime;
		// 呼吸のようなパルス（拡縮と発光明滅）
		float pulse = 1.0f + 0.035f * std::sin(barrierPulseTimer_ * 4.0f);
		float bright = 0.85f + 0.15f * std::sin(barrierPulseTimer_ * 4.0f);

		currentScale.x *= pulse;
		currentScale.y *= pulse;
		currentScale.z *= pulse;

		currentColor.x = barrierBaseColor_.x * bright;
		currentColor.y = barrierBaseColor_.y * bright;
		currentColor.z = barrierBaseColor_.z * bright;
		currentColor.w = barrierBaseColor_.w;
	}
	else if (barrierState_ == BarrierEffectState::Breaking) {
		barrierBreakTimer_ += deltaTime;
		float t = std::clamp(barrierBreakTimer_ / barrierBreakDuration_, 0.0f, 1.0f);

		// 急速に拡大しながらフェードアウト
		float expand = 1.0f + 0.9f * t;
		currentScale.x *= expand;
		currentScale.y *= expand;
		currentScale.z *= expand;

		// 最初の一瞬はフラッシュ、その後減衰
		float fade = (1.0f - t > 0.0f) ? (1.0f - t) : 0.0f;
		if (t < 0.15f) {
			currentColor = barrierFlashColor_;
		} else {
			currentColor.x = barrierBaseColor_.x * fade;
			currentColor.y = barrierBaseColor_.y * fade;
			currentColor.z = barrierBaseColor_.z * fade;
			currentColor.w = barrierBaseColor_.w * fade;
		}

		if (barrierBreakTimer_ >= barrierBreakDuration_) {
			barrierState_ = BarrierEffectState::Inactive;
		}
	}

	// マテリアルカラーを更新
	if (auto mat = barrier_->GetComponent<MaterialComponent>()) {
		if (auto factory = mat->GetMaterialFactory()) {
			factory->SetColor(currentColor);
		}
	}

	// プレイヤーの前方に配置
	Transform barrierTransform;
	barrierTransform.scale = currentScale;
	barrierTransform.rotate = { 0.0f, 0.0f, 0.0f };
	barrierTransform.translate = {
		playerPos.x + barrierOffset_.x,
		playerPos.y + barrierOffset_.y,
		playerPos.z + barrierOffset_.z
	};

	barrier_->SetTransform(barrierTransform);
	barrier_->SettingWvp(view);
}

void GameSceneEffect::Draw(class Draw& draw)
{
	// hitEffect_->Draw(draw);
	// shockwaveEffect_->Draw(draw);
	// dustEffect_->Draw(draw);
	snowEffect_->Draw(draw);

	// バリア描画
	if (barrier_ && barrierState_ != BarrierEffectState::Inactive) {
		draw.DrawObj(barrier_.get());
	}
}

void GameSceneEffect::ImGui()
{
#ifdef _USE_IMGUI
	if (ImGui::CollapsingHeader("Barrier Effect")) {
		const char* stateNames[] = { "Inactive", "Deploying", "Active", "Breaking" };
		ImGui::Text("Current State: %s", stateNames[static_cast<int>(barrierState_)]);

		if (ImGui::Button("Emit Barrier")) {
			EmitBarrier(lastPlayerPos_);
		}
		ImGui::SameLine();
		if (ImGui::Button("Break Barrier")) {
			BreakBarrier(lastPlayerPos_);
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear Barrier")) {
			ClearBarrier();
		}

		ImGui::Separator();
		ImGui::DragFloat3("Barrier Offset", &barrierOffset_.x, 0.05f);
		ImGui::DragFloat3("Barrier Scale", &barrierBaseScale_.x, 0.05f, 0.1f, 10.0f);
		ImGui::ColorEdit4("Barrier Color", &barrierBaseColor_.x);
		ImGui::ColorEdit4("Flash Color", &barrierFlashColor_.x);

		if (barrier_) {
			float r = barrier_->GetRadius();
			if (ImGui::DragFloat("Hex Radius", &r, 0.02f, 0.1f, 5.0f)) {
				barrier_->SetRadius(r);
			}

			bool pointy = barrier_->GetPointyTopped();
			if (ImGui::Checkbox("Pointy Topped", &pointy)) {
				barrier_->SetPointyTopped(pointy);
			}

			int shapeType = static_cast<int>(barrier_->GetShape());
			const char* shapes[] = { "Single", "Honeycomb (7-cell)" };
			if (ImGui::Combo("Shape", &shapeType, shapes, IM_ARRAYSIZE(shapes))) {
				barrier_->SetShape(static_cast<HexBarrierShape>(shapeType));
			}

			if (auto mat = barrier_->GetComponent<MaterialComponent>()) {
				int blendMode = static_cast<int>(mat->GetBlend());
				const char* blendNames[] = { "None", "Normal", "Add", "Subtract", "Multiply", "Screen" };
				if (ImGui::Combo("Blend Mode", &blendMode, blendNames, IM_ARRAYSIZE(blendNames))) {
					mat->SetBlend(static_cast<BlendMode>(blendMode));
				}
			}
		}
	}

	if (ImGui::CollapsingHeader("Particles")) {
		// if (hitEffect_) hitEffect_->ImGui();
		// if (dustEffect_) dustEffect_->ImGui();
		// if (shockwaveEffect_) shockwaveEffect_->ImGui();
		if (snowEffect_) snowEffect_->ImGui();
	}
#endif
}
