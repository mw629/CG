#pragma once
#include <d3d12.h>
class InputLayout
{
private:
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
public:
	void CreateInputLayout();

	D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() { return inputLayoutDesc_; }
};

