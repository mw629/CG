#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
sampler gSampler : register(s0);


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    return output;
}