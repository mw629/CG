#pragma once
#include "externals/DirectXTex/d3dx12.h"

#include "GraphicsDevice.h"

class RootSignature
{
private:
	HRESULT hr_;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_ = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_ = nullptr;
	GraphicsDevice* graphicsDevice;


public:
	~RootSignature();

	void CreateRootSignature(std::ostream& os);

	D3D12_ROOT_SIGNATURE_DESC GetDescriptionRootSignature() { return descriptionRootSignature_; };
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() { return rootSignature_; }
	Microsoft::WRL::ComPtr<ID3DBlob> GetsignatureBlob() { return signatureBlob_; }
};

