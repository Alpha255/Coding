#include "TurnOnTheLight.hlsli"

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

    Material RawMat;
    DirectionalLight DirLight;
    PointLight PLights[3];
    SpotLight SLights[3];
}

struct VSInput
{
    float3 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    ///float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
    output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    ///output.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain_HemisphericAmbient(VSOutput psInput) : SV_Target
{
    float3 ambientClr = HemisphericAmbientLighting(normalize(psInput.NormalW), float3(1.0f, 1.0f, 1.0f), AmbientLowerClr.rgb, AmbientRange.xyz);

    ambientClr = GammaCorrection(ambientClr);
    return float4(ambientClr, 1.0f);
}

float4 PSMain_Directional(VSOutput psInput) : SV_Target
{
    float3 ambientClr = HemisphericAmbientLighting(normalize(psInput.NormalW), float3(1.0f, 1.0f, 1.0f), AmbientLowerClr.rgb, AmbientRange.xyz);

    Material material = RawMat;
    material.Normal = float4(normalize(psInput.NormalW), 0.0f);
    float3 directionalClr = DirectionalLighting(DirLight, psInput, EyePos.xyz, material);

    directionalClr += ambientClr;

    directionalClr = GammaCorrection(directionalClr);
    return float4(directionalClr, 1.0f);
}

float4 PSMain_Point(VSOutput psInput) : SV_Target
{
	float3 ambientClr = HemisphericAmbientLighting(normalize(psInput.NormalW), float3(1.0f, 1.0f, 1.0f), AmbientLowerClr.rgb, AmbientRange.xyz);

    float3 pointClr = float3(0.0f, 0.0f, 0.0f);

    Material material = RawMat;
    material.Normal = float4(normalize(psInput.NormalW), 0.0f);

	[unroll]
    for (uint i = 0; i < 3; ++i)
    {
        pointClr += PointLighting(PLights[i], psInput, EyePos.xyz, material);
    }

    pointClr += ambientClr;

    pointClr = GammaCorrection(pointClr);
    return float4(pointClr, 1.0f);
}

float4 PSMain_Spot(VSOutput psInput) : SV_Target
{
    float3 ambientClr = HemisphericAmbientLighting(normalize(psInput.NormalW), float3(1.0f, 1.0f, 1.0f), AmbientLowerClr.rgb, AmbientRange.xyz);

    float3 spotClr = float3(0.0f, 0.0f, 0.0f);

    Material material = RawMat;
    material.Normal = float4(normalize(psInput.NormalW), 0.0f);

   	[unroll]
    for (uint i = 0; i < 3; ++i)
    {
        spotClr += SpotLighting(SLights[i], psInput, EyePos.xyz, material);
    }

    spotClr += ambientClr;

    spotClr = GammaCorrection(spotClr);
    return float4(spotClr, 1.0f);
}