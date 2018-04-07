cbuffer ConstantsBuf
{
    matrix WVP;
    matrix VP;
    matrix World;
    matrix WorldInverse;

    float3 EyePos;
    float HeightScale;

    float MinTessFactor;
    float MaxTessFactor;
    float MinTessDistance;
    float MaxTessDistance;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    ///float3 Pos : SV_POSITION;
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
    float TessFactor : TESS;
};

struct TessPatch
{
    float EdgeFactor[3] : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
};

struct HSOutput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct DSOutput
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

Texture2D DiffuseMap;
Texture2D HeightMap;
SamplerState SamplerLinear;

VSOutput VS_Main(VSInput input)
{
    VSOutput output;
    output.Pos = mul(float4(input.Pos, 1.0f), World).xyz;
    output.Normal = mul(input.Normal, (float3x3)WorldInverse);
    output.Tangent = mul(input.Tangent, (float3x3)World);
    output.UV = input.UV;

    float dis = distance(output.Pos, EyePos);

    float tess = saturate((MinTessDistance - dis) / (MinTessDistance - MaxTessDistance));

    output.TessFactor = MinTessFactor + tess * (MaxTessFactor - MinTessFactor);

    return output;
}

TessPatch PatchFunc(InputPatch<VSOutput, 3> patch_in, uint patchID : SV_PrimitiveID)
{
    TessPatch patch_out;

    patch_out.EdgeFactor[0] = 0.5f * (patch_in[1].TessFactor + patch_in[2].TessFactor);
    patch_out.EdgeFactor[1] = 0.5f * (patch_in[2].TessFactor + patch_in[0].TessFactor);
    patch_out.EdgeFactor[2] = 0.5f * (patch_in[0].TessFactor + patch_in[1].TessFactor);
    patch_out.InsideFactor = patch_out.EdgeFactor[0];

    return patch_out;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchFunc")]
HSOutput HS_Main(InputPatch<VSOutput, 3> patch_in, uint controlPointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HSOutput output;

    output.Pos = patch_in[controlPointID].Pos;
    output.Normal = patch_in[controlPointID].Normal;
    output.Tangent = patch_in[controlPointID].Tangent;
    output.UV = patch_in[controlPointID].UV;
	
    return output;
}

[domain("tri")]
DSOutput DS_Main(TessPatch tessPatch, float3 bary : SV_DomainLocation, const OutputPatch<HSOutput, 3> triangles)
{
    DSOutput output;

    output.PosW = bary.x * triangles[0].Pos + bary.y * triangles[1].Pos + bary.z * triangles[2].Pos;
    output.Normal = bary.x * triangles[0].Normal + bary.y * triangles[1].Normal + bary.z * triangles[2].Normal;
    output.Tangent = bary.x * triangles[0].Tangent + bary.y * triangles[1].Tangent + bary.z * triangles[2].Tangent;
    output.UV = bary.x * triangles[0].UV + bary.y * triangles[1].UV + bary.z * triangles[2].UV;

    output.Normal = normalize(output.Normal);

    const float mipInterval = 20.0f;
    float mipLevel = clamp((distance(output.PosW, EyePos) - mipInterval) / mipInterval, 0.0f, 6.0f);

    float h = HeightMap.SampleLevel(SamplerLinear, output.UV, mipLevel).r;

    output.PosW += (HeightScale * (h - 1.0f)) * output.Normal;

    output.PosH = mul(float4(output.PosW, 1.0f), VP);

    return output;
}

float4 PS_Main(DSOutput input) : SV_Target
{
    float4 clr = DiffuseMap.Sample(SamplerLinear, input.UV);

    return clr;
}