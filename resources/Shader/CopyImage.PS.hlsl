#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);
sampler gSampler : register(s0);



PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord).r;
    
    if (mask <= 0.5f)
    {
        discard;
    }
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    return output;
}