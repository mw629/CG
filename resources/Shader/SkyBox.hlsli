

struct TransformationMatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4x4 WorldInverseTranspose;
};

struct VertexShaderInput
{
    float32_t4 position : SV_POSITION;
    float32_t3 texcoord : TEXCOORD0;
};

struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t3 texcoord : TEXCOORD0;
};

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};