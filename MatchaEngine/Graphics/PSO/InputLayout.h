#pragma once
#include <d3d12.h>
class InputLayout
{
private:
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};

	D3D12_INPUT_ELEMENT_DESC lineInputElementDescs_[2] = {};
	D3D12_INPUT_LAYOUT_DESC lineInputLayoutDesc_{};

public:
	void CreateInputLayout();

	void CreateLineInputLayout();

	D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() { return inputLayoutDesc_; }
	D3D12_INPUT_LAYOUT_DESC GetLineInputLayoutDesc() { return lineInputLayoutDesc_; }
};

