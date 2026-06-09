#include "Particle.hlsli"

StructuredBuffer<ParticleForGPU> gParticle : register(t0);


VertexShaderOutput main(VertexShaderInput input, uint32_t instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gParticle[instanceID].WVP);
    output.texcoord = input.texcoord;
    output.color = gParticle[instanceID].color;
    return output;
}