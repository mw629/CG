#include "Line.hlsli" 

struct PixelShaderOutput
{
    float4 color : SV_TARGET0; 
};

// メインのピクセルシェーダー関数
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 定数バッファから読み込んだ色をそのまま出力
    output.color = input.color;
    
    return output;
}