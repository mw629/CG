#include "Particle.hlsli" 

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    // UVから中心からの距離を計算（texcoordは0.0～1.0）
    float2 centerOffset = input.texcoord - float2(0.5f, 0.5f);
    float distance = length(centerOffset) * 2.0f; // 中心で0.0、端で1.0
    
    // ほこり・煙用のソフトエッジ（smoothstepで滑らかに減衰）
    float softEdge = smoothstep(1.0f, 0.0f, distance);
    
    // 基本カラーの計算
    output.color = gMaterial.color * textureColor * input.color;
    
    // 黒背景テクスチャの輝度と、ソフトエッジを掛け合わせて透明度にする
    output.color.a *= textureColor.r * softEdge;

    // 煙のような質感を出すため、エッジに向かって少し色をフェード（任意）
    output.color.rgb = lerp(output.color.rgb * 0.5f, output.color.rgb, softEdge);

    if (output.color.a <= 0.0f)
    {
        discard;
    }
   
    return output;
}
