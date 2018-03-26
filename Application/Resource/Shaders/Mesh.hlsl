#include "CommonLighting.hlsli"

cbuffer CBufferVS
{
    matrix World;
    matrix WorldInverse;
    matrix WorldViewProj;

    float4 EyePos;
};

cbuffer CBufferPS
{
    Light LightParams;

    Material MaterialParams;

    uint EnableLight;
    uint EnableMaterial;
    uint Padding[2];
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

Texture2D DiffuseMap;
Texture2D NormalMap;
Texture2D SpecularMap;
Texture2D HeightMap;
SamplerState LinearSampler;

struct VSOutput
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput)
{
    VSOutput vsOutput;

    vsOutput.PosH = mul(float4(vsInput.Pos, 1.0f), WorldViewProj);
    vsOutput.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
    vsOutput.NormalW = mul(vsInput.Normal, (float3x3) WorldInverse);
    vsOutput.UV = vsInput.UV;

    return vsOutput;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, psInput.UV);
}