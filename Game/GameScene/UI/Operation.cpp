#include "Operation.h"


void Operation::Initialize() {
	// テクスチャパスを配列で管理
	std::unique_ptr<Texture> texture = std::make_unique<Texture>();

	const char* texturePathes[6] = {
		"resources/UI/Operation/W.png",
		"resources/UI/Operation/A.png",
		"resources/UI/Operation/S.png",
		"resources/UI/Operation/D.png",
		"resources/UI/Operation/SPACE.png",
		"resources/UI/Operation/ESC.png"
	};
	const Vector2 spritePos[6][2] = {
		{{84.0f,572.0f},{148.0f,636.0f}},
		{{10.0f,646.0f},{74.0f,710.0f}},
		{{84.0f,646.0f},{148.0f,710.0f}},
		{{158.0f,646.0f},{222.0f,710.0f}},
		{{232.0f,646.0f},{360.0f,710.0f}},
		{{1174.0f,646.0f},{1270.0f,710.0f}},
	};

	for (int i = 0; i < 6; i++) {
		Operation_[i] = std::make_unique<Sprite>();
		int textureHandle = texture.get()->CreateTexture(texturePathes[i]);
		Operation_[i].get()->Initialize(textureHandle);
		Operation_[i].get()->SetSize(spritePos[i][0], spritePos[i][1]);
		Operation_[i]->SettingWvp();
		Operation_[i].get()->SetBlend(BlendMode::kBlendModeNormal); // BlendModeを設定
	}
}

void Operation::Update() {
	for (int i = 0; i < 6; i++) {
		Operation_[i].get()->GetMatrial()->SetColor({1.0f,1.0f,1.0f,1.0f});
		Operation_[i]->SettingWvp();
	}
	Vector4 red = { 1.0f,0.0f,0.0f,1.0f };

	if (Input::PressKey(DIK_W)) {
		Operation_[W].get()->GetMatrial()->SetColor(red);
	}
	if (Input::PressKey(DIK_A)) {
		Operation_[A].get()->GetMatrial()->SetColor(red);
	}
	if (Input::PressKey(DIK_S)) {
		Operation_[S].get()->GetMatrial()->SetColor(red);
	}
	if (Input::PressKey(DIK_D)) {
		Operation_[D].get()->GetMatrial()->SetColor(red);
	}
	if (Input::PressKey(DIK_SPACE)) {
		Operation_[SPACE].get()->GetMatrial()->SetColor(red);
	}
	if (Input::PressKey(DIK_ESCAPE)) {
		Operation_[ESC].get()->GetMatrial()->SetColor(red);
	}


}

void Operation::Draw() {
	for (int i = 0; i < 6; i++) {
		if (i != S) {
			Draw::DrawSprite(Operation_[i].get());
		}
	}
}
