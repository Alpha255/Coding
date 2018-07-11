#include "TurnOnTheLight.hlsli"

struct VSOut
{
    float4 PosH : SV_POSITION;
    float4 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD0;
};

struct LightParams
{
    matrix VP;

    float4 Position;
    float4 Direction;
    float4 Color;
    float4 Falloff;
};

cbuffer cbVS
{
    matrix World;
	matrix WorldInverse;
    matrix VP;
};

cbuffer cbPS
{
    float4 MirrorPlane;
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

Texture2D DiffuseMap;
Texture2D NormalMap;
Texture2D ShadowMap;
SamplerState LinearSampler;

float4 LightingColor(uint iLight, float3 vertexPos, float3 vertexNormal)
{
    LightParams light = Lights[iLight];    
    float3 vLightToPixelUnNormalized = vertexPos - light.Position.xyz;
    
    /// Dist falloff = 0 at vFalloffs.x, 1 at vFalloffs.x - vFalloffs.y
    float fDist = length(vLightToPixelUnNormalized);
    float fDistFalloff = saturate((light.Falloff.x - fDist) / light.Falloff.y);
    
    /// Normalize from here on
    float3 vLightToPixelNormalized = vLightToPixelUnNormalized / fDist;
    
    /// Angle falloff = 0 at vFalloffs.z, 1 at vFalloffs.z - vFalloffs.w
    float fCosAngle = dot(vLightToPixelNormalized, normalize(light.Direction.xyz));
    float fAngleFalloff = saturate((fCosAngle - light.Falloff.z) / light.Falloff.w);
    
    /// Diffuse contribution
    float fNDotL = saturate(-dot(vLightToPixelNormalized, vertexNormal));
    
    return light.Color * fNDotL * fDistFalloff * fAngleFalloff;
}

///--------------------------------------------------------------------------------------
/// Test how much pixel is in shadow, using 2x2 percentage-closer filtering
///--------------------------------------------------------------------------------------
float4 CalcUnshadowedAmountPCF2x2(int iShadow, float4 vertexPos)
{
    const float shadowDepthBias = 0.0005f;
    const float shadowMapDims = 2048.0f;

	/// Compute pixel position in light space
    float4 vertexInLightSpace = mul(vertexPos, Lights[iShadow].VP);
    vertexInLightSpace.xyz /= vertexInLightSpace.w;

	/// Translate from surface coords to texture coords 
	/// Could fold these into the matrix
    float2 shadowTexCoord = 0.5f * vertexInLightSpace + 0.5f;
    shadowTexCoord.y = 1.0f - shadowTexCoord.y;

	/// Depth bias to avoid pixel self-shadowing
    float lightSpaceDepth = vertexInLightSpace.z - shadowDepthBias;

	/// Find sub-pixel weights
    float2 vShadowMapDims = float2(shadowMapDims, shadowMapDims);
    float4 subPixelCoord;
    subPixelCoord.xy = frac(vShadowMapDims * shadowTexCoord);
    subPixelCoord.zw = 1.0f - subPixelCoord;

    float4 bilinearWeights = subPixelCoord.zxzx * subPixelCoord.wwyy;

	/// 2x2 percentage closer filtering
    float2 texelUnits = 1.0f / vShadowMapDims;
    float4 shadowDepth;
    shadowDepth.x = ShadowMap.Sample(LinearSampler, shadowTexCoord);
    shadowDepth.y = ShadowMap.Sample(LinearSampler, shadowTexCoord + float2(texelUnits.x, 0.0f));
    shadowDepth.z = ShadowMap.Sample(LinearSampler, shadowTexCoord + float2(0.0f, texelUnits.y));
    shadowDepth.w = ShadowMap.Sample(LinearSampler, shadowTexCoord + texelUnits);

	/// What weighted fraction of the 4 samples are nearer to the light than this pixel?
    float4 shadowTest = (shadowDepth >= lightSpaceDepth) ? 1.0f : 0.0f;

    return dot(bilinearWeights, shadowTest);
}

// We aliased signed vectors as a unsigned format. 
// Need to recover signed values.  The values 1.0 and 2.0
// are slightly inaccurate here.
float3 R10G10B10A2_UNORM_TO_R32G32B32_FLOAT(in float3 vVec)
{
    vVec *= 2.0f;
    return vVec >= 1.0f ? ( vVec - 2.0f ) : vVec;
}

VSOut VSMain(VSInput vsInput)
{
	vsInput.Normal = R10G10B10A2_UNORM_TO_R32G32B32_FLOAT(vsInput.Normal);
	vsInput.Tangent = R10G10B10A2_UNORM_TO_R32G32B32_FLOAT(vsInput.Tangent);

	matrix wvp = mul(World, VP);

    VSOut output;
	output.PosH = mul(vsInput.Pos, wvp);
    output.PosW = mul(vsInput.Pos, World);
    output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain(VSOut psInput) : SV_Target
{
	/// Manual clip test, so that objects which are behind the mirror 
    /// don't show up in the mirror.
    clip(dot(MirrorPlane.xyz, psInput.PosW.xyz) + MirrorPlane.w);

	float4 normalMap = NormalMap.Sample(LinearSampler, psInput.UV);
	float4 diffuse = DiffuseMap.Sample(LinearSampler, psInput.UV);
	float3 normal = UnpackNormal(normalMap.xyz, normalize(psInput.NormalW), psInput.TangentW);

	float3 lightingClr = float3(0.0f, 0.0f, 0.0f);

	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
        lightingClr += LightingColor(i, psInput.PosW.xyz, normalize(normal));

        ///if (i == 0 && any(lightingClr) > 0.0f)
        ///{
        ///    lightingClr *= CalcUnshadowedAmountPCF2x2(i, psInput.PosW);
        ///}
    }

    return diffuse * TintColor * float4(lightingClr, 0.0f) + AmbientColor;
}