#include "CommonLighting.hlsli"

cbuffer cbVS
{
    matrix World;
    matrix WorldInverse;
    matrix WVP;
};

cbuffer cbPS
{
    float4 EyePos;

    Material Mat;

    uint UseFog;
    uint UseAlphaClip;
    float FogStart;
    float FogRange;

    float4 FogClr;
};

Texture2D DiffuseTex;
SamplerState LinearSampler;

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
    float2 UV       : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    output.PosW = mul(float4(input.Pos, 1.0f), World).xyz;

    output.NormalW = mul(input.Pos, (float3x3)WorldInverse);

    output.PosH = mul(float4(input.Pos, 1.0f), WVP);

    output.UV = input.UV;

    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    input.NormalW = normalize(input.NormalW);

    float3 toEye = EyePos.xyz - input.PosW;
    float disToEye = length(toEye);
    toEye = normalize(toEye);

    float4 color = DiffuseTex.Sample(LinearSampler, input.UV);
    if (UseAlphaClip)
    {
        clip(color.a - 0.1f);
    }

    if (UseFog)
    {
        float fogLerp = saturate((disToEye - FogStart) / FogRange);

		// Blend the fog color and the lit color.
        color = lerp(color, FogClr, fogLerp);
    }

    color.a = Mat.Diffuse.a * color.a;

    return color;
}