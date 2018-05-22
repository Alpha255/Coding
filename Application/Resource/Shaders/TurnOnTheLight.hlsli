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
	
	float3 Diffuse;
    uint UsingRawDiffuse;

	float3 Specular;
    uint UsingRawSpecular;
	
	float3 Normal;
    uint UsingRawNormal;
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

    if (0 == materialIn.UsingRawDiffuse)
    {
        materialOut.Diffuse = DiffuseMap.Sample(Sampler, psInput.UV);
    }
    if (0 == materialIn.UsingRawSpecular)
    {
        materialOut.Specular = SpecularMap.Sample(Sampler, psInput.UV);
    }
    if (0 == materialIn.UsingRawNormal)
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

float3 PointLighting(in PointLight light, in VSOutput psInput, in float3 eyePos, in Material material)
{
    Material finalMaterial = ApplyMaterial(material, psInput);

    float3 ToLight = light.Position - psInput.PosW;
    float3 ToEye = eyePos - psInput.PosW;
    float DistToLight = length(ToLight);
   
	// Phong diffuse
    ToLight /= DistToLight; // Normalize
    float NDotL = saturate(dot(ToLight, finalMaterial.Normal.xyz));
    float3 finalColor = light.Diffuse.rgb * NDotL;
   
	// Blinn specular
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(ToEye + ToLight);
    float NDotH = saturate(dot(HalfWay, finalMaterial.Normal.xyz));
    finalColor += light.Diffuse.rgb * pow(NDotH, light.SpecularIntensity) * finalMaterial.Specular.rgb;
   
	// Attenuation
    ///float DistToLightNorm = 1.0 - saturate(DistToLight * light.Range);
    ///float Attn = DistToLightNorm * DistToLightNorm;
    float Attn = 1.0f - DistToLight / light.Range;
    finalColor *= finalMaterial.Diffuse.rgb * Attn;

    float3 ambient = light.Ambient.rgb * finalMaterial.Ambient.rgb;
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