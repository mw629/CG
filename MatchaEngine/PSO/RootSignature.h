#pragma once
#include <d3d12.h> 
#include <wrl.h>
#include <fstream>

class RootSignature
{
private:
	HRESULT hr_;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_ = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_ = nullptr;
	


public:
	

	void CreateRootSignature(std::ostream& os, Microsoft::WRL::ComPtr <ID3D12Device> device);

	D3D12_ROOT_SIGNATURE_DESC GetDescriptionRootSignature() { return descriptionRootSignature_; };
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSignature() { return rootSignature_; }
	Microsoft::WRL::ComPtr<ID3DBlob> GetsignatureBlob() { return signatureBlob_; }
};

