#include "StaticPhong.hlsli"

SamplerState samplerStates[SAMPLER_NUM] : register(s0);

Texture2D diffuseTexture : register(_deffuseTexture);
Texture2D normalTexture : register(_normalTexture);
Texture2D specularTexture : register(_specularTexture);
Texture2D emissiveTexture : register(_emissiveTexture);

float4 main(VS_OUT pin) : SV_TARGET
{
    // ディフューズマップの取得
    float4 diffuseColor = diffuseTexture.Sample(samplerStates[ANISOTROPIC], pin.texcoord);
    
    float4 finalColor;
    finalColor.a = pin.color.a;
    finalColor.rgb = diffuseColor.rgb * pin.color.rgb;
    
    return finalColor;
}