#include "ObjectBase.h"

#pragma once
class Ring : public ObjectBase
{
private:
	uint32_t kRingDivide_ = 32;
	float outerRadius_ = 1.0f;
	float innerRadius_ = 0.5f;
	float radianPerDivide_;

public:
	~Ring()override;

	void Initialize(int textureSrvHandle);

	void CreateVertexData()override;
	void CreateIndexResource()override;

};

