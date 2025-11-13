#pragma once
#include <memory>
#include <Engine.h>

class HP
{
private:

	std::unique_ptr<Sprite> sprite_[3];
	Transform transform_[3];
	bool isAlive[3];

public:

	void ImGui();

	void Initialize();

	void Update(int HP);

	void Draw();

};

