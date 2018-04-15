#include "CommonLighting.hlsli"

cbuffer cbVS
{
    matrix World;
    matrix WorldInverse;
    matrix WVP;
};

cbuffer cbPS
{
    PointLight PointLights[3];

    Material Mat;

    float3 EyePos;
    uint EnableGammaCorrection;
};

struct VSInput
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 UV : TEXCOORD;
};

Texture2D DiffuseMap;
SamplerState LinearSampler;

struct VSOutput
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 UV : TEXCOORD;
};

float3 BlinnPhongLighting(float3 i_Normal, float3 i_VertexPos, float3 i_EyePos, PointLight i_Light)
{
    /// diffuse
    float3 lightDir = normalize(i_Light.Position - i_VertexPos);
    float diff = max(dot(lightDir, i_Normal), 0.0);
    float3 diffuse = diff * i_Light.Diffuse.xyz;
    
	/// specular
    float3 viewDir = normalize(i_EyePos - i_VertexPos);
    float spec = 0.0;
    float3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(i_Normal, halfwayDir), 0.0), 64.0);
    float3 specular = spec * i_Light.Diffuse.xyz;
   
	 /// simple attenuation
    float max_distance = 1.5;
    float distance = length(i_Light.Position - i_VertexPos);
    float attenuation = 1.0 / distance * distance;
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    return diffuse + specular;
}

VSOutput VSMain(VSInput vsInput)
{
    VSOutput output;
    output.PosH = mul(float4(vsInput.Pos, 1.0f), WVP);
    output.PosW = mul(float4(vsInput.Pos, 1.0f), World).xyz;
    output.NormalW = mul(vsInput.Normal, (float3x3)WorldInverse);
    output.UV = vsInput.UV;

    return output;
}

//float4 PSMain(VSOutput psInput) : SV_Target
//{
//    float3 normal = normalize(psInput.NormalW);

//    float4 textureClr = DiffuseMap.Sample(LinearSampler, psInput.UV);
//    clip(textureClr.a - 0.1f);

//    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
//    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
//    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

//    [unroll]	
//    for (int i = 0; i < 3; ++i)
//    {
//        float4 a, d, s;
//        ComputePointLight(Mat, PointLights[i], normal, psInput.PosW, EyePos, a, d, s);

//        ambient += a;
//        diffuse += d;
//        specular += s;
//    }

//    float4 color = textureClr * (ambient + diffuse + specular);

//    if (1 == EnableGammaCorrection)
//    {
//        color = pow(color, 1.0f / 2.2f);
//    }

//    color.a = Mat.Diffuse.a * textureClr.a;
//    return color;
//}

float4 PSMain(VSOutput psInput) : SV_Target
{
    float3 normal = normalize(psInput.NormalW);

    float4 textureClr = DiffuseMap.Sample(LinearSampler, psInput.UV);

    float4 ambient = float4(0.1f, 0.1f, 0.1f, 1.0f);
    float3 lightingClr = float3(0.0f, 0.0f, 0.0f);
	[unroll]
    for (int i = 0; i < 3; ++i)
    {
        lightingClr += BlinnPhongLighting(normal, psInput.PosW, EyePos, PointLights[i]);
    }

    float4 color = textureClr * (float4(lightingClr, 0.0f) + ambient);

    if (1 == EnableGammaCorrection)
    {
        color = pow(color, 1.0f / 2.2f);
        ///color = pow(color, float4(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f, 0.0f));
    }

    color.a = Mat.Diffuse.a * textureClr.a;
    return color;
}