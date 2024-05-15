#include "StaticPhong.hlsli"

VS_OUT main(VS_IN vin)
{
    float3 p = vin.position;
    p = mul(float4(p, 1.0f), worldTransforms[vin.instanceId]).xyz;
    
    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), viewProjection);
    
    vout.texcoord = vin.texcoord;
    vout.color = materialColorKd;
    
    return vout;
}