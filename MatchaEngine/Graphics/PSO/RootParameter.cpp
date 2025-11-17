#include "RootParameter.h"

void RootParameter::CreateDescriptorRange()
{
	descriptorRange_[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange_[0].NumDescriptors = 1;//1から始まる
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
}

void RootParameter::CreateLineRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature)
{
	CreateDescriptorRange();
	// ルートパラメータ0: VS用WVP行列
	lineRootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	lineRootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // 頂点シェーダーから参照
	lineRootParameter[0].Descriptor.ShaderRegister = 0; // HLSLのb0にバインド
	lineRootParameter[0].Descriptor.RegisterSpace = 0;


	descriptionRootSignature.pParameters = lineRootParameter;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(lineRootParameter);//配列の長さ
}

void RootParameter::CreateParticleParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature)
{
	CreateDescriptorRange();

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTadleを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで行う
	rootParameter[1].DescriptorTable.pDescriptorRanges = descriptorRange_; //Tableの中身の配列を配列を指定
	rootParameter[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);//Tableで利用する数
}

void RootParameter::CreateRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature)
{
	CreateDescriptorRange();
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで行う
	rootParameter[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTebleを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange_;//Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//VertexShaderで行う
	rootParameter[3].Descriptor.ShaderRegister = 1;//レジスタ番号1とバインド

	descriptionRootSignature.pParameters = rootParameter;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameter);//配列の長さ
}
