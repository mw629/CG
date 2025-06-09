#pragma once

#include <wrl.h>
#include <ostream>
#include <dxcapi.h>

class ShaderCompile
{
private:
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
public:

	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		//ログ用
		std::ostream& os,
		//CompilerするShaderファイルのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したもの3つ
		Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils,
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler,
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler);

	void CreateShaderCompile(std::ostream& os,
		Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils,
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler,
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler);

	Microsoft::WRL::ComPtr<IDxcBlob>GetVertexShaderBlob(){return vertexShaderBlob_; }

	Microsoft::WRL::ComPtr<IDxcBlob>GetPixelShaderBlob() { return pixelShaderBlob_; }



};

