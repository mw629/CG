#include "Particle.hlsli"


StructuredBuffer<TransformationMatrix> gTransformationMatrix : register(t0);


VertexShaderOutput main(VertexShaderInput input,uint32_t instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformationMatrix[instanceID].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3)
    gTransformationMatrix[instanceID].World));
    return output;
}