#include "RootSignature.h"
#include "LogHandler.h"
#include <cassert>



RootSignature::RootSignature()
{
	descriptionRootSignature_.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

}

void RootSignature::CreateRootSignature(std::ostream& os, ID3D12Device* device)
{
	//シリアライズしてバイナリにする
	hr_ = D3D12SerializeRootSignature(&descriptionRootSignature_,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_, &errorBlob_);
	if (FAILED(hr_)) {
		Error(os);
		Log(os, reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成
	hr_ = device->CreateRootSignature(0,
		signatureBlob_->GetBufferPointer(), signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr_));
}

void RootSignature::Error(std::ostream& os)
{
	std::string errorMsg = "RootSignature Error!\n";
	errorMsg += "File: " __FILE__ "\n";
	errorMsg += "Line: " + std::to_string(__LINE__) + "\n";
	errorMsg += "HRESULT: 0x" + std::to_string(hr_) + "\n";
	if (errorBlob_ && errorBlob_->GetBufferPointer()) {
		errorMsg += "D3D12 Error: ";
		errorMsg += reinterpret_cast<const char*>(errorBlob_->GetBufferPointer());
	}
	else {
		errorMsg += "No errorBlob message.\n";
	}
	Log(os, errorMsg);
}


