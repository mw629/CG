struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// ラインの色情報を保持する定数バッファの構造体
struct LineMaterial
{
    float4 color; // ラインの色 (RGBA)
};