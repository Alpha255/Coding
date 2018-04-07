#include "CommonLighting.hlsli"

cbuffer ConstantBufferVS
{
	matrix World;
	matrix WorldInverse;
	matrix WVP;
	
	float4 EyePos;
    DirectionalLight DirLight;
	Material Mat;
};

cbuffer ConstantBufferPS
{	
	float4 EyePosPS;
    DirectionalLight DirLightPS;
	Material MatPS;
};

struct VSInput
{
	float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct VSOutputFlatGouraud
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

VSOutputFlatGouraud VSMainFlat(VSInput vsInput)
{
	VSOutputFlatGouraud vsOutput;

    vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);

	float3 normal = normalize(mul(vsInput.Normal, (float3x3)WorldInverse));

	float4 ambient = DirLight.Ambient * Mat.Ambient;

    float3 lightDir = normalize(-DirLight.Direction.xyz);

    float4 diffuse = DirLight.Diffuse * Mat.Diffuse * dot(lightDir, normal);

	vsOutput.Color = ambient + diffuse;

	return vsOutput;
}

float4 PSMainFlat(VSOutputFlatGouraud psInput) : SV_Target
{
	return psInput.Color;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VSOutputFlatGouraud VSMainGouraud(VSInput vsInput)
{
	VSOutputFlatGouraud vsOutput;

    vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);

	float3 normal = normalize(mul(vsInput.Normal, (float3x3)WorldInverse));

    float3 lightDir = normalize(-DirLight.Direction.xyz);

    float4 diffuse = DirLight.Diffuse * Mat.Diffuse * max(dot(lightDir, normal), 0.0f);

	float3 viewDir = normalize(EyePos.xyz - mul(float4(vsInput.Pos, 1.0f), World).xyz);
    float3 reflection = reflect(-lightDir, normal);
    float specFactor = pow(max(dot(reflection, viewDir), 0.0f), Mat.Specular.w);
	float4 specular = DirLight.Specular * Mat.Specular * specFactor;

	float4 ambient = DirLight.Ambient * Mat.Ambient;

	vsOutput.Color = ambient + diffuse + specular;
	vsOutput.Color.w = Mat.Diffuse.w;

	return vsOutput;
}

float4 PSMainGouraud(VSOutputFlatGouraud psInput) : SV_Target
{
	return psInput.Color;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct VSOutputPhong
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
};

VSOutputPhong VSMainPhong(VSInput vsInput)
{
	VSOutputPhong vsOutput;

    vsOutput.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
	vsOutput.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
	vsOutput.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);

	return vsOutput;
}

float4 PSMainPhong(VSOutputPhong psInput) : SV_Target
{
	float4 ambient = DirLightPS.Ambient * MatPS.Ambient;

	float3 normal = normalize(psInput.NormalW);

    float3 lightDir = normalize(-DirLightPS.Direction.xyz);

    float4 diffuse = DirLightPS.Diffuse * MatPS.Diffuse * max(dot(lightDir, normal), 0.0f);

	float3 viewDir = normalize(EyePosPS.xyz - psInput.PosW);
    float3 reflection = reflect(-lightDir, normal);
    float specFactor = pow(max(dot(reflection, viewDir), 0.0f), MatPS.Specular.w);
	float4 specular = DirLightPS.Specular * MatPS.Specular * specFactor;

	float4 Color = ambient + diffuse + specular;
	Color.w = MatPS.Diffuse.w;

	return Color;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VSOutputPhong VSMainBlinnPhong(VSInput vsInput)
{
	VSOutputPhong vsOutput;

    vsOutput.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
	vsOutput.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
	vsOutput.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);

	return vsOutput;
}

float4 PSMainBlinnPhong(VSOutputPhong psInput) : SV_Target
{
	float4 ambient = DirLightPS.Ambient * MatPS.Ambient;

	float3 normal = normalize(psInput.NormalW);

    float3 lightDir = normalize(-DirLightPS.Direction.xyz);

    float4 diffuse = DirLightPS.Diffuse * MatPS.Diffuse * max(dot(lightDir, normal), 0.0f);

	float3 viewDir = normalize(EyePosPS.xyz - psInput.PosW);
    float3 halfwayVec = normalize(viewDir + lightDir);
    float specFactor = pow(max(dot(halfwayVec, normal), 0.0f), MatPS.Specular.w);
	float4 specular = DirLightPS.Specular * MatPS.Specular * specFactor;

	float4 Color = ambient + diffuse + specular;
	Color.w = MatPS.Diffuse.w;

	return Color;
}
