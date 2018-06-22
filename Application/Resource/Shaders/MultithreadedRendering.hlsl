#include "TurnOnTheLight.hlsli"

cbuffer cbVS
{
    matrix WVP;
    matrix World;
    matrix WorldInverse;
}

struct VSInput
{
    float3 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    ///float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
    output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    ///output.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 0.0f);
}