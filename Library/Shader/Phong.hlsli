#include "../RegisterNum.h"


struct VS_IN
{
    float4  position : POSITION;
    float4  normal : NORMAL;
    float4  tangent : TANGENT;
    float2  texcoord : TEXCOORD;
    float4  boneWeights : WEIGHTS;
    uint4   boneIndices : BONES;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 worldNormal : NORMAL;
    float4 worldTangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

// BONE �̍ő吔
static const int MAX_BONES = 256;

// �J�����̏��
cbuffer CameraConstant : register(_cameraConstant)
{
    row_major float4x4 viewProjection;
}

// 3d ��Ԃɏo���S�Ẵ��f��������
cbuffer ObjectConstant : register(_objectConstant)
{
    row_major float4x4 world;
    float4 materialColorKd;
    float4 materialColorKs;
    float4 materialColorKa;
    row_major float4x4 boneTransforms[MAX_BONES];
}

