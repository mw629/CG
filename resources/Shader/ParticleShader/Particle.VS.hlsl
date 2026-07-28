#include "Particle.hlsli"

StructuredBuffer<Particle> gParticle : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    Particle particle = gParticle[instanceId];
    float32_t4x4 worldMatrix = gPerView.billboardMatrix;
    if (particle.rotate.x != 0.0f || particle.rotate.y != 0.0f || particle.rotate.z != 0.0f)
    {
        float32_t3 rad = particle.rotate;
        float32_t3 s = sin(rad);
        float32_t3 c = cos(rad);
        float32_t3x3 rotX = float32_t3x3(1, 0, 0, 0, c.x, -s.x, 0, s.x, c.x);
        float32_t3x3 rotY = float32_t3x3(c.y, 0, s.y, 0, 1, 0, -s.y, 0, c.y);
        float32_t3x3 rotZ = float32_t3x3(c.z, -s.z, 0, s.z, c.z, 0, 0, 0, 1);
        float32_t3x3 rotMat = mul(rotZ, mul(rotX, rotY));
        worldMatrix[0].xyz = mul(worldMatrix[0].xyz, rotMat);
        worldMatrix[1].xyz = mul(worldMatrix[1].xyz, rotMat);
        worldMatrix[2].xyz = mul(worldMatrix[2].xyz, rotMat);
    }
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    // C++側（EffectDefinition.cpp）ですでにビルボード計算済みの WVP を使用する
    output.position = mul(input.position, mul(worldMatrix, gPerView.viewProjection));
    output.texcoord = input.texcoord;
    output.color = particle.color;

    return output;
}