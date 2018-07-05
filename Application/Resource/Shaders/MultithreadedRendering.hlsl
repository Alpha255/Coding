#include "TurnOnTheLight.hlsli"

cbuffer cbVS
{
    matrix WVP;
    matrix World;
    matrix WorldInverse;
}

cbuffer cbPS
{
	float4 EyePos;
	
	PointLight Lights[4];
};

struct VSInput
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
	float2 UV : TEXCOORD;
    float3 Tangent : TANGENT;
};

Texture2D DiffuseTex;
Texture2D NormalTex;
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
	float4 normalMap = NormalTex.Sample(LinearSampler, psInput.UV);

	Material RawMat;
	RawMat.Diffuse = DiffuseTex.Sample(LinearSampler, psInput.UV);
	RawMat.Specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
	RawMat.Normal = float4(UnpackNormal(normalMap.xyz, normalize(psInput.NormalW), psInput.TangentW), 0.0f);

	float3 lightingClr = float3(0.0f, 0.0f, 0.0f);

	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		lightingClr += PointLighting(Lights[i], psInput.PosW, EyePos.xyz, RawMat);
	}

	///return RawMat.Normal;
	return float4(lightingClr, 0.0f) + Lights[0].Ambient;
}