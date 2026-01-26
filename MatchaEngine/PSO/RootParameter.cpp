#include "RootParameter.h"



void RootParameter::CreateRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature, ShaderName shader)
{
	switch (shader)
	{
	case ObjectShader:
		CreateOBJRootParameter(descriptionRootSignature);
		break;
	case AnimationObj:
		CreateAnimationRootParameter(descriptionRootSignature, shader);

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
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorRange_[0];//Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[3].Descriptor.ShaderRegister = 1;//レジスタ番号1とバインド

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[4].Descriptor.ShaderRegister = 2;//レジスタ番号1とバインド

	rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[5].Descriptor.ShaderRegister = 3;//レジスタ番号1とバインド

	rootParameter[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[6].Descriptor.ShaderRegister = 4;//レジスタ番号1とバインド


	descriptionRootSignature.pParameters = rootParameter;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = 7;//配列の長さ
}

void RootParameter::CreateAnimationRootParameter(D3D12_ROOT_SIGNATURE_DESC& descriptionRootSignature, ShaderName shader)
{
	// descriptorRange_[0] -> matrix palette (t0)
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange_[0].BaseShaderRegister = 0;
	descriptorRange_[0].NumDescriptors = 1; // single StructuredBuffer for matrix palette
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// descriptorRange_[1] -> texture (t1)
	descriptorRange_[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange_[1].BaseShaderRegister = 1;
	descriptorRange_[1].NumDescriptors = 1; // texture
	descriptorRange_[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで行う
	rootParameter[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	// Descriptor table for matrix palette bound to t0 in VS
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで行う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorRange_[0];
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;

	// Descriptor table for texture bound to t1 in PS
	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[3].DescriptorTable.pDescriptorRanges = &descriptorRange_[1];
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;

	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[4].Descriptor.ShaderRegister = 1;//レジスタ番号1とバインド

	rootParameter[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[5].Descriptor.ShaderRegister = 2;//レジスタ番号2とバインド

	rootParameter[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[6].Descriptor.ShaderRegister = 3;//レジスタ番号3とバインド

	rootParameter[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[7].Descriptor.ShaderRegister = 4;//レジスタ番号4とバインド


	descriptionRootSignature.pParameters = rootParameter;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = 8;//配列の長さ
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
	rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorRange_[0];
	rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTebleを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorRange_[0];
	rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;


	descriptionRootSignature.pParameters = rootParameter;
	descriptionRootSignature.NumParameters = 3;

	//descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//descriptorRange_[0].BaseShaderRegister = 0;
	//descriptorRange_[0].NumDescriptors = 1;
	//descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	//rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	//rootParameter[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	//rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	//rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//rootParameter[1].DescriptorTable.pDescriptorRanges = &descriptorRange_[0];
	//rootParameter[1].DescriptorTable.NumDescriptorRanges = 1;

	//rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTebleを使う
	//rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	//rootParameter[2].DescriptorTable.pDescriptorRanges = &descriptorRange_[0];
	//rootParameter[2].DescriptorTable.NumDescriptorRanges = 1;


	//descriptionRootSignature.pParameters = rootParameter;
	//descriptionRootSignature.NumParameters = 3;

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




