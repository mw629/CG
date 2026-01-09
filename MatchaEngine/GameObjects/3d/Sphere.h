#pragma once
#include "Object3DBase.h"


class Sphere :public Object3DBase
{
private:
	uint32_t kSubdivision_ = 16;





public:
	~Sphere()override;

	void Initialize(int textureSrvHandle);

	void CreateVertexData()override;

	void CreateObject()override;

	uint32_t SetSubdivision(int kSubdivision) { kSubdivision_ = kSubdivision; }

	uint32_t GetSubdivision() { return kSubdivision_; }


};

