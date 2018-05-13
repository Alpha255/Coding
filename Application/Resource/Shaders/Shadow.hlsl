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
    PointLight Light;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

Texture2D DiffuseMap;
Texture2D DepthMap;
SamplerState LinearSampler;

struct VSOutput
{
    float4 PosL : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 UV : TEXCOORD;
};

VSOutput VSMain_Depth(VSInput vsInput)
{
	VSOutput output;
    output.PosL = mul(float4(vsInput.Pos, 1.0f), WVP);
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
    output.PosL = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
    output.NormalW = mul(vsInput.Normal, (float3x3) WorldInverse);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    float3 normal = normalize(psInput.NormalW);

    Material matCopy = Mat;
    matCopy.Diffuse = DiffuseMap.Sample(LinearSampler, psInput.UV);

    float depthValue = DepthMap.Sample(LinearSampler, psInput.UV).r;

    float4 ambient, diffuse, specular;
    ComputePointLight(matCopy, Light, normal, psInput.PosW, EyePos.xyz, ambient, diffuse, specular);

    float4 Color = ambient + diffuse + specular;
    ///float4 Color = ambient + (1.0f - Shadow_Base(psInput.PosL, depthValue)) * (diffuse + specular);
    Color.w = Mat.Diffuse.w;

    return Color;
}

VSOutput VSMain_Quad(VSInput vsInput)
{
    VSOutput output;
    output.PosL = float4(vsInput.Pos, 1.0f);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain_Quad(VSOutput psInput) : SV_Target
{
    float depthValue = DepthMap.Sample(LinearSampler, psInput.UV).r;
	return float4(depthValue, depthValue, depthValue, 1.0f);
}