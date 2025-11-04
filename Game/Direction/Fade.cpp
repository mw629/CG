#include "Fade.h"


void Fade::Initialize() {

	std::unique_ptr<Texture> texture = std::make_unique<Texture>();
	int textureHndle = texture.get()->CreateTexture("resources/white64x64");

	overlay_ = std::make_unique<Sprite>();
	overlay_.get()->Initialize(textureHndle);

}


