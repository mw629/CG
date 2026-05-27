#include "PostEffect.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
sampler gSampler : register(s0);

static const uint32_t kKernelSize = 5;

static const float32_t2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};
static const float32_t kKernel3x3[3][3] =
{
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f }
};

static const float32_t2 kIndex5x5[5][5] =
{
    { { -2.0f, -2.0f }, { -1.0f, -2.0f }, { 0.0f, -2.0f }, { 1.0f, -2.0f }, { 2.0f, -2.0f } },
    { { -2.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f }, { 2.0f, -1.0f } },
    { { -2.0f, 0.0f }, { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 2.0f, 0.0f } },
    { { -2.0f, 1.0f }, { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 2.0f, 1.0f } },
    { { -2.0f, 2.0f }, { -1.0f, 2.0f }, { 0.0f, 2.0f }, { 1.0f, 2.0f }, { 2.0f, 2.0f } }
};
static const float32_t kKernel5x5[5][5] =
{
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
    { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f }
};

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
    float32_t2 uvStepSize = float32_t2(rcp((float) width), rcp((float) height));
    
    // 4. 正しいインデックス順（[y][x]）で回るようにループを調整
    for (int32_t y = 0; y < kKernelSize; ++y)
    {
        for (int32_t x = 0; x < kKernelSize; ++x)
        {
            // 配列の指定を [y][x] に修正
            float32_t2 texcood = input.texcoord + kIndex5x5[y][x] * uvStepSize;
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcood).rgb;
            
            // 25ピクセル分の色を「重み（1/25）」をかけながら足していく
            blurColor += fetchColor * kKernel5x5[y][x];
        }
    }
    
    // 5. 最終的に合計したキレイなボカシ色（合計100%）を代入
    output.color.rgb = blurColor;
    
    return output;
}