#include "Particles.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);

[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint id = DTid.x;
    
    Particle p = particleBuffer[id];
    
    if(p.age > 10.0)
    {
        const float g = -0.5f;
        p.velocity.y += g * deltaTime;
        p.position += p.velocity * deltaTime;
        
        if(p.position.y < 0)
        {
            p.velocity = 0;
            p.position.y = 0;
        }
        
        // [“x’lŽæ“¾
        float4 clipSpacePosition = mul(float4(p.position, 1), viewProjection);
        float ndcZ = clipSpacePosition.z / clipSpacePosition.w;
        p.depth = (ndcZ + 1.0) * 0.5; // ³‹K‰»‚³‚ê‚½[“x’l
    }
    p.age += deltaTime;
    
    particleBuffer[id] = p;
}