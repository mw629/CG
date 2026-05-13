#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
sampler gSampler : register(s0);


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    float32_t value = dot(output.color.rgb, float3(0.2125f, 0.7154f, 0.0721f));
    output.color = float32_t4(value, value, value,output.color.w);
    return output;
}