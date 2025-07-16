#include "Line.hlsli"

struct TransformationMatrix
{
    float4x4 WVP; // World-View-Projection 行列
};
ConstantBuffer<TransformationMatrix> gTransform : register(b0);

struct VertexShaderInput
{
    float3 position : POSITION0; // モデル空間での位置
    float4 color : COLOR0;
};


// メインの頂点シェーダー関数
VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    output.position = mul(float4(input.position, 1.0f), gTransform.WVP);
    output.color = input.color;
    
    return output;
}