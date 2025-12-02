#include "RootParameter.h"



void RootParameter::CreateRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature, ShaderName shader)
{
	switch (shader)
	{
	case ObjectShader:
		CreateOBJRootParameter(descriptionRootSignature);
		break;
	case ParticleShader:
		CreateParticleParameter(descriptionRootSignature);
		break;
	case LineShader:
		CreateLineRootParameter(descriptionRootSignature);
		break;
	case ShaderNum:
		break;
	default:
		break;
	}
}

void RootParameter::CreateOBJRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature)
{
	descriptorRange_[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange_[0].NumDescriptors = 1;//1から始まる
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

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
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[3].Descriptor.ShaderRegister = 1;//レジスタ番号1とバインド

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[4].Descriptor.ShaderRegister = 2;//レジスタ番号1とバインド

	descriptionRootSignature.pParameters = rootParameter;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = 5;//配列の長さ
}

void RootParameter::CreateParticleParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature)
{
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange_[0].BaseShaderRegister = 0; 
	descriptorRange_[0].NumDescriptors = 1;
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameter[1].DescriptorTable.pDescriptorRanges = descriptorRange_;
	rootParameter[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTebleを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange_;//Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);



	descriptionRootSignature.pParameters = rootParameter;
	descriptionRootSignature.NumParameters = 3;
}

void RootParameter::CreateLineRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature)
{
	descriptorRange_[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange_[0].NumDescriptors = 1;//1から始まる
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	// ルートパラメータ0: VS用WVP行列
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // 頂点シェーダーから参照
	rootParameter[0].Descriptor.ShaderRegister = 0; // HLSLのb0にバインド
	rootParameter[0].Descriptor.RegisterSpace = 0;


	descriptionRootSignature.pParameters = rootParameter;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = 1;//配列の長さ
}




