Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;
SamplerState Sampler;

struct VSOutput
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 UV : TEXCOORD;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Normal;
	
	uint UsingRaw[4];	
};

struct PointLight
{
	float3 Position;
    float Range;

    float4 Ambient;
    float4 Diffuse;

    float3 Specular;
	float SpecularIntensity;

	float4 Attenuation;
};

float3 UnpackNormal(float3 normalMap, float3 unitNormalW, float3 tangentW)
{
	/// Uncompress each component from [0,1] to [-1,1].
    float3 normalT = 2.0f * normalMap - 1.0f;

	/// Build orthonormal basis.
    float3 N = unitNormalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

	/// Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}

Material ApplyMaterial(in Material materialIn, in VSOutput psInput)
{
	Material materialOut = materialIn;

	if (0 == materialIn.UsingRaw[1])
	{
		materialOut.Diffuse = DiffuseMap.Sample(Sampler, psInput.UV);
	}
	if (0 == materialIn.UsingRaw[2])
	{
		materialOut.Specular = SpecularMap.Sample(Sampler, psInput.UV);
	}
	if (0 == materialIn.UsingRaw[3])
	{
		float3 normalMap = NormalMap.Sample(Sampler, psInput.UV).rgb;
		materialOut.Normal.xyz = normalize(UnpackNormal(normalMap, normalize(psInput.NormalW), normalize(psInput.TangentW)));
	}
	else
	{
		materialOut.Normal.xyz = normalize(psInput.NormalW);
	}

	return materialOut;
}

float3 PointLighting(in PointLight light, in VSOutput psInput, in float3 eyePos, in Material materialIn)
{
	Material materialOut = ApplyMaterial(materialIn, psInput);

    float3 ToLight = light.Position - psInput.PosW;
    float3 ToEye = eyePos - psInput.PosW;
    float DistToLight = length(ToLight);
   
	// Phong diffuse
    ToLight /= DistToLight; // Normalize
    float NDotL = saturate(dot(ToLight, materialOut.Normal.xyz));
    float3 finalColor = light.Diffuse.rgb * NDotL;
   
	// Blinn specular
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(ToEye + ToLight);
    float NDotH = saturate(dot(HalfWay, materialOut.Normal.xyz));
    finalColor += light.Diffuse.rgb * pow(NDotH, light.SpecularIntensity) * materialOut.Specular.rgb;
   
	// Attenuation
    float DistToLightNorm = 1.0 - saturate(DistToLight * light.Range);
    float Attn = DistToLightNorm * DistToLightNorm;
    finalColor *= materialOut.Diffuse.rgb * Attn;

	float3 ambient = light.Ambient.rgb * materialOut.Ambient.rgb;
	finalColor += ambient;
   
    return finalColor;
}

float3 DirectionalLighting()
{

}

float3 SpotLighting()
{

}

float3 HemisphericAmbientLighting()
{

}