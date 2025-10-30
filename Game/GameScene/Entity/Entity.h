#pragma once
#include <Engine.h>

class Entity 
{
protected:

	std::unique_ptr<Model> backImage_;
	Transform transform_;

	float kWidth_;
	float kHeight_;

	bool isAlive_;

public:
	virtual ~Entity() {};

	virtual void ImGui() {}

	virtual void Initialize() {}

	virtual void Update() {}

	virtual void Draw() {}

	Transform GetTransform() { return transform_; }

	AABB GetAABB();

};
