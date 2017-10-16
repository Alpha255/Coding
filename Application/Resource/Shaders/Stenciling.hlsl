#include "CommonLighting.hlsli"

cbuffer cbVS
{
    matrix World;
    matrix WVP;
};

//cbuffer cvPS
//{
//};

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
    //float3 NormalW : NORMAL;
    float2 UV       : TEXCOORD;
};

VSOutput VS_Main(VSInput input)
{
    VSOutput output;

    output.PosW = mul(float4(input.Pos, 1.0f), World).xyz;

    output.PosH = mul(float4(input.Pos, 1.0f), WVP);

    output.UV = input.UV;

    return output;
}

float4 PS_Main(VSOutput input) : SV_Target
{
    float4 clr = g_DiffuseTex.Sample(samAnisotropic, input.UV);

    return clr;
}