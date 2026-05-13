#include "GrayScale.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
sampler gSampler : register(s0);


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    float32_t value = dot(output.color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
    output.color = value * float32_t4(1.0f, 74.0f / 107.0f, 43.0f / 107.0f, output.color.w);
    return output;
}