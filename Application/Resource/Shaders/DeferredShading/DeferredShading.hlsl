#include "../TurnOnTheLight.hlsli"

cbuffer CBufferVS
{
	matrix World;
    matrix WVP;
};

cbuffer CBufferPS
{
	float4 LightSpecular;
};

struct VSIn
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct VSOut
{
    float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD1;
	float2 UV : TEXCOORD0;
};

struct GBufferOut
{
	float4 ColorSpecInt : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 SpecPow : SV_TARGET2;
};

Texture2D DiffuseTex;
SamplerState LinearSampler;

static const float2 g_SpecPowerRange = { 10.0, 250.0 };

VSOut VSMain(VSIn vsInput)
{
    VSOut vsOutput;

    vsOutput.Pos = mul(vsInput.Pos, WVP);

    vsOutput.UV = vsInput.UV;

	vsOutput.Normal = mul(vsInput.Normal, (float3x3)World);

    return vsOutput;
}

GBufferOut PSMain(VSOut psInput)
{
    float4 diffuseClr = DiffuseTex.Sample(LinearSampler, psInput.UV);

	/// Pack Geometry Buffer

	/// Normalize the specular power
	float specPowerNorm = max(0.0001f, (LightSpecular.w - g_SpecPowerRange.x) / g_SpecPowerRange.y);
	
	GBufferOut gOut;
	gOut.ColorSpecInt = float4(diffuseClr.rgb, LightSpecular.x);

	float3 normal = normalize(psInput.Normal);
	gOut.Normal = float4(normal * 0.5f + 0.5f, 0.0f);

	gOut.SpecPow = float4(specPowerNorm, 0.0f, 0.0f, 0.0f);

	return gOut;
}