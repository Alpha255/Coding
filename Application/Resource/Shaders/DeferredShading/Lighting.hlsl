#include "../TurnOnTheLight.hlsli"
#include "Common.hlsli"

cbuffer CBufferPS
{
	float4 AmbientLowerClr;
	float4 AmbientRange;

	float4 Perpective;

	matrix ViewInverse;

	DirectionalLight DirLight;

	PointLight PointLights[3];
};

struct VSOut
{
    float4 Position : SV_POSITION;
	float2 BasePosXY : TEXCOORD0;
};

SamplerState LinearSampler;

static const float2 g_VertexPos[4] = 
{
	float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0),
};

float3 AmbientLighting(float3 normal, float3 color)
{
	float up = normal.y * 0.5f + 0.5f;
	
	float3 ambient = AmbientLowerClr.rgb + up * AmbientRange.rgb;

	return ambient * color;
}

SurfaceData UnpackGBuffer(int2 vertexPos)
{
	SurfaceData outSurface;
	
	int3 location = int3(vertexPos, 0);

	float depth = DepthTexture.Load(location).r;
	outSurface.LinearDepth = ConvertToLinearDepth(depth, Perpective);

	float4 baseClrSpecInt = ColorSpecIntTexture.Load(location);
	outSurface.Color = baseClrSpecInt.rgb;
	outSurface.SpecIntensity = baseClrSpecInt.a;

	float3 normal = NormalTexture.Load(location).rgb;
	outSurface.Normal = normalize(normal * 2.0f - 1.0f);

	outSurface.SpecPow = SpecPowTexture.Load(location).r;

	return outSurface;
}

void ConstructMaterialFromGBuffer(SurfaceData surface, inout Material mat)
{
	mat.Normal = float4(surface.Normal, 0.0f);
	mat.Diffuse.rgb = surface.Color;
	mat.Diffuse.a = 1.0f;
	mat.Specular.rgb = float3(surface.SpecIntensity, surface.SpecIntensity, surface.SpecIntensity);
	mat.Specular.a = 1.0f;
	mat.Reflection = float4(0.0f, 0.0f, 0.0f, 0.0f);
}

float3 GetWorldPos(float2 basePosXY, float linearDepth)
{
	float4 pos;

	pos.xy = basePosXY * Perpective.xy * linearDepth;
	pos.z = linearDepth;
	pos.w = 1.0f;
	
	return mul(pos, ViewInverse).xyz;
}

VSOut VSMain(uint VertexID : SV_VertexID)
{
    VSOut vsOutput;

	vsOutput.Position = float4(g_VertexPos[VertexID], 0.0f, 1.0f);
	vsOutput.BasePosXY = g_VertexPos[VertexID];

    return vsOutput;
}

float4 PSMain(VSOut psInput) : SV_TARGET
{
	SurfaceData surface = UnpackGBuffer(psInput.Position.xy);

	Material mat;
	ConstructMaterialFromGBuffer(surface, mat);

	float3 pos = GetWorldPos(psInput.BasePosXY, surface.LinearDepth);

	float3 result = AmbientLighting(mat.Normal.xyz, mat.Diffuse.rgb);

	result += DirectionalLighting(DirLight, pos, ViewInverse[3].xyz, mat);

	result = GammaCorrection(result);

	return float4(result, 1.0f);
}