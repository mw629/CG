#pragma once
#include <memory>
#include <Emitter.h>
#include <Engine.h>

class GameSceneParticle {
public:
	GameSceneParticle();
	~GameSceneParticle() = default;

	void Initialize();
	
	void PlayingUpdate(const Matrix4x4& view, const Vector3& playerPos);
	void PlayerHitUpdate(const Matrix4x4& view);
	void EditorUpdate(const Matrix4x4& view);

	void EmitDust(const Vector3& playerPos);
	void EmitShockwave(const Vector3& playerPos);
	void EmitBonusCylinder(const Vector3& playerPos);
	void EmitHitEffect(const Vector3& playerPos);
	void ClearHitParticles();

	void StartBonusEffect(float duration);
	void UpdateBonusEffectEmit(float timeScale, const Vector3& playerPos);

	void Draw();
	void ImGui();

private:
	std::unique_ptr<Emitter> hitEffect_;
	std::unique_ptr<Emitter> dustEffect_;
	std::unique_ptr<Emitter> shockwaveEffect_;
	std::unique_ptr<Emitter> bonusCylinderEffect_;

	bool isBonusEffectActive_ = false;
	float bonusEffectTimer_ = 0.0f;
};
