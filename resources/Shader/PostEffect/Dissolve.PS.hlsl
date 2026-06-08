#include "PostEffect.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t4> gMaskTexture : register(t1);
sampler gSampler : register(s0);

ConstantBuffer<PostEffectData> gPostEffect : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 元の画面のピクセルカラーを取得
    float32_t4 color = gTexture.Sample(gSampler, input.texcoord);
    
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord).r;
    
    // 時間で変動するしきい値を計算 (0.0 から 1.0 を往復)
    float32_t threshold = sin(gPostEffect.time * 2.0f) * 0.5f + 0.5f; 
    
    float32_t edge = 1.0f - smoothstep(threshold, threshold + 0.03f, mask);
    
    if (mask <= threshold)
    {
        discard;
    }
    
    // 元の色にエッジの赤色を加算して出力
    output.color.rgb = color.rgb + edge * float32_t3(1.0f, 0.4f, 0.3f);
    output.color.a = 1.0f;
    
    return output;
}
