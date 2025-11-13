#pragma once
#include <memory>
#include<Engine.h>

class Fade
{
private:

	enum State {
		kNone,
		kFadeIn,
		kFadeOut,
	};

	State state_ = kNone;

	std::unique_ptr<Sprite> overlay_;
	Vector2 overlayPos_[2] = { {0.0f,0.0f}, {1280.0f,720.0f} };
	Vector4 overlayColor_ = { 0.0f,0.0f,0.0f,0.0f };

	float fadeFream_;
	float fadeSpeed_;

	bool isFinish_;

public:

	void Initialize(float fadeSpeed);

	void Update();

	void Draw();

	void FadeIn();

	void FadeOut();


	bool IsFinish() { return isFinish_; }

};

