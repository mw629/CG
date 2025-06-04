#pragma once
#include <d3d12.h>
class InputLayout
{
private:
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
public:
	void CreateInputLayout();

};

