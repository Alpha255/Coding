#include "TurnOnTheLight.hlsli"

cbuffer cbVS
{
	matrix World;
	matrix WorldInverse;
    matrix WVP;
	matrix UVTransform;

	float4 EyePosVS;
	float4 LightDir;
};

cbuffer cbPS
{
	float3 EyePosPS;
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

struct VSOutput_POM
{
	VSOutput VSOut;

	float3 LightDirectionT : TEXCOORD1;
	float3 ViewDirectionT : TEXCOORD2;
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

float GetMipLevel(float2 uv)
{
	float2 dx = ddx(uv);
	float2 dy = ddy(uv);
	float d = max(dot(dx, dx), dot(dy, dy));
	
	float mipLevel = 0.5f * log2(d);
	
	return max(mipLevel, 0.0f);	
}

VSOutput_POM VSMain(VSInput vsInput)
{
    VSOutput_POM output;
	output.VSOut.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
	output.VSOut.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
	output.VSOut.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.VSOut.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.VSOut.UV = mul(float4(vsInput.UV, 0.0f, 1.0f), UVTransform).xy;

	float3 N = normalize(output.VSOut.NormalW);
    float3 T = normalize(output.VSOut.TangentW - dot(output.VSOut.TangentW, N) * N);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);

	output.LightDirectionT = normalize(mul(LightDir.xyz, TBN));
    output.ViewDirectionT = normalize(mul(TBN, EyePosVS.xyz - output.VSOut.PosW));

    return output;
}

/// quadtree version
float2 ParallaxOcclusionMappingInACL(float2 srcUV, float3 viewDirT, float heightScale)
{
	float2 result = srcUV;

	float3 tangentViewDir = -(normalize(float3(viewDirT.x, - viewDirT.y, viewDirT.z)));
	float lowestMipLevel = GetMipLevel(srcUV);

	const float stepSize = 1.0f / 1024.0f;
	float3 tangentEyeVector = tangentViewDir * float3(stepSize, stepSize, stepSize / (heightScale / (1.0f + 100.0f * lowestMipLevel))); 

	float height = 0.99f;
	int numIter = 0;

	[loop]
	while (numIter < 24)
	{
		numIter += 1;

		float4 texCoords[2];
		texCoords[0] = result.xyxy + float4(1.0f, 1.0f, 2.0f, 2.0f) * tangentEyeVector.xyxy;
		texCoords[1] = result.xyxy + float4(3.0f, 3.0f, 4.0f, 4.0f) * tangentEyeVector.xyxy;

		float4 compareVal = height.xxxx + float4(1.0f, 2.0f, 3.0f, 4.0f) * tangentEyeVector.zzzz;

		float4 fetchHeight;
		fetchHeight.x = HeightMap.SampleLevel(LinearSampler, texCoords[0].xy, lowestMipLevel).r;
		fetchHeight.y = HeightMap.SampleLevel(LinearSampler, texCoords[0].zw, lowestMipLevel).r;
		fetchHeight.z = HeightMap.SampleLevel(LinearSampler, texCoords[1].xy, lowestMipLevel).r;
		fetchHeight.w = HeightMap.SampleLevel(LinearSampler, texCoords[1].zw, lowestMipLevel).r;

		bool4 testResult = fetchHeight >= compareVal;

		[branch]
		if (any(testResult))
		{
			float2 outResult = float2(0.0f, 0.0f);

			[flatten]
			if (testResult.w)
			{
				outResult = texCoords[1].xy;
			}
			
			[flatten]
			if (testResult.z)
			{
				outResult = texCoords[0].zw;
			}
						
			[flatten]
			if (testResult.y)
			{
				outResult = texCoords[0].xy;
			}
						
			[flatten]
			if (testResult.x)
			{
				outResult = result;
			}

			result = outResult;
			break;
		}

		result = texCoords[1].zw;

		height = compareVal.w;
	}

	return result;
}

//float2 ParallaxMappingWithOffsetLimit(VSOutput_POM psInput, float3 viewDirT, float2 vertexUV, float heightScale)
//{
//    const float s_HeightBias = 0.01f;

//    float heightMapValue = HeightMap.Sample(LinearSampler, vertexUV).r;

//    float height = heightMapValue * heightScale + s_HeightBias;
//    height /= viewDirT.z;

//    float2 uv = vertexUV + viewDirT.xy * height;

//    return uv;
//}

float4 PSMain_NormalMapping(VSOutput_POM psInput) : SV_Target
{
    Material material = ApplyMaterial(RawMat, psInput.VSOut);
    float3 lightingColor = DirectionalLighting(DirLight, psInput.VSOut, EyePosPS.xyz, material);

	return float4(lightingColor, 1.0f);
}

float4 PSMain_ParallaxOcclusionMappingInACL(VSOutput_POM psInput) : SV_Target
{
    VSOutput input = psInput.VSOut;
    input.UV = ParallaxOcclusionMappingInACL(psInput.VSOut.UV, psInput.ViewDirectionT, HeightScale);

    Material material = ApplyMaterial(RawMat, input);

	///DirectionalLight dirLight = DirLight;
	///dirLight.Direction = float4(normalize(psInput.LightDirectionT), 0.0f);
    ///float3 lightingColor = DirectionalLighting(dirLight, psInput.VSOut, psInput.ViewDirectionT, material);
	float3 lightingColor = DirectionalLighting(DirLight, psInput.VSOut, EyePosPS, material);

    return float4(lightingColor, 1.0f);
}

//float4 PSMain_ParallaxOcclusionMapping(VSOutput_POM psInput) : SV_Target
//{
//    VSOutput input = psInput;
//    input.UV = ParallaxOcclusionMapping(psInput.VSOut.PosW, psInput.VSOut.UV, 0.01f);
//    Material material = ApplyMaterial(RawMat, input);
	
//    float3 lightingColor = DirectionalLighting(DirLight, psInput.VSOut, EyePosPS.xyz, material);

//    return float4(lightingColor, 1.0f);
//}

//float4 PSMain_ParallaxMappingWithOffsetLimit(VSOutput_POM psInput) : SV_Target
//{
//    VSOutput input = psInput;

//    float3 N = psInput.VSOut.NormalW;
//    float3 T = normalize(psInput.VSOut.TangentW - dot(psInput.VSOut.TangentW, N) * N);
//    float3 B = cross(N, T);
//    float3x3 TBN = float3x3(T, B, N);
//    float3 viewDirT = normalize(mul(TBN, EyePosPS - psInput.VSOut.PosW));
//    float3 lightDirT = normalize(mul(-DirLight.Direction.xyz, TBN));

//    input.UV = ParallaxMappingWithOffsetLimit(psInput, viewDirT, psInput.VSOut.UV, HeightScale);

//    Material material = ApplyMaterial(RawMat, input);
	
//    DirectionalLight dirLight = DirLight;
//    dirLight.Direction = float4(lightDirT, 1.0f);
//    float3 lightingColor = DirectionalLighting(dirLight, psInput.VSOut, viewDirT, material);

//    return float4(lightingColor, 1.0f);
//}