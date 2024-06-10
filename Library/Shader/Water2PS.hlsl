#include "Phong.hlsli"


Texture2D spriteTexture : register(_spriteTexture);

// タイマーの情報
cbuffer TimerConstant : register(_timerConstant)
{
    float deltaTime;
    float nowTime;
    float2 timePad;
};

//Calculate the squared length of a vector
float length2(float2 p)
{
    return dot(p, p);
}

//Generate some noise to scatter points.
float noise(float2 p)
{
    return frac(sin(frac(sin(p.x) * (43.13311)) + p.y) * 31.0011);
}

float worley(float2 p)
{
    //Set our distance to infinity
    float d = 1e30;
    //For the 9 surrounding grid points
    for (int xo = -1; xo <= 1; ++xo)
    {
        for (int yo = -1; yo <= 1; ++yo)
        {
            //Floor our float2 and add an offset to create our point
            float2 tp = floor(p) + float2(xo, yo);
            //Calculate the minimum distance for this grid point
            //Mix in the noise value too!
            d = min(d, length2(p - tp - noise(tp)));
        }
    }
    return 3.0 * exp(-4.0 * abs(2.5 * d - 1.0));
}

float fworley(float2 p)
{
    //Stack noise layers 
    return sqrt(sqrt(sqrt(
		worley(p * 5.0*3 + 0.05 * nowTime) *
		sqrt(worley(p * 50.0*3 + 0.12 + -0.1 * nowTime)) *
		sqrt(sqrt(worley(p * -10.0*3 + 0.03 * nowTime))))));
}
      
float4 main(VS_OUT pin) : SV_TARGET
{
    float2 fragCoord = pin.texcoord;
    float2 iResolution = { 1280, 720 };

    float2 uv = pin.texcoord;
    //Calculate an intensity
    float t = fworley(uv * iResolution.xy / 1500.0);
    //Add some gradient
    t *= exp(-length2(abs(0.7 * uv - 1.0)));
    //Make it blue!
    float4 fragColor = float4(t * float3(0.1, 1.1 * t, pow(t, 0.5 - t)), 1.0);

    return fragColor;
}
