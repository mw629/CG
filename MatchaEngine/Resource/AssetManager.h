#pragma once
#include <string>
#include "Core/VariableTypes.h"

class AssetManager
{
public:
    static ModelData LoadModel(const std::string& directoryPath, const std::string& filename);
    static int LoadTexture(const std::string& filePath);
};
