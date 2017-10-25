#include "CommonLighting.hlsli"

cbuffer cbVS
{
    matrix World;
    matrix WorldInverse;
    matrix WVP;
};

cbuffer cbPS
{
    float3 EyePos;
    uint EnableReflection;

    DirectionalLight DirLights[3];
    Material Mat;
};

Texture2D DiffuseTex;
TextureCube CubemapTex;

SamplerState SamplerAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 4;
    AddressU = WRAP;
    AddressV = WRAP;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct VSOutput
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 UV : TEXCOORD;
};

VSOutput VS_Main(VSInput input)
{
    VSOutput output;

    output.PosW = mul(float4(input.Pos, 1.0f), World).xyz;
    output.NormalW = mul(input.Normal, (float3x3)WorldInverse);

    output.PosH = mul(float4(input.Pos, 1.0f), WVP);

    output.UV = input.UV;

    return output;
}

float4 PS_Main(VSOutput input) : SV_Target
{
    input.NormalW = normalize(input.NormalW);

    float3 toEye = EyePos - input.PosW;

    float disToEye = length(toEye);

    ///toEye = normalize(toEye);
    toEye /= disToEye;

    float4 texClr = float4(1.0f, 1.0f, 1.0f, 1.0f);
    texClr = DiffuseTex.Sample(SamplerAnisotropic, input.UV);
    clip(texClr.a - 0.1f);

    float4 litClr = texClr;

    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll]
    for (int i = 0; i < 3; ++i)
    {
        float4 A, D, S;
        ComputeDirectionalLight(Mat, DirLights[i], input.NormalW, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        spec += S;
    }

    litClr = texClr * (ambient + diffuse) + spec;

    if (EnableReflection == 1)
    {
        float3 reflectionVector = reflect(-toEye, input.NormalW);
        float4 reflectionColor = CubemapTex.Sample(SamplerAnisotropic, reflectionVector);

        litClr += Mat.Reflect * reflectionColor;
    }

    litClr.a = Mat.Diffuse.a * texClr.a;

    return litClr;
}