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


// �J�����̏��
cbuffer CameraConstant : register(_cameraConstant)
{
    row_major float4x4 viewProjection;
    float4 cameraPosition;
}

struct Emitter
{
    float3 pos;             // �G�~�b�^�[���W
		
    float emitLife;         // �G�~�b�^�[�̎���
    float emitLifeTime;     // �G�~�b�^�[�̔N��
    float emitTime;         // �G�~�b�^�[����������Ă���̎���
    float emitRate;         // 1�b�Ԃ̃p�[�e�B�N��������
    int emitCount;          // ���݂̃p�[�e�B�N����
};

cbuffer PARTICLE_CONSTANTS : register(b9)
{
    Emitter emitter;
    float particleSize;
    float deltaTime;
    float2 pad;
};