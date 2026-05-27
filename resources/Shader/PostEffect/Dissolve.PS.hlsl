#include "PostEffect.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);
sampler gSampler : register(s0);



PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord).r;
    float32_t edge = 1.0f - smoothstep(0.5f, 0.53f, mask);

    
    if (mask <= 0.5f)
    {
        discard;
    }
    output.color.rgb = edge * float32_t3(1.0f, 0.4f, 0.3f);
    output.color.a = 1.0f;
    
    
    return output;
}
