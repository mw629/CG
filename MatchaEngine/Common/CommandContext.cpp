#include "CommandContext.h"
#include <cassert>

CommandContext::~CommandContext()
{
}

CommandContext::CommandContext(ID3D12Device* device)
{
	CreateCommandQueue(device);
	CreateCommandAllocator(device);
	CreateCommandList(device);
}

void CommandContext::CreateCommandQueue(ID3D12Device* device)
{
	//コマンドキューを生成する
	commandQueue_ = nullptr;
	hr_ = device->CreateCommandQueue(&commandQueueDesc_, IID_PPV_ARGS(&commandQueue_));
	//コマンドキューを生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}

void CommandContext::CreateCommandAllocator(ID3D12Device* device)
{
	hr_ = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	//コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}

void CommandContext::CreateCommandList(ID3D12Device* device)
{
	//コマンドリストを生成する
	hr_ = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(),
		nullptr, IID_PPV_ARGS(&commandList_));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}
