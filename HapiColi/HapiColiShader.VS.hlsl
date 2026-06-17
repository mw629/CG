struct VSInput {
    float3 position : POSITION;
};

struct VSOutput {
    float4 position : SV_POSITION;
};

cbuffer Constants : register(b0) {
    matrix wvp;
    float4 color;
};

VSOutput main(VSInput input) {
    VSOutput output;
    output.position = mul(float4(input.position, 1.0f), wvp);
    return output;
}
