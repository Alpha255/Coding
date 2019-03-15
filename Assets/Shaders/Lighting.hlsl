#include "TurnOnTheLight.hlsli"

cbuffer cbVS
{
	matrix World;
	matrix WorldInverse;
	matrix WVP;
	
	float4 EyePos;
    DirectionalLight DirLight;
	Material Mat;
};

cbuffer cbPS
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

float3 DirectiaonalLighting_Flat(in DirectionalLight light, in float3 vertexNormal, in Material material)
{
    /// Diffuse
	float3 ToLight = -light.Direction.xyz;
    float NDotL = saturate(dot(ToLight, vertexNormal));
    float3 finalColor = light.Diffuse.rgb * NDotL;

	finalColor *= material.Diffuse.rgb;

	return finalColor;
}

float3 DirectionalLighting_Phong(DirectionalLight light, in float3 vertexPos, in float3 vertexNormal, in float3 eyePos, in Material material)
{
	/// Phong diffuse
    float3 ToLight = normalize(-light.Direction.xyz);
    float NDotL = saturate(dot(ToLight, vertexNormal));
    float3 finalColor = light.Diffuse.rgb * NDotL;
   
	/// Phong specular
	float3 reflection = reflect(-ToLight, vertexNormal);
    float3 ToEye = normalize(eyePos - vertexPos);
    finalColor += light.Diffuse.rgb * pow(saturate(dot(reflection, ToEye)), light.SpecularIntensity) * material.Specular.rgb;

	finalColor *= material.Diffuse.rgb;

	return finalColor;
}

VSOutputFlatGouraud VSMainFlat(VSInput vsInput)
{
	VSOutputFlatGouraud vsOutput;

    vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);

	float3 normal = normalize(mul(vsInput.Normal, (float3x3)WorldInverse));

	float3 lightColor = DirectiaonalLighting_Flat(DirLight, normal, Mat);

	vsOutput.Color = DirLight.Ambient + float4(lightColor, 1.0f);

	return vsOutput;
}

float4 PSMainFlat(VSOutputFlatGouraud psInput) : SV_Target
{
	return psInput.Color;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VSOutputFlatGouraud VSMainGouraud(VSInput vsInput)
{
	VSOutputFlatGouraud vsOutput;

    vsOutput.Pos = mul(float4(vsInput.Pos, 1.0f), WVP);

	float3 posW = mul(vsInput.Pos, (float3x3)World);
	float3 normal = mul(vsInput.Normal, (float3x3)WorldInverse);

	float3 lightingColor = DirectionalLighting_Phong(DirLight, posW, normalize(normal), EyePos.xyz, Mat);

	vsOutput.Color = DirLight.Ambient + float4(lightingColor, 1.0f);

	return vsOutput;
}

float4 PSMainGouraud(VSOutputFlatGouraud psInput) : SV_Target
{
	return psInput.Color;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    float3 lightingColor = DirectionalLighting_Phong(DirLightPS, psInput.PosW, normalize(psInput.NormalW), EyePosPS.xyz, MatPS);

    return float4(lightingColor, 1.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VSOutput VSMainBlinnPhong(VSInput vsInput)
{
    VSOutput output;
    output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
    output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMainBlinnPhong(VSOutput psInput) : SV_Target
{
    Material material = MatPS;
    material.Normal = float4(normalize(psInput.NormalW), 1.0f);

    float3 lightingColor = DirectionalLighting(DirLightPS, psInput.PosW, EyePosPS.xyz, material);

	lightingColor += DirLightPS.Ambient.xyz;

	return float4(lightingColor, 1.0f);
}
