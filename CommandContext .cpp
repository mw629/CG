#include "CommandContext .h"
#include <cassert>

void CommandContext::CreateCommandQueue()
{
	//コマンドキューを生成する
	commandQueue_ = nullptr;
	hr_ = graphicsDevice_->GetDevice()->CreateCommandQueue(&commandQueueDesc_, IID_PPV_ARGS(&commandQueue_));
	//コマンドキューを生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}

void CommandContext::CreateCommandAllocator()
{
	hr_ = graphicsDevice_->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	//コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}

void CommandContext::CreateCommandList()
{
	//コマンドリストを生成する
	hr_ = graphicsDevice_->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(),
		nullptr, IID_PPV_ARGS(&commandList_));
	//コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}
