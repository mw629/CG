#include "InputLayout.h"

void InputLayout::CreateInputLayout()
{
	
	inputElementDescs_[0].SemanticName = "POSITION";
	inputElementDescs_[0].SemanticIndex = 0;
	inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[1].SemanticName = "TEXCOORD"; // texcoord -> TEXCOORD
	inputElementDescs_[1].SemanticIndex = 0;
	inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElementDescs_[2].SemanticName = "NORMAL";
	inputElementDescs_[2].SemanticIndex = 0;
	inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(inputElementDescs_);
}

void InputLayout::CreateLineInputLayout()
{
	lineInputElementDescs_[0].SemanticName = "POSITION";
	lineInputElementDescs_[0].SemanticIndex = 0;
	lineInputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	lineInputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	lineInputElementDescs_[1].SemanticName = "COLOR";
	lineInputElementDescs_[1].SemanticIndex = 0;
	lineInputElementDescs_[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	lineInputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputLayoutDesc_.pInputElementDescs = lineInputElementDescs_;
	inputLayoutDesc_.NumElements = _countof(lineInputElementDescs_);
}
