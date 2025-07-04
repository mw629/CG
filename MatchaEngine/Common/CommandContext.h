#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "../Graphics/GraphicsDevice.h"

class CommandContext
{
private:
	HRESULT hr_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc_{};
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

public:
	~CommandContext();
	CommandContext(ID3D12Device* device);
	void CreateCommandQueue(ID3D12Device* device);
	void CreateCommandAllocator(ID3D12Device* device);
	void CreateCommandList(ID3D12Device* device);

	ID3D12CommandQueue* GetCommandQueue() { return commandQueue_.Get(); }
	ID3D12CommandAllocator* GetCommandAllocator() { return commandAllocator_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }
};

