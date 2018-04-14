#include "CommonLighting.hlsli"

cbuffer cbVS
{
    matrix World;
    matrix WorldInverse;
    matrix WVP;
};

cbuffer cbPS
{
    float4 EyePos;

    DirectionalLight DirLight;
    Material Mat;
};

Texture2D DiffuseTex : register(t0);
TextureCube CubemapTex : register(t1);

SamplerState LinearSampler;

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
    float3 NormalW : NORMAL;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    output.PosW = mul(float4(input.Pos, 1.0f), World).xyz;
    output.NormalW = mul(input.Normal, (float3x3)WorldInverse);

    output.PosH = mul(float4(input.Pos, 1.0f), WVP);

    output.UV = input.UV;

    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    input.NormalW = normalize(input.NormalW);

    float3 toEye = EyePos.xyz - input.PosW;

    float disToEye = length(toEye);

    ///toEye = normalize(toEye);
    toEye /= disToEye;

    float4 texClr = float4(1.0f, 1.0f, 1.0f, 1.0f);
    texClr = DiffuseTex.Sample(LinearSampler, input.UV);
    clip(texClr.a - 0.1f);

    float4 litClr = texClr;

    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll]
    for (int i = 0; i < 1; ++i)
    {
        float4 A, D, S;
        ComputeDirectionalLight(Mat, DirLight, input.NormalW, toEye, A, D, S);

        ambient += A;
        diffuse += D;
        spec += S;
    }

    litClr = texClr * (ambient + diffuse) + spec;

    float3 reflectionVector = reflect(-toEye, input.NormalW);
    float4 reflectionColor = CubemapTex.Sample(LinearSampler, reflectionVector);

    litClr += Mat.Reflection * reflectionColor;

    litClr.a = Mat.Diffuse.a * texClr.a;

    return litClr;
}