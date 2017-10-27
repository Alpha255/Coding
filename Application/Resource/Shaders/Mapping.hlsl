#include "CommonLighting.hlsli"

cbuffer cbVS
{
    matrix World;
    matrix WorldInverse;
    matrix VP;
    matrix WVP;
    matrix TexTransform;
};

cbuffer cbPS
{
    DirectionalLight DirLights[3];
    Material Mat;

    float EyePos;
    float HeightScale;

    float MinTessDistance;
    float MaxTessDistance;
    float MinTessFactor;
    float MaxTessFactor;
};

Texture2D DiffuseTex;
Texture2D NormalmapTex;
TextureCube CubemapTex;

SamplerState SamplerLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct VSOutput_NormalMap
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD;
};

struct VSOutput_DisplacementMap
{
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD;
    float TessFactor : TESS;
};

struct PatchTess
{
    float EdgeTess[3] : SV_TessFactor;
    float InsideTess : SV_InsideTessFactor;
};

struct HSOutput
{
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD;
};

struct DSOutput
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD;
};

VSOutput_NormalMap VS_Main_NormalMap(VSInput input)
{
    VSOutput_NormalMap output;

    output.PosW = mul(float4(input.Pos, 1.0f), World).xyz;
    output.NormalW = mul(input.Normal, (float3x3)WorldInverse);
    output.TangentW = mul(input.Tangent, (float3x3)World);

    output.PosH = mul(float4(input.Pos, 1.0f), WVP);

    ///output.UV = input.UV;
    output.UV = mul(float4(input.UV, 0.0f, 1.0f), TexTransform).xy;

    return output;
}

float4 PS_Main_NormalMap(VSOutput_NormalMap input) : SV_Target
{
    input.NormalW = normalize(input.NormalW);

    float3 toEye = EyePos - input.PosW;

    float disToEye = length(toEye);

    ///toEye = normalize(toEye);
    toEye /= disToEye;

    float4 texClr = float4(1.0f, 1.0f, 1.0f, 1.0f);
    texClr = DiffuseTex.Sample(SamplerLinear, input.UV);
    clip(texClr.a - 0.1f);


	/// Normal mapping
    float3 normalMapSample = NormalmapTex.Sample(SamplerLinear, input.UV).rgb;
    float3 bumpedNormal = NormalSampleToWorldSpace(normalMapSample, input.NormalW, input.TangentW);

	/// Lighting
    float4 litClr = texClr;

    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll]
    for (int i = 0; i < 3; ++i)
    {
        float4 A, D, S;
        ComputeDirectionalLight(Mat, DirLights[i], bumpedNormal, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        spec += S;
    }

    litClr = texClr * (ambient + diffuse) + spec;

	/// Reflection
    float3 reflectionVector = reflect(-toEye, bumpedNormal);
    float4 reflectionColor = CubemapTex.Sample(SamplerLinear, reflectionVector);
    litClr += Mat.Reflect * reflectionColor;

	/// Common to take alpha from diffuse material and texture.
    litClr.a = Mat.Diffuse.a * texClr.a;

    return litClr;
}

VSOutput_DisplacementMap VS_Main_DisplacementMap(VSInput input)
{
    VSOutput_DisplacementMap output;

    output.PosW = mul(float4(input.Pos, 1.0f), World).xyz;
    output.NormalW = mul(input.Normal, (float3x3)WorldInverse);
    output.TangentW = mul(input.Tangent, (float3x3)World);

    output.UV = mul(float4(input.UV, 0.0f, 1.0f), TexTransform).xy;

    float d = distance(output.PosW, EyePos);

	/// Normalized tessellation factor. 
	/// The tessellation is 
	///   0 if d >= MinTessDistance and
	///   1 if d <= MaxTessDistance.  
    float tess = saturate((MinTessDistance - d) / (MinTessDistance - MaxTessDistance));

	/// Rescale [0,1] --> [gMinTessFactor, gMaxTessFactor].
    output.TessFactor = MinTessFactor + tess * (MaxTessFactor - MinTessFactor);

    return output;
}

PatchTess PatchHS(InputPatch<VSOutput_DisplacementMap, 3> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess pt;

	/// Average tess factors along edges, and pick an edge tess factor for 
	/// the interior tessellation.  It is important to do the tess factor
	/// calculation based on the edge properties so that edges shared by 
	/// more than one triangle will have the same tessellation factor.  
	/// Otherwise, gaps can appear.
    pt.EdgeTess[0] = 0.5f * (patch[1].TessFactor + patch[2].TessFactor);
    pt.EdgeTess[1] = 0.5f * (patch[2].TessFactor + patch[0].TessFactor);
    pt.EdgeTess[2] = 0.5f * (patch[0].TessFactor + patch[1].TessFactor);
    pt.InsideTess = pt.EdgeTess[0];
	
    return pt;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchHS")]
HSOutput HS_Main_DisplacementMap(InputPatch<VSOutput_DisplacementMap, 3> patch, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HSOutput output;

    /// Pass through shader.
    output.PosW = patch[i].PosW;
    output.NormalW = patch[i].NormalW;
    output.TangentW = patch[i].TangentW;
    output.UV = patch[i].UV;
	
    return output;
}

/// The domain shader is called for every vertex created by the tessellator.  
/// It is like the vertex shader after tessellation.
[domain("tri")]
DSOutput DS_Main_DisplacementMap(PatchTess patchTess, float3 bary : SV_DomainLocation, const OutputPatch<HSOutput, 3> tri)
{
    DSOutput output;

	/// Interpolate patch attributes to generated vertices.
    output.PosW = bary.x * tri[0].PosW + bary.y * tri[1].PosW + bary.z * tri[2].PosW;
    output.NormalW = bary.x * tri[0].NormalW + bary.y * tri[1].NormalW + bary.z * tri[2].NormalW;
    output.TangentW = bary.x * tri[0].TangentW + bary.y * tri[1].TangentW + bary.z * tri[2].TangentW;
    output.UV = bary.x * tri[0].UV + bary.y * tri[1].UV + bary.z * tri[2].UV;

	/// Interpolating normal can unnormalize it, so normalize it.
    output.NormalW = normalize(output.NormalW);

	/// Displacement mapping
	/// Choose the mipmap level based on distance to the eye; specifically, choose
	/// the next miplevel every MipInterval units, and clamp the miplevel in [0,6].
    float mipInterval = 20.0f;
    float mipLevel = clamp((distance(output.PosW, EyePos) - mipInterval) / mipInterval, 0.0f, 6.0f);

	/// Sampler height map (stored in alpha channel)
    float h = NormalmapTex.SampleLevel(SamplerLinear, output.UV, mipLevel).a;

	/// Offset vertex along normal
    output.PosW += (HeightScale * (h - 1.0f)) * output.NormalW;

	/// Project to homogeneous clip space
    output.PosH = mul(float4(output.PosW, 1.0f), VP);

    return output;
}

float4 PS_Main_DisplacementMap(DSOutput input) : SV_Target
{
    input.NormalW = normalize(input.NormalW);

    float3 toEye = EyePos - input.PosW;

    float disToEye = length(toEye);

    toEye /= disToEye;

    float4 texClr = DiffuseTex.Sample(SamplerLinear, input.UV);
    clip(texClr.a - 0.1f);

	/// Normal mapping
    float3 normalMapSample = NormalmapTex.Sample(SamplerLinear, input.UV).rgb;
    float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, input.NormalW, input.TangentW);

	/// Lighting
    float4 litClr = texClr;
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll]
    for (int i = 0; i < 3; ++i)
    {
        float4 A, D, S;
        ComputeDirectionalLight(Mat, DirLights[i], bumpedNormalW, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        spec += S;
    }

    litClr = texClr * (ambient + diffuse) + spec;

	/// Reflection
    float3 incident = -toEye;
    float3 reflectionVector = reflect(incident, bumpedNormalW);
    float4 reflectionClr = CubemapTex.Sample(SamplerLinear, reflectionVector);
    litClr += Mat.Reflect * reflectionClr;


    litClr.a = Mat.Diffuse.a * texClr.a;

    return litClr;
}