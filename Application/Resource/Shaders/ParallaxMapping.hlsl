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
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
	output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
	output.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
	output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.UV = mul(float4(vsInput.UV, 0.0f, 1.0f), UVTransform).xy;

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
    ///float3 normal = normalize(psInput.NormalW);
	float3 normal = normalize(NormalMap.Sample(LinearSampler, psInput.UV).rgb);
	float3 toEye = normalize(EyePos.xyz - psInput.PosW);

    float4 texClr = DiffuseMap.Sample(LinearSampler, psInput.UV);
    clip(texClr.a - 0.1f);

    float4 ambient, diffuse, specular;
	Material matCopy = Mat;
	matCopy.Specular = SpecularMap.Sample(LinearSampler, psInput.UV);
	ComputeDirectionalLight(matCopy, DirLight, psInput.NormalW, toEye, ambient, diffuse, specular);

    float4 litClr = texClr * (ambient + diffuse + specular);
	litClr.a = Mat.Diffuse.a * texClr.a;

    return litClr;
}