#include "Common/GraphicsDevice.h"
#include "Math/Calculation.h"
#include <fstream>
#include <sstream>




/// オブジェクトの読み込み///

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

///テクスチャの読み込み///

DirectX::ScratchImage LoadTexture(const std::string& filePath);

Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);

Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);