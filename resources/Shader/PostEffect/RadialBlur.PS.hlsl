#include "PostEffect.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
sampler gSampler : register(s0);

ConstantBuffer<PostEffectData> gPostEffect : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    const float32_t2 kCenter = float32_t2(0.5f, 0.5f);
    const int32_t kNumSamples = 10;
    // 時間経過でボケ幅を波立たせ、強さ(ratio)を掛ける
    float32_t blurWidth = (0.02f + 0.01f * sin(gPostEffect.time * 5.0f)) * gPostEffect.ratio;
    
    float32_t2 direction = input.texcoord - kCenter;
    float32_t3 outputColor = float32_t3(0.0f, 0.0f, 0.0f);
    for(int32_t sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex)
    {
        float32_t2 texcoord = input.texcoord + direction * blurWidth * float32_t(sampleIndex);
        outputColor += gTexture.Sample(gSampler, texcoord).rgb;
    }
    outputColor.rgb *= rcp(kNumSamples);
    
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
        
    return output;
}
