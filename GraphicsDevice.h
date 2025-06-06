#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <format>
#include <string>
#include <filesystem>
#include <dxgidebug.h>

#include "externals/DirectXTex/DirectXTex.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class GraphicsDevice
{
private:

	HRESULT hr_;
	
	IDXGIFactory7* dxgiFactory_ = nullptr;

	IDXGIAdapter4* useAdapter_ = nullptr;

	ID3D12Device* device_ = nullptr;


public:
	
	ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes);

	void Initialize(std::ostream& os);

	/// <summary>
	/// DXGIFactoryの生成をする
	/// </summary>
	void CreateDxgiFactory();

	/// <summary>
	/// 使用するアダプタを決定する
	/// </summary>
	void SelectAdapter(std::ostream& os);

	/// <summary>
	/// D3D12Deviceの生成
	/// </summary>
	void CreateD3D12Device(std::ostream& os);
};

