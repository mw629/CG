#include "PostEffect.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t> gMaskTexture : register(t1);
sampler gSampler : register(s0);



PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
  
    
    int32_t SAMPLES = 20;
    float32_t2 center = float32_t2(0.5, 0.5);
    float32_t2 dir =  center- v_uv;
    float32_t2 step = (dir * u_strength) / float(SAMPLES);

    for (int i = 0; i < SAMPLES; i++)
    {
        output.color = gTexture.Sample(gSampler, input.texcoord);
        current_uv += step;
    }

    // 最後に自分で定義した outColor に出力します
    output.color = color / float(SAMPLES);
    
    return output;
}
