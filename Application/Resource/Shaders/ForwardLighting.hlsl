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

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
    output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.TangentW = mul(vsInput.Tangent, (float3x3)World);
    output.UV = vsInput.UV;

    return output;
}

float4 PSMain_HemisphericAmbient(VSOutput psInput) : SV_Target
{
    float3 ambientClr = HemisphericAmbientLighting(normalize(psInput.NormalW), float3(1.0f, 1.0f, 1.0f), AmbientLowerClr.rgb, AmbientRange.xyz);

    return float4(ambientClr, 1.0f);
}

float4 PSMain_Directional(VSOutput psInput) : SV_Target
{
    float3 ambientClr = HemisphericAmbientLighting(normalize(psInput.NormalW), float3(1.0f, 1.0f, 1.0f), AmbientLowerClr.rgb, AmbientRange.xyz);

    float3 directionalClr = DirectionalLighting(DirLight, psInput, EyePos.xyz, Mat);

    float3 result = ambientClr + directionalClr;

    return float4(result, 1.0f);
}

float4 PSMain_Point(VSOutput psInput) : SV_Target
{
    float3 finalClr = float3(0.0f, 0.0f, 0.0f);

	[unroll]
    for (uint i = 0; i < 3; ++i)
    {
        finalClr += PointLighting(PLight[i], psInput, EyePos.xyz, Mat);
    }

    return float4(finalClr, 1.0f);
}

float4 PSMain_Spot(VSOutput psInput) : SV_Target
{
    float3 ambientClr = HemisphericAmbientLighting(normalize(psInput.NormalW), float3(1.0f, 1.0f, 1.0f), AmbientLowerClr.rgb, AmbientRange.xyz);

    return float4(ambientClr, 1.0f);
}