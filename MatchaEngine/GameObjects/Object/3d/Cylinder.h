#pragma once
#include "ObjectBase.h"

class Cylinder : public ObjectBase
{
private:
	uint32_t divide_;
	float topRadius_;
	float bottomRadius_;
	float height_;
	float radianPerDivide_;

public:
	void ImGui()override;
	~Cylinder()override;
	void Initialize(int textureSrvHandle);
	void CreateVertexData()override;
	void CreateIndexResource()override;
};

