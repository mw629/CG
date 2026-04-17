#include "SkyBox.hlsli"

TextureCube<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 texturColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = texturColor * gMaterial.color;
    return output;
}