#include "CommonLighting.hlsli"

cbuffer cbVS
{
    matrix WVP;
    matrix World;
    matrix WorldInverse;
}

cbuffer cbPS
{
    float4 AmbientLowerClr;
    float4 AmbientRange;

    float4 EyePos;

    Material Mat;
    DirectionalLight DirLight;
    PointLight PLight[3];
}

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
    output.Normal = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.UV = vsInput.UV;

    return output;
}

float3 HemisphericAmbientLighting(float3 normal, float3 color)
{
    float up = normal.y * 0.5f + 0.5f;

    float3 ambient = (AmbientLowerClr + up * AmbientRange).rgb;

    return ambient * color;
}

float3 DirectionalLighting(float3 position, float3 normal)
{
	/// Phong diffuse
    float3 DirToLight = -DirLight.Direction.xyz;
    float3 NDotL = dot(DirToLight, normal);
    float3 result = DirLight.Diffuse.rgb * saturate(NDotL);

	/// Blinn specular
    float3 ToEye = EyePos.xyz - position;
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(ToEye + DirToLight);
    float NDotH = saturate(dot(HalfWay, normal));
    result += DirLight.Diffuse.rgb * pow(NDotH, DirLight.Specular.w) * DirLight.Specular.xyz;

    return result * /*Mat.Diffuse.rgb*/float3(1.0f, 1.0f, 1.0f);
}

float3 PointLighting(PointLight pLight, float3 position, float3 normal)
{
    float3 ToLight = pLight.Position.xyz - position;
    float3 ToEye = EyePos.xyz - position;
    float DistToLight = length(ToLight);
   
	// Phong diffuse
    ToLight /= DistToLight; // Normalize
    float NDotL = saturate(dot(ToLight, normal));
    float3 finalColor = pLight.Diffuse.rgb * NDotL;
   
	// Blinn specular
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(ToEye + ToLight);
    float NDotH = saturate(dot(HalfWay, normal));
    finalColor += pLight.Diffuse.rgb * pow(NDotH, Mat.Specular.w) * Mat.Specular.rgb;
   
	// Attenuation
    float DistToLightNorm = 1.0 - saturate(DistToLight * pLight.Range);
    float Attn = DistToLightNorm * DistToLightNorm;
    finalColor *= Mat.Diffuse.rgb * Attn;
   
    return finalColor;
}

float4 PSMain_HemisphericAmbient(VSOutput psInput) : SV_Target
{
    float3 ambientClr = HemisphericAmbientLighting(normalize(psInput.Normal), float3(1.0f, 1.0f, 1.0f));

    return float4(ambientClr, 1.0f);
}

float4 PSMain_Directional(VSOutput psInput) : SV_Target
{
    float3 ambientClr = HemisphericAmbientLighting(normalize(psInput.Normal), float3(1.0f, 1.0f, 1.0f));

    float3 directionalClr = DirectionalLighting(psInput.PosW, normalize(psInput.Normal));

    float3 result = ambientClr + directionalClr;

    return float4(result, 1.0f);
}

float4 PSMain_Point(VSOutput psInput) : SV_Target
{
    float3 finalClr = float3(0.0f, 0.0f, 0.0f);

	[unroll]
    for (uint i = 0; i < 3; ++i)
    {
        finalClr += PointLighting(PLight[i], psInput.PosW, normalize(psInput.Normal));
    }

    return float4(finalClr, 1.0f);
}

float4 PSMain_Spot(VSOutput psInput) : SV_Target
{
    float3 ambientClr = HemisphericAmbientLighting(normalize(psInput.Normal), float3(1.0f, 1.0f, 1.0f));

    return float4(ambientClr, 1.0f);
}