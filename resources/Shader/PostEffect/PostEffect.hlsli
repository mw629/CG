
struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
};

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET;
};

struct PostEffectData
{
    float32_t time;
    float32_t ratio;
    float32_t value1;
    float32_t value2;
};