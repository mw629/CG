#include "AssetManager.h"
#include "Load.h"
#include "Texture.h"
#include <memory>

ModelData AssetManager::LoadModel(const std::string& directoryPath, const std::string& filename)
{
    // AssimpLoadObjFile already uses ModelManager internally which caches models
    return AssimpLoadObjFile(directoryPath, filename);
}

int AssetManager::LoadTexture(const std::string& filePath)
{
    // Texture::CreateTexture already uses TextureLoader internally which caches textures
    std::unique_ptr<Texture> tex = std::make_unique<Texture>();
    return tex->CreateTexture(filePath);
}
