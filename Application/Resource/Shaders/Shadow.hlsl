#include "TurnOnTheLight.hlsli"

Texture2D DiffuseMap;
Texture2D DepthMap;
SamplerState Sampler;

cbuffer cbVS
{
    matrix World;
    matrix WorldInverse;
    matrix WVP;
};

cbuffer cbPS
{
    float4 EyePos;
    Material RawMat;
    PointLight Light;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

Material ApplyMaterial(in Material materialIn, in VSOutput psInput)
{
    Material materialOut = materialIn;

    materialOut.Diffuse = DiffuseMap.Sample(Sampler, psInput.UV);

    materialOut.Normal = float4(normalize(psInput.NormalW), 0.0f);

    return materialOut;
}

VSOutput VSMain_Depth(VSInput vsInput)
{
	VSOutput output;
    output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.UV = vsInput.UV;

    return output;
}

void PSMain_Depth(VSOutput psInput)
{
}

float Shadow_Base(float4 vertexPos, float depth)
{
    float3 projPos = vertexPos.xyz / vertexPos.z;

    projPos = projPos * 0.5f + 0.5f;

    float shadow = projPos.z > depth ? 1.0f : 0.0f;

    return shadow;
}

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
	output.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    Material material = ApplyMaterial(RawMat, psInput);
    float3 lightingColor = PointLighting(Light, psInput, EyePos.xyz, material);
    ///float4 Color = ambient + (1.0f - Shadow_Base(psInput.PosL, depthValue)) * (diffuse + specular);

    return float4(lightingColor, 1.0f);
}

VSOutput VSMain_Quad(VSInput vsInput)
{
    VSOutput output;
    output.PosH = float4(vsInput.Pos, 1.0f);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain_Quad(VSOutput psInput) : SV_Target
{
    float depthValue = DepthMap.Sample(Sampler, psInput.UV).r;
	return float4(depthValue, depthValue, depthValue, 1.0f);
}