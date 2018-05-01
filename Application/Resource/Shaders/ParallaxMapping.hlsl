#include "CommonLighting.hlsli"

cbuffer cbVS
{
	matrix World;
	matrix WorldInverse;
    matrix WVP;
	matrix UVTransform;
};

cbuffer cbPS
{
	DirectionalLight DirLight;
	Material Mat;

	float4 EyePos;
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
    output.UV = mul(float4(vsInput.UV, 0.0f, 1.0f), UVTransform).xy;

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    float3 normal = normalize(psInput.NormalW);
	float3 toEye = normalize(EyePos.xyz - psInput.PosW);

    float4 texClr = DiffuseMap.Sample(LinearSampler, psInput.UV);
    clip(texClr.a - 0.1f);

	float4 specularTex = SpecularMap.Sample(LinearSampler, psInput.UV);
    float4 spec = specularTex.g;

    Material matCopy = Mat;
    matCopy.Specular = spec;

    float4 ambient, diffuse, specular;
    ComputeDirectionalLight(matCopy, DirLight, normal, toEye, ambient, diffuse, specular);

    float4 litClr = texClr * (ambient + diffuse) + specular;
	litClr.a = Mat.Diffuse.a * texClr.a;

    return litClr;
}

float4 PSMain_NormalMapping(VSOutput psInput) : SV_Target
{
    float3 normalTex = normalize(NormalMap.Sample(LinearSampler, psInput.UV).rgb);
    float3 toEye = normalize(EyePos.xyz - psInput.PosW);

    float4 texClr = DiffuseMap.Sample(LinearSampler, psInput.UV);
    clip(texClr.a - 0.1f);

    float4 specularTex = SpecularMap.Sample(LinearSampler, psInput.UV);
    float4 spec = specularTex.g;

    Material matCopy = Mat;
    matCopy.Specular = spec;

    float3 bumpNormal = UnpackNormal(normalTex, psInput.NormalW, psInput.TangentW);

    float4 ambient, diffuse, specular;
    ComputeDirectionalLight(matCopy, DirLight, bumpNormal, toEye, ambient, diffuse, specular);

    float4 litClr = texClr * (ambient + diffuse) + specular;
    litClr.a = Mat.Diffuse.a * texClr.a;

    return litClr;
}