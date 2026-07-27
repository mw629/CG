#include "Particle.hlsli"

StructuredBuffer<Particle> gParticle : register(t0);
StructuredBuffer<PreView> gParView : register(t0);



VertexShaderOutput main(VertexShaderInput input, uint32_t instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    Particle particle = gParticle[instanceID];
    float32_t4x4 worldMatrix = gParView.billboardMatrix;
    
    output.position = mul(input.position, gParticle[instanceID].WVP);
    output.texcoord = input.texcoord;
    output.color = gParticle[instanceID].color;
    return output;
}