#include "PostEffect.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
sampler gSampler : register(s0);

static const uint32_t kKernelSize = 3;
static const float32_t PI = 3.14159265f;

static const float32_t2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};

float gauss(float32_t x, float32_t y, float32_t sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color.a = 1.0f;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    float32_t weight = 0.0f;
    float32_t kernel3x3[kKernelSize][kKernelSize];
    
    for (int32_t x = 0; x < kKernelSize; ++x)
    {
        for (int32_t y = 0; y < kKernelSize; ++y)
        {
            kernel3x3[x][y] = gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, 2.0f);
            weight += kernel3x3[x][y];
            
            
        }
    }
    
    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    
    for (int32_t x = 0; x < kKernelSize; ++x)
    {
        for (int32_t y = 0; y < kKernelSize; ++y)
        {
            float32_t2 texcood = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcood).rgb;
            output.color.rgb += fetchColor * kernel3x3[x][y];
        }
    }
        
    return output;
}