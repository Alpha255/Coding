#include "TurnOnTheLight.hlsli"

cbuffer cbVS
{
    matrix WVP;
    matrix World;
    matrix WorldInverse;
}

struct VSInput
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
	float2 UV : TEXCOORD;
    float3 Tangent : TANGENT;
};

Texture2D DiffuseTex;
SamplerState LinearSampler;

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.PosH = mul(vsInput.Pos, WVP);
    output.PosW = mul(vsInput.Pos, World).xyz;
    output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
	///return float4(1.0f, 1.0f, 1.0f, 0.0f);
	return DiffuseTex.Sample(LinearSampler, psInput.UV);
}