#pragma once
#include "ObjectBase.h"

class Cube : public ObjectBase
{
public:
    ~Cube() override;

    void Initialize(int textureSrvHandle);

    void CreateVertexData() override;
    void CreateIndexResource() override;
};
