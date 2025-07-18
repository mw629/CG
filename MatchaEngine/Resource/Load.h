#pragma once
#include "Graphics/GraphicsDevice.h"
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


D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriprtorSize, uint32_t index);

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriprtorSize, uint32_t index);

