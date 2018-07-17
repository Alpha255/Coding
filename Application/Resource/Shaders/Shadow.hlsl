#include "TurnOnTheLight.hlsli"

Texture2D DiffuseMap;
Texture2D DepthMap;
SamplerState LinearSampler;

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

	matrix LightSpaceVP;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct VSOut
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD0;
};

Material ApplyMaterial(in Material materialIn, in VSOut psInput)
{
    Material materialOut = materialIn;

    materialOut.Diffuse = float4(1.0, 1.0f, 1.0f, 1.0f);

    materialOut.Normal = float4(normalize(psInput.NormalW), 0.0f);

    return materialOut;
}

float Shadow_Base(float4 vertexPos)
{
	float4 vertexPosInLightSpace = mul(vertexPos, LightSpaceVP);

    float3 projPosUV = vertexPosInLightSpace.xyz / vertexPosInLightSpace.w;

    projPosUV = projPosUV * 0.5f + 0.5f;

	float closetDepth = DepthMap.Sample(LinearSampler, projPosUV.xy).r;
	float currentDepth = projPosUV.z;

    float shadow = currentDepth > closetDepth ? 1.0f : 0.0f;

    return shadow;
}

VSOut VSMain(VSInput vsInput)
{
    VSOut output;
    output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.PosW = mul(float4(vsInput.Pos, 1.0f), World);
	output.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain(VSOut psInput) : SV_Target
{
    Material material = ApplyMaterial(RawMat, psInput);
    float4 lightingColor = float4(PointLighting(Light, psInput.PosW.xyz, EyePos.xyz, material), 1.0f);
	float4 diffuseClr = DiffuseMap.Sample(LinearSampler, psInput.UV);
    float4 finalColor = Light.Ambient + (1.0f - Shadow_Base(psInput.PosW) * lightingColor) * diffuseClr;

    return finalColor;
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
    float depthValue = DepthMap.Sample(LinearSampler, psInput.UV).r;
	return float4(depthValue, depthValue, depthValue, 1.0f);
}