#pragma once
#include "ObjectBase.h"

class Cylinder : public ObjectBase
{
private:
	uint32_t kDivide_ = 32;
	float radius_ = 1.0f;
	float height_ = 2.0f;
	float radianPerDivide_;
public:
	void ImGui()override;
	~Cylinder()override;
	void Initialize(int textureSrvHandle);
	void CreateVertexData()override;
	void CreateIndexResource()override;
};

