struct PSInput {
    float4 position : SV_POSITION;
};

cbuffer Constants : register(b0) {
    matrix wvp;
    float4 color;
};

float4 main(PSInput input) : SV_TARGET {
    return color;
}
