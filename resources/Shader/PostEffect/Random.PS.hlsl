#include "PostEffect.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
sampler gSampler : register(s0);

float rand2dTo1d(float2 value)
{
    // ドット積で値を圧縮し、サイン波でランダムな周期（小数部）を抽出します
    float random = dot(value, float2(12.9898, 78.233));
    return frac(sin(random) * 43758.5453);
}
PixelShaderOutput main(VertexShaderOutput input)
{   
    PixelShaderOutput output;
    float32_t random = rand2dTo1d(input.texcoord);
    output.color = float32_t4(random, random, random, 1.0f);
  
    return output;
}