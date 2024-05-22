#include "Particles.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);
AppendStructuredBuffer<uint> deadList : register(u1);

[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint id = DTid.x;
    
    Particle p = particleBuffer[id];
    
    // g—p’†‚È‚çˆ—‚ğ‚·‚é
    if(p.isActive)
    {
        const float g = -0.5f;
        p.velocity.y += g * deltaTime;
        p.position += p.velocity * deltaTime;
        
        if (p.position.y < 0)
        {
            p.velocity = 0;
            p.position.y = 0;
        }
        
        
        p.lifeTime -= deltaTime;
        
        if(p.lifeTime <= 0)
        {
            p.velocity = float3(0, 0, 0);
            p.isActive = false;
            p.scale = 0;
            deadList.Append(id);
        }
        particleBuffer[id] = p;
    }
}