#pragma once
#include <d3d12.h>
#include "RenderState.h"
#include <string>

class InputLayout
{
private:
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[5] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};


public:
	void CreateInputLayout(const std::string& shader);

	D3D12_INPUT_LAYOUT_DESC GetInputLayoutDesc() { return inputLayoutDesc_; }
};

