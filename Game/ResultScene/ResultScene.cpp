#include "ResultScene.h"
#include "Graphics/Render/Draw.h"
#include <imgui.h>

void ResultScene::ImGui()
{
#ifdef _USE_IMGUI
	ImGui::Text("RESULT");
#endif // _USE_IMGUI
}

void ResultScene::Initialize()
{
	sceneID_ = SceneID::Clear;

	camera_ = std::make_unique<Camera>();
	camera_->Update();

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	int backImageHandle = texture.get()->CreateTexture("Resources/Texture/Result.png");

	spriteData_.transform.scale = { 1.0f, 1.0f, 1.0f };
	spriteData_.transform.translate = { 640.0f, 360.0f, 0.0f };
	spriteData_.transform.rotate = { 0.0f, 0.0f, 0.0f };
	spriteData_.size = { 1280.0f, 720.0f };
	spriteData_.textureArea[0] = { 0.0f, 0.0f };
	spriteData_.textureArea[1] = { 1.0f, 1.0f };

	backImage_ = std::make_unique<Sprite>();
	backImage_->Initialize(spriteData_, backImageHandle);
	backImage_->GetMartial()->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	spritePos[0] = { 0.0f,0.0f };
	spritePos[1] = { 1280.0f, 720.0f };
}

void ResultScene::Update()
{
	camera_->Update();
	backImage_->Update(spriteData_);

	if (Input::PushKey(DIK_SPACE) || GamePadInput::PressButton(XINPUT_GAMEPAD_A)) {
		nextSceneID_ = SceneID::Title;
		sceneChangeRequest_ = true;
	}
}

void ResultScene::Draw(class Draw& draw)
{
	draw.SetCamera(camera_.get());
	draw.DrawSprite(backImage_.get());
}
