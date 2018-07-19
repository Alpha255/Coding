#include "Common.hlsli"
#include "../TurnOnTheLight.hlsli"

cbuffer cbGBufferUnpack
{
	float4 PerspectiveValue;
}

SamplerState PointSampler;

struct VSOut
{
	float4 Position : SV_Position;
	float4 SampleMask : TEXCOORD0;
	float2 UV : TEXCOORD1;
};

SurfaceData UnpackGBuffer(float2 uv)
{
	SurfaceData surfaceOut;

	float depth = DepthTexture.Sample(PointSampler, uv).r;
	surfaceOut.LinearDepth = ConvertToLinearDepth(depth, PerspectiveValue);

	float4 baseClrSpecInt = ColorSpecIntTexture.Sample(PointSampler, uv);
	surfaceOut.Color = baseClrSpecInt.rgb;
	surfaceOut.SpecIntensity = baseClrSpecInt.a;

	float3 normal = NormalTexture.Sample(PointSampler, uv).rgb;
	surfaceOut.Normal = normalize(normal * 2.0f - 1.0f);

	surfaceOut.SpecPow = SpecPowTexture.Sample(PointSampler, uv).r;

	return surfaceOut;
}

static float2 g_Offsets[4] = 
{
	float2(-0.75, -0.75),
	float2(-0.25, -0.75),
	float2(0.25, -0.75),
	float2(0.75, -0.75),
};

static const float2 g_Positions[4] = 
{
	float2(1.0, 1.0),
	float2(1.0, -1.0),
	float2(-1.0, 1.0),
	float2(-1.0, -1.0),
};

static const float2 g_UVs[4] = 
{
	float2(1.0, 0.0),
	float2(1.0, 1.0),
	float2(0.0, 0.0),
	float2(0.0, 1.0),
};

static const float4 g_Masks[4] = 
{
	float4(1.0, 0.0, 0.0, 0.0),
	float4(0.0, 1.0, 0.0, 0.0),
	float4(0.0, 0.0, 1.0, 0.0),
	float4(0.0, 0.0, 0.0, 1.0),
};

VSOut VSMain(uint VertexID : SV_VertexID)
{
	VSOut output;
	
	output.Position = float4(g_Positions[VertexID % 4].xy * 0.2f + g_Offsets[VertexID / 4], 0.0f, 1.0f);
	output.UV = g_UVs[VertexID % 4];
	output.SampleMask = g_Masks[VertexID / 4];	

	return output;
}

float4 PSMain(VSOut psInput) : SV_TARGET
{
	SurfaceData surface = UnpackGBuffer(psInput.UV);

	float4 result = float4(0.0f, 0.0f, 0.0f, 1.0f);
	result += float4(1.0f - saturate(surface.LinearDepth / 75.0f), 
		             1.0f - saturate(surface.LinearDepth / 125.0f), 
	                 1.0f - saturate(surface.LinearDepth / 200.0f), 
	                 0.0f) * psInput.SampleMask.xxxx;
	result += float4(surface.Color.rgb, 0.0f) * psInput.SampleMask.yyyy;
	result += float4(surface.Normal.xyz * 0.5f + 0.5f, 0.0f) * psInput.SampleMask.zzzz;
	result += float4(surface.SpecIntensity, surface.SpecPow, 0.0f, 0.0f) * psInput.SampleMask.wwww;

	return float4(GammaCorrection(result.rgb), 1.0f);
}