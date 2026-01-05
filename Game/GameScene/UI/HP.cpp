#include "HP.h"

void HP::ImGui() {

}

void HP::Initialize() {
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	int textureHndle = texture.get()->CreateTexture("resources/UI/HP.png");
	Vector2 spritePos[2];
	for (int i = 0; i < 3; i++) {
		sprite_[i] = std::make_unique<Sprite>();
		//スプライト作成
		sprite_[i]->Initialize(textureHndle);

		spritePos[0] = { 0.0f + 64.0f * i,0 };
		spritePos[1] = { 64.0f + 64.0f * i,64.0f };

		sprite_[i].get()->SetSize(spritePos[0], spritePos[1]);
		sprite_[i]->SettingWvp();

		isAlive[i] = true;
		alpha_[i] = 1.0f;
	}
}

void HP::Update(int HP) {

	for (int i = 0; i < 3; i++) {
		if (i < HP) {
			isAlive[i] = true;
			alpha_[i] = 1.0f;
		}
		else {
			isAlive[i] = false;
			// 徐々にアルファ値を減少させる
			if (alpha_[i] > 0.0f) {
				alpha_[i] -= kFadeSpeed;
				if (alpha_[i] < 0.0f) {
					alpha_[i] = 0.0f;
				}
			}
		}
	}
}

void HP::Draw() {
	for (int i = 0; i < 3; i++) {
		if (alpha_[i] > 0.0f) {
			// アルファ値を設定
			sprite_[i]->GetMatrial()->SetColor(Vector4(1.0f, 1.0f, 1.0f, alpha_[i]));
			Draw::DrawSprite(sprite_[i].get());
		}
	}
}
