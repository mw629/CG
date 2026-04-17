#include "CopyImage.hlsli"

Texture2D<float32_t> gTexture : register(t0);
sampler gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET;
};

