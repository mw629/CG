#pragma once
#include <Engine.h>
#include "Camera.h"
#include "IScene.h"
#include "ParticleManager.h"
#include <memory>

class GameScene :public IScene
{
private:
	std::unique_ptr<Texture> texture_ = std::make_unique<Texture>();

	std::unique_ptr<Camera>camera_ = std::make_unique<Camera>();
	Transform cameraTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,2.0f,-5.0f} };

	std::vector<std::unique_ptr<ParticleManager>> particle_;
	Emitter particleEmitter_;

	std::unique_ptr<Sprite> sprite_ = std::make_unique<Sprite>();
	SpriteData spriteData_{
		{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {400.0f, 100.0f, 0.0f} }, // transform
		{ 300, 300 }, // size
		{ {0.0f, 0.0f}, {0.1f, 0.1f} } // texxtureArea
	};

	std::unique_ptr<Model> model_ = std::make_unique<Model>();
	Transform modelTransform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,2.0f,1.5f} };

	std::unique_ptr<Sphere> sphere_ = std::make_unique<Sphere>();
	Transform Transform_{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,2.0f,1.5f} };

	std::unique_ptr<Model> floor = std::make_unique<Model>();
	Transform floorT{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f,},{0.0f,0.0f,0.0f} };

	std::unique_ptr<CharacterAnimator> animation_ = std::make_unique<CharacterAnimator>();
	std::unique_ptr<TransformAnimation> nodeanimation_ = std::make_unique<TransformAnimation>();

	bool bill = true;

public:
	~GameScene()override;

	void ImGui()override;

	void Initialize()override;

	void Update()override;

	void Draw()override;

};

