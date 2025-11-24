#include "Particle.hlsli"


StructuredBuffer<ParticleForGPU> gParticle : register(t0);


VertexShaderOutput main(VertexShaderInput input,uint32_t instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gParticle[instanceID].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3)
    gParticle[instanceID].World));
    output.color = input.color;
    return output;
}