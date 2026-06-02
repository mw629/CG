#include "PostEffect.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
sampler gSampler : register(s0);

ConstantBuffer<PostEffectData> gPostEffect : register(b0);

float rand2dTo1d(float2 value, float time)
{
    // テクスチャ座標と時間を組み合わせてアニメーションするノイズのシード値を生成します
    float random = dot(value + sin(time), float2(12.9898, 78.233));
    return frac(sin(random) * 43758.5453);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 元のシーンのカラーをサンプリングします
    float32_t4 color = gTexture.Sample(gSampler, input.texcoord);
    
    // ノイズ生成用のUV座標を決定します
    float32_t2 noiseUv = input.texcoord;
    if (gPostEffect.value1 > 0.0f)
    {
        // UV座標を量子化して、ドット絵風の粗いブロックノイズにします
        float32_t noiseScale = gPostEffect.value1 * 100.0f;
        noiseUv = floor(noiseUv * noiseScale) / noiseScale;
    }
    
    // ノイズの値を生成します
    float32_t noiseR = rand2dTo1d(noiseUv, gPostEffect.time);
    float32_t noiseG = noiseR;
    float32_t noiseB = noiseR;
    
    if (gPostEffect.value2 > 0.0f)
    {
        // 座標オフセットをずらしてRGB個別のノイズにし、カラーノイズにします
        noiseG = rand2dTo1d(noiseUv + float32_t2(0.1f, 0.2f), gPostEffect.time);
        noiseB = rand2dTo1d(noiseUv + float32_t2(0.3f, 0.4f), gPostEffect.time);
    }
    
    float32_t3 noiseColor = float32_t3(noiseR, noiseG, noiseB);
    
    // ratio に基づいて元のカラーとノイズをブレンドします（ratio=1.0 のとき完全なノイズ塗りつぶしになります）
    output.color.rgb = lerp(color.rgb, noiseColor, gPostEffect.ratio);
    output.color.a = color.a;
  
    return output;
}