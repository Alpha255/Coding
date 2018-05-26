#include "TurnOnTheLight.hlsli"

cbuffer cbVS
{
    matrix World;
    matrix WorldInverse;
    matrix WVP;
};

cbuffer cbPS
{
	float4 EyePos;

    PointLight Lights[3];

    Material RawMaterial;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

Texture2D DiffuseMap;
SamplerState LinearSampler;

Material ApplyMaterial(in Material materialIn, in VSOutput psInput)
{
    Material materialOut = materialIn;
    materialOut.Diffuse = DiffuseMap.Sample(LinearSampler, psInput.UV);
	materialOut.Normal = float4(normalize(psInput.NormalW), 0.0f);

    return materialOut;
}

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
    output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
	output.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
	float3 lightingColor = float3(0.0f, 0.0f, 0.0f);

    Material material = ApplyMaterial(RawMaterial, psInput);
	for (uint i = 0; i < 3; ++i)
	{
        lightingColor += PointLighting(Lights[i], psInput, EyePos.xyz, material);
    }

	lightingColor += Lights[0].Ambient.rgb;

    return float4(lightingColor, 1.0f);
}