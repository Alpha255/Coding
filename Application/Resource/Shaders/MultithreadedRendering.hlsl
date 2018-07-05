#include "TurnOnTheLight.hlsli"

struct LightParams
{
    matrix WVP;

    float4 Position;
    float4 Direction;
    float4 Color;
    float4 Falloff;
};

cbuffer cbVS
{
    matrix WVP;
    matrix World;
    matrix WorldInverse;
}

cbuffer cbPS
{
	float4 TintColor;

    float4 AmbientColor;
    LightParams Lights[4];
};

struct VSInput
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
	float2 UV : TEXCOORD;
    float3 Tangent : TANGENT;
};

Texture2D DiffuseTex;
Texture2D NormalTex;
SamplerState LinearSampler;

float4 LightingColor(uint iLight, float3 vertexPos, float3 vertexNormal)
{
    LightParams light = Lights[iLight];    
    float3 vLightToPixelUnNormalized = vertexPos - light.Position.xyz;
    
    // Dist falloff = 0 at vFalloffs.x, 1 at vFalloffs.x - vFalloffs.y
    float fDist = length(vLightToPixelUnNormalized);
    float fDistFalloff = saturate((light.Falloff.x - fDist) / light.Falloff.y);
    
    // Normalize from here on
    float3 vLightToPixelNormalized = vLightToPixelUnNormalized / fDist;
    
    // Angle falloff = 0 at vFalloffs.z, 1 at vFalloffs.z - vFalloffs.w
    float fCosAngle = dot(vLightToPixelNormalized, normalize(light.Direction.xyz));
    float fAngleFalloff = saturate((fCosAngle - light.Falloff.z) / light.Falloff.w);
    
    // Diffuse contribution
    float fNDotL = saturate(-dot(vLightToPixelNormalized, vertexNormal));
    
    return light.Color * fNDotL * fDistFalloff * fAngleFalloff;
}

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.PosH = mul(vsInput.Pos, WVP);
    output.PosW = mul(vsInput.Pos, World).xyz;
    output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
	float4 normalMap = NormalTex.Sample(LinearSampler, psInput.UV);
	float4 diffuse = DiffuseTex.Sample(LinearSampler, psInput.UV);
	float3 normal = UnpackNormal(normalMap.xyz, normalize(psInput.NormalW), psInput.TangentW);

	float3 lightingClr = float3(0.0f, 0.0f, 0.0f);

	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
        lightingClr += LightingColor(i, psInput.PosW, normalize(normal));
    }

    return diffuse * TintColor * float4(lightingClr, 0.0f) + AmbientColor;
}