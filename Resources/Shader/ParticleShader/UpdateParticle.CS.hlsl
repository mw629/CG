#include "Particle.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);

ConstantBuffer<PerFrame> gPerFrame : register(b0);

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    uint32_t particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        // alphaが0のparticleは生きていないとみなし更新しない
        if (gParticles[particleIndex].color.a != 0.0f)
        {
            gParticles[particleIndex].velocity += gPerFrame.acceleration;
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
            gParticles[particleIndex].scale *= gPerFrame.sizeDelta;
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float32_t lifetime = gParticles[particleIndex].lifetime > 0.0f ? gParticles[particleIndex].lifetime : 3.0f;
            float32_t alpha = 1.0f - (gParticles[particleIndex].currentTime / lifetime);
            gParticles[particleIndex].color.a = saturate(alpha);
        }
    }  
}
