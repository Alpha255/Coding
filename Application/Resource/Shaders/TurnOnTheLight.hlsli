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
	float4 Diffuse;

	float4 Specular;
	
	float3 Normal;
    uint UsingRawNormal;

	float4 Reflection;
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

struct DirectionalLight
{
	float4 Direction;

	float4 Ambient;

	float4 Diffuse;

    float3 Specular;
	float SpecularIntensity;
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
    Material materialOut;

	float4 texDiffuse = DiffuseMap.Sample(Sampler, psInput.UV);
	materialOut.Diffuse = materialIn.Diffuse + texDiffuse;

	float4 texSpecular = SpecularMap.Sample(Sampler, psInput.UV);
	materialOut.Specular = materialIn.Specular + texSpecular;

    if (materialIn.UsingRawNormal)
    {
		materialOut.Normal = normalize(psInput.NormalW);
    }
    else
    {
		float3 normalMap = NormalMap.Sample(Sampler, psInput.UV).rgb;
        materialOut.Normal = normalize(UnpackNormal(normalMap, normalize(psInput.NormalW), normalize(psInput.TangentW)));
    }

    return materialOut;
}

float3 PointLighting(in PointLight light, in VSOutput psInput, in float3 eyePos, in Material material)
{
    Material finalMaterial = ApplyMaterial(material, psInput);

    float3 ToLight = light.Position - psInput.PosW;
    float3 ToEye = eyePos - psInput.PosW;
    float DistToLight = length(ToLight);
   
	/// Phong diffuse
    ToLight /= DistToLight; // Normalize
    float NDotL = saturate(dot(ToLight, finalMaterial.Normal.xyz));
    float3 finalColor = light.Diffuse.rgb * NDotL;
   
	/// Blinn specular
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(ToEye + ToLight);
    float NDotH = saturate(dot(HalfWay, finalMaterial.Normal.xyz));
    finalColor += light.Diffuse.rgb * pow(NDotH, light.SpecularIntensity) * finalMaterial.Specular.rgb;
   
	/// Attenuation
    ///float DistToLightNorm = 1.0 - saturate(DistToLight * light.Range);
    ///float Attn = DistToLightNorm * DistToLightNorm;
	float Attn = 1.0f / dot(light.Attenuation.xyz, float3(1.0f, DistToLight, DistToLight * DistToLight));
    ///float Attn = 1.0f - DistToLight / light.Range;
    finalColor *= finalMaterial.Diffuse.rgb * Attn;
   
    return finalColor;
}

float3 DirectionalLighting(DirectionalLight light, in VSOutput psInput, in float3 eyePos, in Material material)
{
	Material finalMaterial = ApplyMaterial(material, psInput);

	/// Phong diffuse
	float3 ToLight = -light.Direction.xyz;
    float NDotL = saturate(dot(ToLight, finalMaterial.Normal.xyz));
    float3 finalColor = light.Diffuse.rgb * NDotL;
   
	/// Blinn specular
    float3 ToEye = normalize(eyePos - psInput.PosW);
    float3 HalfWay = normalize(ToEye + ToLight);
    float NDotH = saturate(dot(HalfWay, finalMaterial.Normal.xyz));
    finalColor += light.Diffuse.rgb * pow(NDotH, light.SpecularIntensity) * finalMaterial.Specular.rgb;

	finalColor *= finalMaterial.Diffuse.rgb;

	return finalColor;
}

float3 SpotLighting()
{

}

float3 HemisphericAmbientLighting(in float3 vertexNormal, in float3 diffuseColor, in float3 ambientColorLower, in float3 ambientRange)
{
    float up = vertexNormal.y * 0.5f + 0.5f;

    float3 ambient = (ambientColorLower + up * ambientRange).rgb;

    return ambient * diffuseColor;
}

float3 GammaCorrection(in float3 rawColor)
{
	return pow(rawColor, 1.0f / 2.2f);	
}