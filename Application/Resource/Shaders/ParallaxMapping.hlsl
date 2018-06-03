#include "TurnOnTheLight.hlsli"

cbuffer cbVS
{
	matrix World;
	matrix WorldInverse;
    matrix WVP;
	matrix UVTransform;
};

cbuffer cbPS
{
	float3 EyePos;
    float HeightScale;

	DirectionalLight DirLight;
	Material RawMat;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;
Texture2D HeightMap;
SamplerState LinearSampler;

Material ApplyMaterial(in Material materialIn, in VSOutput psInput)
{
    Material materialOut = materialIn;
    materialOut.Diffuse = DiffuseMap.Sample(LinearSampler, psInput.UV);
    materialOut.Specular = SpecularMap.Sample(LinearSampler, psInput.UV);

    float3 normalMap = NormalMap.Sample(LinearSampler, psInput.UV).rgb;
    materialOut.Normal.xyz = normalize(UnpackNormal(normalMap, normalize(psInput.NormalW), normalize(psInput.TangentW)));
    materialOut.Normal.w = 0.0f;

    return materialOut;
}

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

float4 PSMain_NormalMapping(VSOutput psInput) : SV_Target
{
    Material material = ApplyMaterial(RawMat, psInput);
    float3 lightingColor = DirectionalLighting(DirLight, psInput, EyePos.xyz, material);

	return float4(lightingColor, 1.0f);
}

float2 ParallaxOcclusionMapping(float3 vertexPos, float2 vertexUV, float heightScale)
{
    float heightValue = HeightMap.Sample(LinearSampler, vertexUV).r;
    float2 vertexXY = vertexPos.xy / vertexPos.z;
    float2 uv = vertexUV + heightScale * heightValue * vertexXY;

    return uv;
}

float2 ParallaxMappingWithOffsetLimit(VSOutput psInput, float3 viewDirT, float2 vertexUV, float heightScale)
{
    const float s_HeightBias = 0.01f;

    float heightMapValue = HeightMap.Sample(LinearSampler, vertexUV).r;

    float height = heightMapValue * heightScale + s_HeightBias;
    height /= viewDirT.z;

    float2 uv = vertexUV + viewDirT.xy * height;

    return uv;
}

float4 PSMain_ParallaxOcclusionMapping(VSOutput psInput) : SV_Target
{
    VSOutput input = psInput;
    input.UV = ParallaxOcclusionMapping(psInput.PosW, psInput.UV, 0.01f);
    Material material = ApplyMaterial(RawMat, input);
	
    float3 lightingColor = DirectionalLighting(DirLight, psInput, EyePos.xyz, material);

    return float4(lightingColor, 1.0f);
}

float4 PSMain_ParallaxMappingWithOffsetLimit(VSOutput psInput) : SV_Target
{
    VSOutput input = psInput;

    float3 N = psInput.NormalW;
    float3 T = normalize(psInput.TangentW - dot(psInput.TangentW, N) * N);
    float3 B = cross(N, T);
    float3x3 TBN = float3x3(T, B, N);
    float3 viewDirT = normalize(mul(TBN, EyePos - psInput.PosW));
    float3 lightDirT = normalize(mul(-DirLight.Direction.xyz, TBN));

    input.UV = ParallaxMappingWithOffsetLimit(psInput, viewDirT, psInput.UV, HeightScale);

    Material material = ApplyMaterial(RawMat, input);
	
    DirectionalLight dirLight = DirLight;
    dirLight.Direction = float4(lightDirT, 1.0f);
    float3 lightingColor = DirectionalLighting(dirLight, psInput, viewDirT, material);

    return float4(lightingColor, 1.0f);
}