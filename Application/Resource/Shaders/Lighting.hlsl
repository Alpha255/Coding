#include "CommonLighting.hlsli"

cbuffer cbVS
{
    matrix World;
    matrix WorldInverseTranspose;
    matrix WVP;
};

cbuffer cbPS
{
    float3 EyePos;
    uint LightCount;

    DirectionalLight DirLights[3];

    Material Mat;
};

struct VSInput
{
    float3 Pos    : POSITION;
    float3 Normal : NORMAL;
};

struct VSOutput
{
    float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.PosW = mul(float4(input.Pos, 1.0f), World).xyz;
    output.NormalW = mul(input.Normal, (float3x3)WorldInverseTranspose);

    output.PosH = mul(float4(input.Pos, 1.0f), WVP);

    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    input.NormalW = normalize(input.NormalW);

    float3 toEye = normalize(EyePos - input.PosW);

    float4 ambient = float4(.0f, .0f, .0f, .0f);
    float4 diffuse = float4(.0f, .0f, .0f, .0f);
    float4 specular = float4(.0f, .0f, .0f, .0f);

	[unroll]
    for (int i = 0; i < LightCount; ++i)
    {
        float4 outAmbient, outDiffuse, outSpecular;
        ComputeDirectionalLight(Mat, DirLights[i], input.NormalW, toEye, outAmbient, outDiffuse, outSpecular);

        ambient += outAmbient;
        diffuse += outDiffuse;
        specular += outSpecular;
    }

    float4 litClr = ambient + diffuse + specular;
    litClr.a = Mat.Diffuse.a;
        
    return litClr;
}