#pragma once
#include <Engine.h>

class Entity 
{
protected:

	//モデル関連
	std::unique_ptr<Model> model_;
	Transform transform_ = { {1.0f,1.0f,1.0f}, { 0.0f,0.0f,0.0f },{0.0f,0.0f,0.0f} };
	Vector4 modelColor_;

	//サイズ
	float kWidth_;
	float kHeight_;

	//生存フラグ
	bool isAlive_;

public:
	virtual ~Entity() {};

	virtual void ImGui() {}

	virtual void Initialize() {}

	virtual void Update(Matrix4x4 viewMatrix) {}

	virtual void Draw() {}

	Transform GetTransform() { return transform_; }

	bool IsAlive() { return isAlive_; }
	
	AABB GetAABB();

};
