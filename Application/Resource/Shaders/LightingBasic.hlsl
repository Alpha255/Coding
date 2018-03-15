#include "CommonLighting.hlsli"

cbuffer ConstantBuffer
{
	matrix World;
	matrix WorldInverse;
	matrix WVP;
	
	float4 EyePos;
	DirectionalLight DirLight;
	Material Mat;
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
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

VSOutput VSMain(VSInput vsInput)
{
	VSOutput vsOutput;

    vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);

	float3 normal = mul(vsInput.Normal, (float3x3)WorldInverse);

	float4 ambient = DirLight.Ambient * Mat.Ambient;

	vsOutput.Color = ambient;

	float diffuseFactor = dot(DirLight.Direction.xyz, normal);

	if (diffuseFactor > 0.0f)
	{
		float4 posW = mul(float4(vsInput.Pos, 1.0f), World);
		float3 toEye = normalize(EyePos - posW);

		float4 diffuse = DirLight.Diffuse * Mat.Diffuse * diffuseFactor;
		
		float3 refVec = reflect(DirLight.Direction.xyz, normal);
        float specFactor = pow(max(dot(refVec, toEye), 0.0f), Mat.Specular.w);
        float4 specular = specFactor * Mat.Specular * DirLight.Specular;

		vsOutput.Color += diffuse;
		vsOutput.Color += specular;
    }

	return vsOutput;
}

float4 PSMain(VSOutput psInput) : SV_Target
{
	return psInput.Color;
}
