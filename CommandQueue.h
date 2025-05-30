#pragma once
#include <d3d12.h>
#include <wrl.h>
class CommandQueue
{
private:

	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};


public:

	CommandQueue()

};

