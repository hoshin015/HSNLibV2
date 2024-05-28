#include "Particles.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);
ConsumeStructuredBuffer<uint> particlePool : register(u1);

float rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}


[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{ 
    // 未使用リストの末尾から取り出す
    uint id = particlePool.Consume();
 
    const float noise_scale = 0.1;
    float f0 = rand(id * noise_scale);
    float f1 = rand(f0 * noise_scale);
    float f2 = rand(f1 * noise_scale);
    
    Particle p = particleBuffer[id];
    p.isActive = true;
    p.position = float3(0, 3, 0);
    
    p.velocity.x = 0.5 * sin(2 * 3.14 * f0);
    p.velocity.y = 0.5 * sin(2 * 3.14 * f1);
    p.velocity.z = 0.5 * sin(2 * 3.14 * f2);
    
    p.color = particleColor;
    
    p.lifeTime = particleLifeTime;
    p.scale = particleSize;
    
    particleBuffer[id] = p;
}