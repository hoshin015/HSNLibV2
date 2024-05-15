#include "Phong.hlsli"

VS_OUT main(VS_IN vin)
{
    float4 blendedPosition = { 0, 0, 0, 1 };
    for (int boneIndex = 0; boneIndex < 4; boneIndex++)
    {
        blendedPosition += vin.boneWeights[boneIndex] * mul(vin.position, boneTransforms[vin.boneIndices[boneIndex]]);
    }
    vin.position = float4(blendedPosition.xyz, 1.0f);

    VS_OUT vout;
    vout.position = mul(vin.position, mul(world, viewProjection));
    
    vout.texcoord = vin.texcoord;
    vout.color = materialColorKd;
    
    return vout;
}