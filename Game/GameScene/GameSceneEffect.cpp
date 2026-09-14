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
}

void GameSceneEffect::Initialize()
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
	/* ほかのパーティクルは一旦無効化
	hitEffect_->Update(view);
	
	shockwaveEffect_->Update(view, [](const EffectDefinitionData& p) {
		EffectDefinitionData next = p;
		// 衝撃波のように急速にスケールを拡大する
		next.transform.scale.x += 0.06f;
		next.transform.scale.y += 0.06f;
		next.transform.scale.z += 0.06f;
		return next;
	});

	// 砂埃のUpdate
	dustEffect_->Update(view);
	*/

	// 雪エフェクトの更新はAlwaysUpdateに移動
}

void GameSceneEffect::PlayerHitUpdate(const Matrix4x4& view)
{
	// hitEffect_->Update(view);
	// dustEffect_->Update(view);
	// snowEffect_->Update(view); // AlwaysUpdateに移動
}

void GameSceneEffect::EditorUpdate(const Matrix4x4& view)
{
	// hitEffect_->EditorUpdate(view);
	// dustEffect_->EditorUpdate(view);
	// shockwaveEffect_->EditorUpdate(view);
	// snowEffect_->EditorUpdate(view); // AlwaysUpdateに移動
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

void GameSceneEffect::Draw(class Draw& draw)
{
	// hitEffect_->Draw(draw);
	// shockwaveEffect_->Draw(draw);
	// dustEffect_->Draw(draw);
	snowEffect_->Draw(draw);
}

void GameSceneEffect::ImGui()
{
#ifdef _USE_IMGUI
	if (ImGui::CollapsingHeader("Particles")) {
		// if (hitEffect_) hitEffect_->ImGui();
		// if (dustEffect_) dustEffect_->ImGui();
		// if (shockwaveEffect_) shockwaveEffect_->ImGui();
		if (snowEffect_) snowEffect_->ImGui();
	}
#endif
}
