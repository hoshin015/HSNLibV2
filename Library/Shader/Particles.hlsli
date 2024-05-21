#include "../RegisterNum.h"

struct VS_OUT
{
    uint vertexId : VERTEXID;
};

struct GS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct Particle
{
    float4 color;
    float3 position;
    float3 velocity;
    float age;
    int state;
    float depth;
};


// ÉJÉÅÉâÇÃèÓïÒ
cbuffer CameraConstant : register(_cameraConstant)
{
    row_major float4x4 viewProjection;
    float4 cameraPosition;
}

cbuffer PARTICLE_CONSTANTS : register(b9)
{
    float3 emitterPosition;
    float particleSize;
    float deltaTime;
    float3 pad;
};