

struct Camera
{
    float32_t3 worldPosition;
};

struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t3 worldPosition : POSITION0;
};

struct TransformationMatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4x4 WorldInverseTranspose;
};

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

struct DirectionalLight
{
    float32_t4 color; //ライトの色
    float32_t3 direction; //ライトの向き
    float32_t intensity; //輝度
};

struct PointLight
{
    float32_t4 color; //ライトの色
    float32_t3 position; //ライトの向き
    float32_t intensity; //輝度
    float32_t radius; //ライトの届く最大距離
    float32_t decay; //減衰率
};

struct SpotLight
{
    float32_t4 color; //ライトの色
    float32_t3 position; //ライトの向き
    float32_t intensity; //輝度
    float32_t3 direction; //ライトの向き
    float32_t distance; //ライトの届く最大距離
    float32_t decay; //減衰率
    float32_t cosAngle; //スポットライトの余弦
    float32_t cosFalloffStart;//Falloの開始角度
};

// ライトの数を受け取る
struct LightCounts
{
    uint pointLightCount;
    uint spotLightCount;
};

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};