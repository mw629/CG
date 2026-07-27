#include "Particle.hlsli"

StructuredBuffer<ParticleForGPU> gParticle : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    
    // C++側（EffectDefinition.cpp）ですでにビルボード計算済みの WVP を使用する
    output.position = mul(input.position, gParticle[instanceId].WVP);
    output.texcoord = input.texcoord;
    output.color = gParticle[instanceId].color;

    return output;
}