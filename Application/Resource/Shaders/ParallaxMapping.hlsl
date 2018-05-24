#include "TurnOnTheLight.hlsli"

cbuffer cbVS
{
	matrix World;
	matrix WorldInverse;
    matrix WVP;
	matrix UVTransform;
};

cbuffer cbPS
{
	float4 EyePos;
	DirectionalLight DirLight;
	Material Mat;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
	output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
	output.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
	output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.UV = mul(float4(vsInput.UV, 0.0f, 1.0f), UVTransform).xy;

    return output;
}

float4 PSMain_NormalMapping(VSOutput psInput) : SV_Target
{
	float3 lightingColor = DirectionalLighting(DirLight, psInput, EyePos.xyz, Mat);

	return float4(lightingColor, 1.0f);
}