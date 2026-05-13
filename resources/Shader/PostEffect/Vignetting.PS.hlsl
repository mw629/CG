#include "Vignetting.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
sampler gSampler : register(s0);


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
   
    //周囲を0に、中心になるほど暗くなるように計算する
    float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    //currectだけで計算すると中心のサイファ位置が0.0635で暗すぎるのでScaleで調整
    float vignette = correct.x * correct.y * 16.0f;
    //とりあえず0.8乗でそれっぽく
    vignette = saturate(pow(vignette, 0.8f));
    //係数として乗算
    output.color.rgb *= vignette;
        
    return output;
}