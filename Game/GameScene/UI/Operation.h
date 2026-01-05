#pragma once
#include <memory>
#include <Engine.h>

class Operation
{
private:

	enum {
		W,
		A,
		S,
		D,
		SPACE,
		ESC,
	};

	std::unique_ptr<Sprite> Operation_[6];

public:

	void Initialize();
	void Update();
	void Draw();

};

