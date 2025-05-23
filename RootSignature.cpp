#include "RootSignature.h"

void RootSignature::CreateRootSignature( )
{
	//DescriptorRange//
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//1から始まる
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//RootParameter//
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//RootParameter
	D3D12_ROOT_PARAMETER rootParameter[4] = {};
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで行う
	rootParameter[1].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTebleを使う
	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで行う
	rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);

	rootParameter[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//VertexShaderで行う
	rootParameter[3].Descriptor.ShaderRegister = 1;//レジスタ番号0とバインド

	descriptionRootSignature.pParameters = rootParameter;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameter);//配列の長さ
}
