#if 0
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
Texture2D NormalMap;
Texture2D SpecularMap;
SamplerState LinearSampler;

struct VSOutput
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD;
};

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
    Material material;
    ///material.Ambient = float4(0.1f, 0.1f, 0.1f, 1.0f);
    material.Diffuse = DiffuseMap.Sample(LinearSampler, psInput.UV);
    material.Specular = SpecularMap.Sample(LinearSampler, psInput.UV);

    float3 texNormal = NormalMap.Sample(LinearSampler, psInput.UV);
    float3 normal = UnpackNormal(texNormal, psInput.NormalW, psInput.TangentW);

    float4 ambient, diffuse, specular;
    ComputePointLight(material, Light, normalize(normal), psInput.PosW, EyePos.xyz, ambient, diffuse, specular);

    float4 Color = ambient + diffuse  + specular;

    return float4(Color.rgb, 1.0f);
}
#endif

#include "TurnOnTheLight.hlsli"

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

cbuffer cbVS
{
    matrix World;
    matrix WorldInverse;
    matrix WVP;
};

cbuffer cbPS
{
	float4 EyePos;

    PointLight Light;

	Material MatIn;
};

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
	float3 lightingClr = PointLighting(Light, psInput, EyePos.xyz, MatIn);

	return float4(lightingClr, 1.0f);
}