struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Sepcular;
    float4 Direction;
};

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Reflect;
};

cbuffer cbVS
{
    matrix World;
    matrix WorldInverseTranspose;
    matrix WVP;
};

cbuffer cbPS
{
    float4 EyePos;

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

float4 PSMain(VSOutput input/*, uniform int s_LightCount*/) : SV_Target
{
    float4 litClr = Mat.Diffuse;

    return litClr;
}