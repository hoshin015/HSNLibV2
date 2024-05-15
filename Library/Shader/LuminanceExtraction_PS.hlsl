#include "FullScreenQuad.hlsli"
#include "FilterFunctions.hlsli"

cbuffer LuminanceExtractionConstant : register(b0)
{
    float threshould;   // ���P�x���o�̂��߂�臒l
    float intensity;    // �u���[���̋��x
    float2 lumiPad;
}

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps    : register(t0);
Texture2D bloomFilterMap : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    // ���ʂɕ`�悳�ꂽ�e�N�X�`��
    float4 color = textureMaps.Sample(samplerStates[POINT], pin.texcoord);
    
    // r �� 1 �Ȃ玟�̃e�N�X�`���ɐF����������
    float4 flag = bloomFilterMap.Sample(samplerStates[POINT], pin.texcoord);
    
    // RGB > �P�x�l�ɕϊ�
    float luminance = RGB2Luminance(color.rgb);
    clip(luminance - 0.01f);
    
    // 臒l�Ƃ̍����Z�o
    float contribution = max(0, luminance - threshould);
    
    // �o�͂���F��␳����
    contribution /= luminance;
    color.rgb *= contribution * intensity;
    
    //return (flag.r != 0) ? color : 0;
    return flag.r * color;
    //return flag;
}