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

    DirectionalLight DirLight;
    Material RawMat;
};

Texture2D DiffuseMap;
TextureCube CubeMap;
SamplerState LinearSampler;

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
    Material material = ApplyMaterial(RawMat, psInput);
    float3 lightingColor = DirectionalLighting(DirLight, psInput.PosW, EyePos.xyz, material);
	lightingColor += DirLight.Ambient.rgb;

    float3 reflectionVector = reflect(-EyePos.xyz, psInput.NormalW);
    float4 reflectionColor = CubeMap.Sample(LinearSampler, reflectionVector);

    lightingColor += material.Reflection * reflectionColor;

    return float4(lightingColor, 1.0f);
}