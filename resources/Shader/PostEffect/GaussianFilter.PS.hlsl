PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color.a = 1.0f;
    
    // 【重要】RGBを0で初期化
    output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
    
    float32_t weight = 0.0f;
    float32_t3 accumulatedColor = float32_t3(0.0f, 0.0f, 0.0f);
    
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(float32_t(width)), rcp(float32_t(height)));
    
    // ループを1つにまとめて効率化
    for (int32_t x = 0; x < kKernelSize; ++x)
    {
        for (int32_t y = 0; y < kKernelSize; ++y)
        {
            // sigmaを2.0fから1.0fに変更すると、3x3でもメリハリのあるボケになります
            float32_t g = gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, 1.0f);
            weight += g;
            
            float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            accumulatedColor += gTexture.Sample(gSampler, texcoord).rgb * g;
        }
    }
    
    // 【重要】重みの合計で割って正規化する
    output.color.rgb = accumulatedColor * rcp(weight);
        
    return output;
}