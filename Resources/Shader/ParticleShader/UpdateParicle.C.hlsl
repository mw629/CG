
static const uint32_t kMaxParticles = 1024;
RWStructuredBuffer<Particle> gParticles : register(u0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint32_t particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        //alphaが0のparticleは生きていないとみなし更新しない
        if (gParticles[particleIndex].color.a != 0.0f)
        {
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
            gParticles[particleIndex].currentTime += gParticles[particleIndex].lifetime;
            float32_t alpha =1.0f-(gParticles[particleIndex].currentTime / gParticles[particleIndex].lifetime);
            gParticles[particleIndex].color.a = saturate(alpha);
        }
    }  
}