#pragma once
#include <memory>
#include <Emitter.h>
#include <Engine.h>

class GameSceneEffect {
public:
	GameSceneEffect();
	~GameSceneEffect() = default;

	void Initialize();
	
	void PlayingUpdate(const Matrix4x4& view, const Vector3& playerPos);
	void PlayerHitUpdate(const Matrix4x4& view);
	void EditorUpdate(const Matrix4x4& view);
	void AlwaysUpdate(const Matrix4x4& view, const Vector3& cameraPos);

	void EmitDust(const Vector3& playerPos);
	void EmitShockwave(const Vector3& playerPos);
	void EmitHitEffect(const Vector3& playerPos);
	void ClearHitParticles();

	void Draw(class Draw& draw);
	void ImGui();

private:
	std::unique_ptr<Emitter> hitEffect_;
	std::unique_ptr<Emitter> dustEffect_;
	std::unique_ptr<Emitter> shockwaveEffect_;
	std::unique_ptr<Emitter> snowEffect_;
};
