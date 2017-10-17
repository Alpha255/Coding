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

    DirectionalLight DirLights[3];

    Material Mat;

    uint UseLighting;
    uint UseTexture;
    uint UseFog;
    uint UseAlphaClip;

    float4 FogClr;

    float FogStart;
    float FogRange;
    float2 Padding;
};

Texture2D g_DiffuseTex;

SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;

    AddressU = WRAP;
    AddressV = WRAP;
};

struct VSInput
{
    float3 Pos    : POSITION;
    float3 Normal : NORMAL;
    float2 UV     : TEXCOORD;
};

struct VSOutput
{
    float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
    float2 UV       : TEXCOORD;
};

VSOutput VS_Main(VSInput input)
{
    VSOutput output;

    output.PosW = mul(float4(input.Pos, 1.0f), World).xyz;

    output.NormalW = mul(input.Pos, (float3x3)WorldInverse);

    output.PosH = mul(float4(input.Pos, 1.0f), WVP);

    output.UV = input.UV;

    return output;
}

float4 PS_Main(VSOutput input) : SV_Target
{
    input.NormalW = normalize(input.NormalW);

    float3 toEye = EyePos.xyz - input.PosW;
    float disToEye = length(toEye);
    toEye = normalize(toEye);

    float4 texClr = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (UseTexture > 0)
    {
        texClr = g_DiffuseTex.Sample(samAnisotropic, input.UV);

        if (UseAlphaClip)
        {
            clip(texClr.a - 0.1f);
        }
    }

    float4 litClr = texClr;
    if (UseLighting)
    {
        float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

		[unroll]
        for (uint i = 0; i < 3; ++i)
        {
            float4 A, D, S;
            ComputeDirectionalLight(Mat, DirLights[i], input.NormalW, toEye, A, D, S);

            ambient += A;
            diffuse += D;
            spec += S;
        }

        litClr = texClr * (ambient + diffuse) + spec;
    }

    if (UseFog)
    {
        float fogLerp = saturate((disToEye - FogStart) / FogRange);

		// Blend the fog color and the lit color.
        litClr = lerp(litClr, FogClr, fogLerp);
    }

    litClr.a = Mat.Diffuse.a * texClr.a;

    return litClr;
}