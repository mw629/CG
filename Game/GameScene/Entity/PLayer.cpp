#include "Player.h"
#include <imgui.h>

Player::~Player()
{
}

void Player::ImGui()
{
	if (ImGui::CollapsingHeader("player")) {
		ImGui::DragFloat3("Pos", &transform_.translate.x, 0.1f);
		ImGui::DragFloat3("Size", &transform_.scale.x, 0.1f);
		ImGui::DragFloat3("Roteta", &transform_.rotate.x, 0.1f);
		ImGui::ColorEdit4("Color", &modelColor_.x);
	}
}

void Player::Initialize()
{

	speed_ = 2.0f / 60.0f;

	ModelData modelData = LoadObjFile("resources/Player", "Player.obj");

	model_ = std::make_unique<Model>();
	model_->Initialize(modelData);
	model_->CreateModel();
}

void Player::Update(Matrix4x4 viewMatrix)
{
	MoveInput();

	model_.get()->SetTransform(transform_);
	model_.get()->SettingWvp(viewMatrix);
}

void Player::Draw()
{
	Draw::DrawModel(model_.get());
}

void Player::MoveInput()
{
	if (Input::PressKey(DIK_A) || GamePadInput::GetLeftStickX() < 0) {
		transform_.translate.x -= speed_;
	}
	if (Input::PressKey(DIK_D) || GamePadInput::GetLeftStickX() > 0) {
		transform_.translate.x += speed_;
	}


}
