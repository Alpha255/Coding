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
	
	float4 Normal;

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

struct SpotLight
{
    float3 Position;
    float Range;

    float3 LookAt;
    float SpotCosOuterCone;

    float3 Attenuation;
    float SpotCosInnerCone;

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

///Material ApplyMaterial(in Material materialIn, in VSOutput psInput)
///{
///    Material materialOut = materialIn;
///	   materialOut.Diffuse = DiffuseMap.Sample(Sampler, psInput.UV);
///	   materialOut.Specular = SpecularMap.Sample(Sampler, psInput.UV);

///	   materialOut.Normal = float4(normalize(psInput.NormalW), 0.0f);

///	   float3 normalMap = NormalMap.Sample(Sampler, psInput.UV).rgb;
///    materialOut.Normal.xyz = normalize(UnpackNormal(normalMap, normalize(psInput.NormalW), normalize(psInput.TangentW)));
///    materialOut.Normal.w = 0.0f;

///    return materialOut;
///}

float3 PointLighting(in PointLight light, in VSOutput psInput, in float3 eyePos, in Material material)
{
    float3 ToLight = light.Position - psInput.PosW;
    float3 ToEye = eyePos - psInput.PosW;
    float DistToLight = length(ToLight);
   
	/// Phong diffuse
    ToLight /= DistToLight; // Normalize
    float NDotL = saturate(dot(ToLight, material.Normal.xyz));
    float3 lightingColor = light.Diffuse.rgb * NDotL;
   
	/// Blinn specular
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(ToEye + ToLight);
    float NDotH = saturate(dot(HalfWay, material.Normal.xyz));
    lightingColor += light.Diffuse.rgb * pow(NDotH, light.SpecularIntensity) * material.Specular.rgb;
   
	/// Attenuation
#if 1
    float DistToLightNorm = 1.0 - saturate(DistToLight * 1.0f / light.Range);
    float Attn = DistToLightNorm * DistToLightNorm;
#else
	float Attn = 1.0f / dot(light.Attenuation.xyz, float3(1.0f, DistToLight, DistToLight * DistToLight));
#endif
    lightingColor *= material.Diffuse.rgb * Attn;
   
    return lightingColor;
}

float3 DirectionalLighting(in DirectionalLight light, in VSOutput psInput, in float3 eyePos, in Material material)
{
	/// Phong diffuse
    float3 ToLight = normalize(-light.Direction.xyz);
    float NDotL = saturate(dot(ToLight, material.Normal.xyz));
    float3 lightingColor = light.Diffuse.rgb * NDotL;
   
	/// Blinn specular
    float3 ToEye = normalize(eyePos - psInput.PosW);
    float3 HalfWay = normalize(ToEye + ToLight);
    float NDotH = saturate(dot(HalfWay, material.Normal.xyz));
    lightingColor += light.Diffuse.rgb * pow(NDotH, light.SpecularIntensity) * material.Specular.rgb;

    lightingColor *= material.Diffuse.rgb;

    return lightingColor;
}

float3 SpotLighting(in SpotLight light, in VSOutput psInput, in float3 eyePos, in Material material)
{
    float3 ToLight = light.Position - psInput.PosW;
    float3 ToEye = eyePos - psInput.PosW;
    float DistToLight = length(ToLight);
   
	/// Phong diffuse
    ToLight /= DistToLight; /// Normalize
    float NDotL = saturate(dot(ToLight, material.Normal.xyz));
    float3 lightingColor = light.Diffuse.rgb * NDotL;
   
	/// Blinn specular
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(ToEye + ToLight);
    float NDotH = saturate(dot(HalfWay, material.Normal.xyz));
    lightingColor += light.Diffuse.rgb * pow(NDotH, light.SpecularIntensity) * material.Specular.rgb;

	/// Cone attenuation
    const float PI = 3.14159f;
    float cosAngle = dot(normalize(light.LookAt - light.Position), ToLight);
    float coneAttOuter = cos(PI * light.SpotCosOuterCone / 180.0f);
    float coneAttInner = 1.0f / cos(PI * light.SpotCosInnerCone / 180.0f);
    float coneAtt = saturate((cosAngle - coneAttOuter) * coneAttInner);
    coneAtt *= coneAtt;
   
	/// Attenuation
#if 1
    float DistToLightNorm = 1.0 - saturate(DistToLight * 1.0f / light.Range);
    float Attn = DistToLightNorm * DistToLightNorm;
#else
	float Attn = 1.0f / dot(light.Attenuation.xyz, float3(1.0f, DistToLight, DistToLight * DistToLight));
#endif
    lightingColor *= material.Diffuse.rgb * Attn * coneAtt;
   
    return lightingColor;
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