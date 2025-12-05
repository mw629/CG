#include "InputLayout.h"

void InputLayout::CreateInputLayout(ShaderName shader)
{
	switch (shader)
	{
	case ObjectShader:
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

		inputElementDescs_[3].SemanticName = "POSITION";
		inputElementDescs_[3].SemanticIndex = 0;
		inputElementDescs_[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs_[3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
		inputLayoutDesc_.NumElements = 3;
		break;
	case SpriteShader:
		inputElementDescs_[0].SemanticName = "POSITION";
		inputElementDescs_[0].SemanticIndex = 0;
		inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementDescs_[1].SemanticName = "TEXCOORD";
		inputElementDescs_[1].SemanticIndex = 0;
		inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
		inputLayoutDesc_.NumElements = 2;

		break;
	case ParticleShader:
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
		break;
	case LineShader:
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
		break;
	case ShaderNum:
		break;
	default:
		break;
	}
}
