#include "InputLayout.h"


void InputLayout::CreateInputLayout(const std::string& shader)
{
	// 以前の列挙型 switch 文から、文字列比較による判定に変更しました
	if (shader == "Object3d" || shader == "SkyBox")
	{
		inputElementDescs_[0].SemanticName = "POSITION";
		inputElementDescs_[0].SemanticIndex = 0;
		inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs_[1].SemanticName = "TEXCOORD"; 
		inputElementDescs_[1].SemanticIndex = 0;
		inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs_[2].SemanticName = "NORMAL";
		inputElementDescs_[2].SemanticIndex = 0;
		inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
		inputLayoutDesc_.NumElements = 3;
	}
	else if (shader == "SkinningObject3d")
	{
		inputElementDescs_[0].SemanticName = "POSITION";
		inputElementDescs_[0].SemanticIndex = 0;
		inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs_[1].SemanticName = "TEXCOORD";
		inputElementDescs_[1].SemanticIndex = 0;
		inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs_[2].SemanticName = "NORMAL";
		inputElementDescs_[2].SemanticIndex = 0;
		inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs_[3].SemanticName = "WEIGHT";
		inputElementDescs_[3].SemanticIndex = 0;
		inputElementDescs_[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs_[3].InputSlot = 1;//一番目のslotのVBVのこと
		inputElementDescs_[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs_[4].SemanticName = "INDEX";
		inputElementDescs_[4].SemanticIndex = 0;
		inputElementDescs_[4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
		inputElementDescs_[4].InputSlot = 1;//一番目のslotのVBVのこと
		inputElementDescs_[4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
		inputLayoutDesc_.NumElements = 5;
	}
	else if (shader == "Particle")
	{
		inputElementDescs_[0].SemanticName = "POSITION";
		inputElementDescs_[0].SemanticIndex = 0;
		inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs_[1].SemanticName = "TEXCOORD";
		inputElementDescs_[1].SemanticIndex = 0;
		inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs_[2].SemanticName = "COLOR";
		inputElementDescs_[2].SemanticIndex = 0;
		inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
		inputLayoutDesc_.NumElements = 3;
	}
	else if (shader == "Line")
	{
		inputElementDescs_[0].SemanticName = "POSITION";
		inputElementDescs_[0].SemanticIndex = 0;
		inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs_[1].SemanticName = "COLOR";
		inputElementDescs_[1].SemanticIndex = 0;
		inputElementDescs_[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
		inputLayoutDesc_.NumElements = 2;
	}
}
