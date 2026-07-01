#include "GameSceneParticle.h"
#include <cmath>
#include <cstdlib>
#include <imgui.h>

GameSceneParticle::GameSceneParticle()
{
	hitEffect_ = std::make_unique<Emitter>();
	dustEffect_ = std::make_unique<Emitter>();
	shockwaveEffect_ = std::make_unique<Emitter>();
	bonusCylinderEffect_ = std::make_unique<Emitter>();
}

void GameSceneParticle::Initialize()
{
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

	// ボーナスヒット時のシリンダーエフェクトの初期化
	bonusCylinderEffect_->Initialize();
	bonusCylinderEffect_->LoadFromJson("BonusCylinder");
	bonusCylinderEffect_->name_ = "Bonus Cylinder";
	bonusCylinderEffect_->SetStop(true);
	bonusCylinderEffect_->generatorBehavior = [](EffectDefinitionData& p) {
		p.velocity = { 0.0f, 0.0f, 0.0f }; // 発生時は移動なし
		p.transform.rotate.x = 0.0f; 
	};
}

void GameSceneParticle::PlayingUpdate(const Matrix4x4& view, const Vector3& playerPos)
{
	hitEffect_->Update(view);
	
	shockwaveEffect_->Update(view, [](const EffectDefinitionData& p) {
		EffectDefinitionData next = p;
		// 衝撃波のように急速にスケールを拡大する
		next.transform.scale.x += 0.06f;
		next.transform.scale.y += 0.06f;
		next.transform.scale.z += 0.06f;
		return next;
	});

	// ボーナスシリンダーの更新（エディタと同じ挙動にするため、独自拡張を削除）
	bonusCylinderEffect_->Update(view);

	// 砂埃のUpdate
	dustEffect_->Update(view);
}

void GameSceneParticle::PlayerHitUpdate(const Matrix4x4& view)
{
	hitEffect_->Update(view);
	dustEffect_->Update(view);
}

void GameSceneParticle::EditorUpdate(const Matrix4x4& view)
{
	hitEffect_->EditorUpdate(view);
	dustEffect_->EditorUpdate(view);
	shockwaveEffect_->EditorUpdate(view);
	bonusCylinderEffect_->EditorUpdate(view);
}

void GameSceneParticle::EmitDust(const Vector3& playerPos)
{
	EmitterData ed = dustEffect_->GetEmitterData();
	ed.transform.translate = playerPos;
	ed.transform.translate.y += 0.5f; // さらに高く調整
	dustEffect_->SetEmitterData(ed);
	dustEffect_->Emit();
}

void GameSceneParticle::EmitShockwave(const Vector3& playerPos)
{
	EmitterData ringData = shockwaveEffect_->GetEmitterData();
	ringData.transform.translate = playerPos;
	ringData.transform.translate.y -= 0.4f; // 足元より少し上（腰から足の間くらい）に設定
	shockwaveEffect_->SetEmitterData(ringData);
	shockwaveEffect_->Emit();
}

void GameSceneParticle::EmitBonusCylinder(const Vector3& playerPos)
{
	EmitterData cylinderData = bonusCylinderEffect_->GetEmitterData();
	cylinderData.transform.translate = playerPos;
	// 足元（Y=2.0付近）を基準にするため少し下げる
	cylinderData.transform.translate.y -= 1.0f; 
	bonusCylinderEffect_->SetEmitterData(cylinderData);
	bonusCylinderEffect_->Emit();
}

void GameSceneParticle::EmitHitEffect(const Vector3& playerPos)
{
	EmitterData emData = hitEffect_->GetEmitterData();
	emData.transform.translate = playerPos;
	emData.transform.translate.y += 4.0f; // プレイヤーの体より上に発生させる
	emData.count = 10; // 数を半分にする
	hitEffect_->SetEmitterData(emData);
	hitEffect_->Emit();
}

void GameSceneParticle::ClearHitParticles()
{
	hitEffect_->ClearParticles();
}

void GameSceneParticle::StartBonusEffect(float duration)
{
	isBonusEffectActive_ = true;
	bonusEffectTimer_ = duration;
}

void GameSceneParticle::UpdateBonusEffectEmit(float timeScale, const Vector3& playerPos)
{
	if (isBonusEffectActive_) {
		bonusEffectTimer_ -= 1.0f * timeScale;
		if (bonusEffectTimer_ > 0.0f) {
			EmitterData cylinderData = bonusCylinderEffect_->GetEmitterData();
			cylinderData.transform.translate = playerPos;
			cylinderData.transform.translate.y -= 1.0f; 
			bonusCylinderEffect_->SetEmitterData(cylinderData);
			bonusCylinderEffect_->Emit();
		} else {
			isBonusEffectActive_ = false;
		}
	}
}

void GameSceneParticle::Draw()
{
	hitEffect_->Draw();
	shockwaveEffect_->Draw();
	bonusCylinderEffect_->Draw();
	dustEffect_->Draw();
}

void GameSceneParticle::ImGui()
{
#ifdef _USE_IMGUI
	if (ImGui::CollapsingHeader("Particles")) {
		if (hitEffect_) hitEffect_->ImGui();
		if (dustEffect_) dustEffect_->ImGui();
		if (shockwaveEffect_) shockwaveEffect_->ImGui();
		if (bonusCylinderEffect_) bonusCylinderEffect_->ImGui();
	}
#endif
}
