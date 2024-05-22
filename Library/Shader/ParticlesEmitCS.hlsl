#include "Particles.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u0);
ConsumeStructuredBuffer<uint> particlePool : register(u1);

float rand(float n)
{
    return frac(sin(n) * 43758.5453123);
}


[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{ 
    // 未使用リストの末尾から取り出す
    uint id = particlePool.Consume();
 
    const float noise_scale = 1.0;
    float f0 = rand(id * noise_scale);
    float f1 = rand(f0 * noise_scale);
    float f2 = rand(f1 * noise_scale);
    
    Particle p = particleBuffer[id];
    p.isActive = true;
    p.position = emitter.pos;
    
    p.velocity.x = 0.5 * sin(2 * 3.14 * f0);
    p.velocity.y = 2.0 * f1;
    p.velocity.z = 0.5 * cos(2 * 3.14 * f0);

    p.color.x = 1.0;
    p.color.y = f0 * 0.5;
    p.color.z = f0 * 0.05;
    p.color.xyz *= 3.0;
    p.color.w = 1.0f;
    
    p.lifeTime = 3;
    p.time = 0;
    p.scale = particleSize;
    
    particleBuffer[id] = p;
}