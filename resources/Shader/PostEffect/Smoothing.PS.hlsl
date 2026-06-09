#include "PostEffect.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
sampler gSampler : register(s0);

ConstantBuffer<PostEffectData> gPostEffect : register(b0);

static const uint32_t kKernelSize = 5;



PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 1. 元の画像のアルファ値（透明度）だけを先にキープしておく
    float4 originalColor = gTexture.Sample(gSampler, input.texcoord);
    output.color.a = originalColor.a;
    
    // 2. ボカシ用の色を貯める変数を 0（真っ黒）で初期化
    float32_t3 blurColor = float32_t3(0.0f, 0.0f, 0.0f);
    
    // 3. (float) にキャストして正確に1ピクセル分のUVサイズを出す
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp((float) width), rcp((float) height)) * gPostEffect.blurStrength;
    
    // 4. 指定されたカーネルサイズに応じてループを回す
    if (gPostEffect.kernelSize == 3)
    {
        for (int32_t y = 0; y < 3; ++y)
        {
            for (int32_t x = 0; x < 3; ++x)
            {
                float32_t2 texcood = input.texcoord + gPostEffect.index3x3[y][x] * uvStepSize;
                float32_t3 fetchColor = gTexture.Sample(gSampler, texcood).rgb;
                blurColor += fetchColor * gPostEffect.kernel3x3[y][x];
            }
        }
    }
    else
    {
        for (int32_t y = 0; y < 5; ++y)
        {
            for (int32_t x = 0; x < 5; ++x)
            {
                float32_t2 texcood = input.texcoord + gPostEffect.index5x5[y][x] * uvStepSize;
                float32_t3 fetchColor = gTexture.Sample(gSampler, texcood).rgb;
                blurColor += fetchColor * gPostEffect.kernel5x5[y][x];
            }
        }
    }
    
    // 5. 最終的に合計したキレイなボカシ色（合計100%）を代入
    output.color.rgb = blurColor;
    
    return output;
}