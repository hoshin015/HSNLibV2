#include "Phong.hlsli"


Texture2D spriteTexture : register(_spriteTexture);

// タイマーの情報
cbuffer TimerConstant : register(_timerConstant)
{
    float deltaTime;
    float nowTime;
    float2 timePad;
};



// https://www.shadertoy.com/view/fsSfR3
float rand(float2 n)
{
    return frac(cos(dot(n, float2(5.9898, 4.1414))) * 65899.89956);
}


float noise(in float2 n)
{
    const float2 d = float2(0.0, 1.0);
    float2 b = floor(n), f = smoothstep(float2(0.0,0), float2(1.0,1), frac(n));
    return lerp(lerp(rand(b), rand(b + d.yx), f.x), lerp(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

float fbm(float2 n)
{
    float total = 0.0;
    float amplitude = 1.0;
    for (int i = 0; i < 4; i++)
    {
        total += noise(n) * amplitude;
        n += n;
        amplitude *= 0.51;
    }
    return total;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 uv = pin.texcoord;
    uv *= 20;

    float3 col1 = float3(0.5, 0.0, 0.1);
    float3 col2 = float3(0.9, 0.3, 0.1);
    float3 col3 = float3(0.0, 0.0, 0.0);
    float3 col4 = float3(1.0, 0.9, 0.6);
    float3 col5 = float3(0.1, 0.4, 0.8);
    float3 col6 = float3(1.155, 1.155, 1.155);

    float2 speed = float2(0.35, 0.35);

    float2 nfbm = float2(uv.x, uv.y - rand(float2(1., 5.)));
    float q = fbm(nfbm);
    float2 arg1 = float2(uv + q + nowTime * speed.x - uv.x - uv.y);
    float2 arg2 = float2(uv + q - rand(float2(100., 100.)) * speed.y);
    float2 r = float2(fbm(arg1), fbm(arg2));

    float2 agr3 = float2(uv + r);
    float3 c = lerp(col1, col2, fbm(agr3)) + lerp(col3, col4, r.x) - lerp(col5, col6, r.y);


    float4 fragColor = float4(c, 1.);
    return fragColor;
}