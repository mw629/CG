#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
sampler gSampler : register(s0);

static const float32_t PI = 3.14159265f;

static const float32_t2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f,  0.0f }, { 0.0f,  0.0f }, { 1.0f,  0.0f } },
    { { -1.0f,  1.0f }, { 0.0f,  1.0f }, { 1.0f,  1.0f } }
};

cbuffer GaussianBlurData : register(b0)
{
    float32_t gSigma;
    float32_t3 padding;
};

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) / (2.0f * sigma * sigma);
    float coeff = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(coeff);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    uint32_t width, height;
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));

    float32_t weight = 0.0f;
    float32_t3 color = float32_t3(0.0f, 0.0f, 0.0f);

    for (int32_t x = 0; x < 3; ++x)
    {
        for (int32_t y = 0; y < 3; ++y)
        {
            float kernel = gauss(kIndex3x3[x][y].x, kIndex3x3[x][y].y, gSigma);
            float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            color += gTexture.Sample(gSampler, texcoord).rgb * kernel;
            weight += kernel;
        }
    }

    output.color.rgb = color * rcp(weight);
    output.color.a = gTexture.Sample(gSampler, input.texcoord).a;
    return output;
}