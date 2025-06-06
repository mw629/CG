#pragma once
#include <wrl.h>
#include <dxcapi.h>
class DirectXShaderCompiler
{
private:
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
	HRESULT hr_;
public:
	void CreateDXC();

	Microsoft::WRL::ComPtr<IDxcUtils> GetDxcUtils() { return dxcUtils_; }
	Microsoft::WRL::ComPtr<IDxcCompiler3> GetDxcCompiler() { return dxcCompiler_; }
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> GetIncludeHandler() { return includeHandler_; }
};

