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
    float scale;
    float depth;

    float time; 
    float lifeTime; 
    bool isActive; 
};


// カメラの情報
cbuffer CameraConstant : register(_cameraConstant)
{
    row_major float4x4 viewProjection;
    float4 cameraPosition;
}

struct Emitter
{
    float3 pos;             // エミッター座標
		
    float emitLife;         // エミッターの寿命
    float emitLifeTime;     // エミッターの年齢
    float emitTime;         // エミッターが生成されてからの時間
    float emitRate;         // 1秒間のパーティクル発生数
    int emitCount;          // 現在のパーティクル数
};

cbuffer PARTICLE_CONSTANTS : register(b9)
{
    Emitter emitter;
    float particleSize;
    float deltaTime;
    float2 pad;
};